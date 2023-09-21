#include "Graphics.h"

#include "Component\Camera.h"
#include "BindableObject\DepthStencil.h"
#include "..\Exception\GraphicsException.h"
#include "DX\GPUAdapter.h"
#include "DX\DXDevice.h"
#include "DX\CommandQueue.h"
#include "DX\SwapChain.h"
#include "DX\Viewport.h"
#include "DX\ScissorRects.h"
#include "DX\CommandLIst.h"
#include "DX\DescriptorAllocator.h"
#include "DX\DescriptorAllocation.h"
#include "..\DiveBomberCore.h"
#include "..\Window\Window.h"

#include <iostream>
#include <array>

namespace DiveBomber::DEGraphics
{
	using namespace DX;
	using namespace DEException;
	using namespace BindableObject;
	using namespace Component;

	std::unique_ptr<Graphics> Graphics::instance;

	Graphics::Graphics()
		:
		hWnd(DEWindow::Window::GetInstance().GetHandle()),
		width(MainWindowWidth),
		height(MainWindowHeight)
	{
		// Check for DirectX Math library support.
		if (!DirectX::XMVerifyCPUSupport())
		{
			MessageBoxA(nullptr, "Failed to verify DirectX Math library support.", "Error", MB_OK | MB_ICONERROR);
			return;
		}

		gpuAdapter = std::make_unique<GPUAdapter>();
		dxDevice = std::make_unique<DXDevice>(gpuAdapter->GetAdapter());
	}

	Graphics::~Graphics()
	{
		Flush();
	}

	void Graphics::PostInitializeGraphics()
	{
		fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
		assert(fenceEvent && "Failed to create fence event.");

		cbvSrvUavDescriptorHeap = std::make_shared<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 10'000u);
		rtvDescriptorHeap = std::make_shared<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 50u);
		dsvDescriptorHeap = std::make_shared<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 50u);
		samplerDescriptorHeap = std::make_shared<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 1024u);

		directCommandQueue = std::make_unique<CommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT);
		computeCommandQueue = std::make_unique<CommandQueue>(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		copyCommandQueue = std::make_unique<CommandQueue>(D3D12_COMMAND_LIST_TYPE_COPY);
		swapChain = std::make_unique<SwapChain>(hWnd, directCommandQueue->GetCommandQueue());
		swapChain->UpdateBackBuffer(rtvDescriptorHeap);
		viewport = std::make_unique<Viewport>();
		scissorRects = std::make_unique<ScissorRects>();

		std::shared_ptr<DescriptorAllocation> dsvHandle = dsvDescriptorHeap->Allocate(1u);
		mainDS = std::make_shared<DepthStencil>(width, height, std::move(dsvHandle), 0u);
	}

	void Graphics::BeginFrame()
	{
		auto currentBackBufferIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();
		auto backBuffer = swapChain->GetBackBuffer(currentBackBufferIndex);

		wrl::ComPtr<ID3D12GraphicsCommandList7> commandList = GetGraphicsCommandList();

		// Clear the render target.
		{
			GetCommandList()->AddTransitionBarrier(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, true);

			D3D12_CPU_DESCRIPTOR_HANDLE rtv = swapChain->GetBackBufferDescriptorHandle(currentBackBufferIndex);

			FLOAT clearColor[] = ClearMainRTColor;
			commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

			mainDS->ClearDepth(commandList);
		}

		viewport->Bind(commandList);
		scissorRects->Bind(commandList);
	}

	void Graphics::EndFrame()
	{
		auto currentBackBufferIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();
		auto backBuffer = swapChain->GetBackBuffer(currentBackBufferIndex);

		auto commandQueue = GetCommandQueue();
		wrl::ComPtr<ID3D12GraphicsCommandList7> commandList = GetGraphicsCommandList();

		// Present
		{
			GetCommandList()->AddTransitionBarrier(backBuffer, D3D12_RESOURCE_STATE_PRESENT, true);

			frameFenceValues[currentBackBufferIndex] = ExecuteCommandList();

			HRESULT hr;
			bool enableVSync = VSync;
			UINT syncInterval = enableVSync ? 1 : 0;
			UINT presentFlags = swapChain->CheckTearingSupport() && !enableVSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
			GFX_THROW_INFO(swapChain->GetSwapChain()->Present(syncInterval, presentFlags));

			currentBackBufferIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();

			commandQueue->WaitForFenceValue(frameFenceValues[currentBackBufferIndex]);
		}
	}

	HANDLE Graphics::GetFenceEvent() const noexcept
	{
		return fenceEvent;
	}

	void Graphics::ReSizeMainRT(const uint32_t inputWidth, const uint32_t inputHeight)
	{
		width = inputWidth;
		height = inputHeight;

		// Flush the GPU queue to make sure the swap chain's back buffers
		// are not being referenced by an in-flight command list.
		Flush();
		
		for (int i = 0; i < SwapChainBufferCount; ++i)
		{
			frameFenceValues[i] = frameFenceValues[swapChain->GetSwapChain()->GetCurrentBackBufferIndex()];
		}

		swapChain->ResetSizeBackBuffer(width, height, rtvDescriptorHeap);

		mainDS->Resize(width, height);

		renderCamera->ResizeAspectRatio(width, height);

		viewport->ResizeViewport(width, height);
	}

	UINT Graphics::GetWidth() const noexcept
	{
		return width;
	}

	UINT Graphics::GetHeight() const noexcept
	{
		return height;
	}

	std::shared_ptr<CommandQueue> Graphics::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) const noexcept
	{
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			return directCommandQueue;
			break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			return computeCommandQueue;
			break;
		case D3D12_COMMAND_LIST_TYPE_COPY:
			return copyCommandQueue;
			break;
		default:
			assert(false && "Invalid command queue type.");
			return nullptr;
		}
	}

	void Graphics::Flush() const noexcept
	{
		directCommandQueue->Flush();
		computeCommandQueue->Flush();
		copyCommandQueue->Flush();
	}

	wrl::ComPtr<ID3D12Device10> Graphics::GetDevice() const noexcept
	{
		return dxDevice->GetDevice();
	}

	std::shared_ptr<RenderTarget> Graphics::GetCurrentBackBuffer() const noexcept
	{
		return swapChain->GetCurrentBackBuffer();
	}

	std::shared_ptr<DepthStencil> Graphics::GetMainDS() const noexcept
	{
		return mainDS;
	}

	std::shared_ptr<CommandList> Graphics::GetCommandList(const D3D12_COMMAND_LIST_TYPE type) noexcept
	{
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			if (directCommandList)
				return directCommandList;
			else
			{
				directCommandList = GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)->GetCommandList();
				return directCommandList;
			}
			break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			if (computeCommandList)
				return computeCommandList;
			else
			{
				computeCommandList = GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE)->GetCommandList();
				return computeCommandList;
			}
			break;
		case D3D12_COMMAND_LIST_TYPE_COPY:
			if (copyCommandList)
				return copyCommandList;
			else
			{
				copyCommandList = GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY)->GetCommandList();
				return copyCommandList;
			}
			break;
		default:
			assert(false && "Invalid command queue type.");
			return nullptr;
		}
	}

	wrl::ComPtr<ID3D12GraphicsCommandList7> Graphics::GetGraphicsCommandList(const D3D12_COMMAND_LIST_TYPE type) noexcept
	{
		return GetCommandList(type)->GetGraphicsCommandList();
	}

	void Graphics::SetCamera(const std::shared_ptr<Camera> camera) noexcept
	{
		renderCamera = camera;
	}

	DirectX::XMMATRIX Graphics::GetCameraMatrix() const noexcept
	{
		return renderCamera->GetMatrix();
	}

	DirectX::XMMATRIX Graphics::GetProjetionMatrix() const noexcept
	{
		return renderCamera->GetProjection(GetWidth(), GetHeight());
	}

	float Graphics::GetFOV() const noexcept
	{
		return renderCamera->GetFOV();
	}

	uint64_t Graphics::ExecuteCommandList(const D3D12_COMMAND_LIST_TYPE type)
	{
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			return directCommandList ? directCommandQueue->ExecuteCommandList(std::move(directCommandList)) : 0;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			return computeCommandList ? computeCommandQueue->ExecuteCommandList(std::move(computeCommandList)) : 0;
		case D3D12_COMMAND_LIST_TYPE_COPY:
			return copyCommandList ? copyCommandQueue->ExecuteCommandList(std::move(copyCommandList)) : 0;
		default:
			assert(false && "Invalid command queue type.");
			return -1;
		}
	}

	uint64_t Graphics::ExecuteCommandList(std::shared_ptr<CommandList> commandList, const D3D12_COMMAND_LIST_TYPE type)
	{
		assert(commandList->GetCommandListType() == type);

		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			return directCommandQueue->ExecuteCommandList(std::move(commandList));
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			return computeCommandQueue->ExecuteCommandList(std::move(commandList));
		case D3D12_COMMAND_LIST_TYPE_COPY:
			return copyCommandQueue->ExecuteCommandList(std::move(commandList));
		default:
			assert(false && "Invalid command queue type.");
			return -1;
		}
	}

	uint64_t Graphics::ExecuteCommandLists(std::vector<std::shared_ptr<CommandList>> commandLists, const D3D12_COMMAND_LIST_TYPE type)
	{
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			return directCommandQueue->ExecuteCommandLists(std::move(commandLists));
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			return computeCommandQueue->ExecuteCommandLists(std::move(commandLists));
		case D3D12_COMMAND_LIST_TYPE_COPY:
			return copyCommandQueue->ExecuteCommandLists(std::move(commandLists));
		default:
			assert(false && "Invalid command queue type.");
			return -1;
		}
	}

	std::shared_ptr<DescriptorAllocator> Graphics::GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type) const noexcept
	{
		switch (type)
		{
		case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
			return cbvSrvUavDescriptorHeap;
		case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
			return rtvDescriptorHeap;
		case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
			return dsvDescriptorHeap;
		case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
			return samplerDescriptorHeap;
		default:
			assert(false && "Invalid descriptor heap type.");
			return 0;
		}
	}

	void Graphics::BindShaderDescriptorHeaps()
	{
		const std::array<const std::shared_ptr<DescriptorAllocator>, 2u> shaderVisibleDescriptorHeaps = {
			cbvSrvUavDescriptorHeap,
			samplerDescriptorHeap,
		};

		std::vector<ID3D12DescriptorHeap*> descriptorHeaps{};
	#if BindlessRendering
		descriptorHeaps.reserve(shaderVisibleDescriptorHeaps.size());
	#endif
		for (const auto& heaps : shaderVisibleDescriptorHeaps)
		{
			auto descriptorHeapsClip = heaps->GetAllDescriptorHeaps();
			for (const auto& heap : descriptorHeapsClip)
			{
				descriptorHeaps.emplace_back(heap.Get());
			}
		};

		GetGraphicsCommandList()->SetDescriptorHeaps(static_cast<UINT>(descriptorHeaps.size()), descriptorHeaps.data());
	}

	Graphics& Graphics::GetInstance()
	{
		if (instance == nullptr)
		{
			instance = std::make_unique<Graphics>();
		}
		return *instance;
	}

	void Graphics::Destructor() noexcept
	{
		if (instance != nullptr)
		{
			instance.reset();
		}
	}
}
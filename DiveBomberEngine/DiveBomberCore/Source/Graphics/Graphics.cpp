#include "Graphics.h"

#include "Component\Camera.h"
#include "BindObj\DepthStencil.h"
#include "..\Exception\GraphicsException.h"
#include "DX\GPUAdapter.h"
#include "DX\DXDevice.h"
#include "DX\CommandQueue.h"
#include "DX\SwapChain.h"
#include "DX\DescriptorHeap.h"
#include "DX\Viewport.h"
#include "DX\ScissorRects.h"

#include <iostream>

namespace DiveBomber::DEGraphics
{
	using namespace DX;
	using namespace DEException;
	using namespace BindObj;

	Graphics::Graphics(HWND inputHWnd, UINT includeWidth, UINT includeHeight)
	{
		// Check for DirectX Math library support.
		if (!DirectX::XMVerifyCPUSupport())
		{
			MessageBoxA(nullptr, "Failed to verify DirectX Math library support.", "Error", MB_OK | MB_ICONERROR);
			return;
		}

		hWnd = inputHWnd;
		width = includeWidth;
		height = includeHeight;

		gpuAdapter = std::make_unique<GPUAdapter>();
		dxDevice = std::make_unique<DXDevice>(gpuAdapter->GetAdapter());

		fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
		assert(fenceEvent && "Failed to create fence event.");

		directCommandQueue = std::make_unique<CommandQueue>(dxDevice->GetDecive(), D3D12_COMMAND_LIST_TYPE_DIRECT);
		computeCommandQueue = std::make_unique<CommandQueue>(dxDevice->GetDecive(), D3D12_COMMAND_LIST_TYPE_COMPUTE);
		copyCommandQueue = std::make_unique<CommandQueue>(dxDevice->GetDecive(), D3D12_COMMAND_LIST_TYPE_COPY);
		swapChain = std::make_unique<SwapChain>(hWnd, directCommandQueue->GetCommandQueue());
		swapChain->UpdateBackBuffer(dxDevice->GetDecive());
		viewport = std::make_unique<Viewport>();
		scissorRects = std::make_unique<ScissorRects>();

		std::shared_ptr<DescriptorHeap> dsHeap = std::make_shared<DescriptorHeap>(dxDevice->GetDecive(),
			D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1u);
		mainDS = std::make_shared<DepthStencil>(dxDevice->GetDecive(), width, height, std::move(dsHeap), 0u);
	}

	Graphics::~Graphics()
	{
		Flush();
	}

	void Graphics::BeginFrame()
	{
		auto currentBackBufferIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();
		auto backBuffer = swapChain->GetBackBuffer(currentBackBufferIndex);

		auto commandQueue = GetCommandQueue();
		directCommandList = commandQueue->GetCommandList();
		wrl::ComPtr<ID3D12GraphicsCommandList2> commandList = directCommandList;

		// Clear the render target.
		{
			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				backBuffer.Get(),
				D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

			commandList->ResourceBarrier(1, &barrier);

			CD3DX12_CPU_DESCRIPTOR_HANDLE rtv = swapChain->GetBackBufferDescriptorHandle(currentBackBufferIndex);

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
		wrl::ComPtr<ID3D12GraphicsCommandList2> commandList = directCommandList;

		// Present
		{
			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				backBuffer.Get(),
				D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			commandList->ResourceBarrier(1, &barrier);

			frameFenceValues[currentBackBufferIndex] = commandQueue->ExecuteCommandList(commandList);

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

		swapChain->ResetSizeBackBuffer(GetDecive(), width, height);

		mainDS->Resize(GetDecive(), width, height);

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

	wrl::ComPtr<ID3D12Device2> Graphics::GetDecive() const noexcept
	{
		return dxDevice->GetDecive();
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE Graphics::GetRenderTargetDescriptorHandle() const noexcept
	{
		return swapChain->GetBackBufferDescriptorHandle();
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE Graphics::GetDepthStencilDescriptorHandle() const noexcept
	{
		return mainDS->GetDescriptorHandle();
	}

	wrl::ComPtr<ID3D12GraphicsCommandList2> Graphics::GetCommandList(const D3D12_COMMAND_LIST_TYPE type) noexcept
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
			return nullptr;
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

	void Graphics::SetCamera(const std::shared_ptr<Component::Camera> camera) noexcept
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
}
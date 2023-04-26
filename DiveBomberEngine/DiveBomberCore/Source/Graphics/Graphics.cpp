#include "Graphics.h"

namespace DiveBomber::DEGraphics
{
	using namespace DX;
	using namespace DEException;

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

		fenceManager = std::make_shared<FenceManager>(dxDevice->GetDecive());
		fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
		assert(fenceEvent && "Failed to create fence event.");

		commandManager = std::make_unique<CommandManager>(dxDevice->GetDecive(), fenceManager);
		SCRTDesHeap = std::make_unique<DescriptorHeap>(dxDevice->GetDecive(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		swapChain = std::make_unique<SwapChain>(hWnd, commandManager->GetCommandQueue());
		swapChain->UpdateMainRT(dxDevice->GetDecive(), SCRTDesHeap->GetDescriptorHeap());
	}

	Graphics::~Graphics()
	{
		Flush();
	}

	void Graphics::BeginFrame()
	{
		auto currentBackBufferIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();
		auto backBuffer = swapChain->GetBackBuffer(currentBackBufferIndex);

		commandList = commandManager->GetCommandList();

		// Clear the render target.
		{
			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				backBuffer,
				D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

			commandList->ResourceBarrier(1, &barrier);

			CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(SCRTDesHeap->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
				currentBackBufferIndex, dxDevice->GetDecive()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

			FLOAT clearColor[] = ClearMainRTColor;
			commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
		}
	}

	void Graphics::EndFrame()
	{
		auto currentBackBufferIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();
		auto backBuffer = swapChain->GetBackBuffer(currentBackBufferIndex);

		// Present
		{
			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				backBuffer,
				D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			commandList->ResourceBarrier(1, &barrier);

			auto fenceValue = commandManager->ExecuteCommandList(commandList.Get());

			HRESULT hr;
			bool enableVSync = VSync;
			UINT syncInterval = enableVSync ? 1 : 0;
			UINT presentFlags = swapChain->CheckTearingSupport() && !enableVSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
			GFX_THROW_INFO(swapChain->GetSwapChain()->Present(syncInterval, presentFlags));

			commandManager->WaitForFenceValue(fenceValue);
		}
	}

	HANDLE Graphics::GetFenceEvent() noexcept
	{
		return fenceEvent;
	}

	void Graphics::ReSizeMainRT(uint32_t inputWidth, uint32_t inputHeight)
	{
		// Don't allow 0 size swap chain back buffers.
		width = std::max(1u, inputWidth);
		height = std::max(1u, inputHeight);

		// Flush the GPU queue to make sure the swap chain's back buffers
		// are not being referenced by an in-flight command list.
		Flush();

		for (int i = 0; i < SwapChainBufferCount; ++i)
		{
			// Any references to the back buffers must be released
			// before the swap chain can be resized.
			wrl::ComPtr<ID3D12Resource> a;
			swapChain->ResetBackBuffer(i);
			frameFenceValues[i] = frameFenceValues[swapChain->GetSwapChain()->GetCurrentBackBufferIndex()];
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

		HRESULT hr;
		GFX_THROW_INFO(swapChain->GetSwapChain()->GetDesc(&swapChainDesc));
		GFX_THROW_INFO(swapChain->GetSwapChain()->ResizeBuffers(SwapChainBufferCount, width, height,
			swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

		swapChain->UpdateMainRT(dxDevice->GetDecive(), SCRTDesHeap->GetDescriptorHeap());
	}

	UINT Graphics::GetWidth() const noexcept
	{
		return width;
	}

	UINT Graphics::GetHeight() const noexcept
	{
		return height;
	}

	CommandManager* Graphics::GetCommandManager() noexcept
	{
		return commandManager.get();
	}

	void Graphics::Flush() noexcept
	{
		commandManager->Flush();
	}
}
#include "Graphics.h"

Graphics::Graphics(HWND inputHWnd, UINT includeWidth, UINT includeHeight)
{
	hWnd = inputHWnd;
	width = includeWidth;
	height = includeHeight;

	gpuAdapter = std::make_unique<GPUAdapter>();
	dxDevice = std::make_unique<DXDevice>(gpuAdapter->GetAdapter());

	fenceManager = std::make_shared<FenceManager>(dxDevice->GetDecive());
	fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent && "Failed to create fence event.");

	commandQueue = std::make_unique<CommandQueue>(dxDevice->GetDecive());
	SCRTDesHeap = std::make_unique<DescriptorHeap>(dxDevice->GetDecive(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	swapChain = std::make_unique<SwapChain>(hWnd, commandQueue->GetCommandQueue());
	swapChain->UpdateMainRT(dxDevice->GetDecive(), SCRTDesHeap->GetDescriptorHeap());
	swapChain->CreateComandAllocator(dxDevice->GetDecive(), D3D12_COMMAND_LIST_TYPE_DIRECT);
	commandList = std::make_unique<CommandList>(dxDevice->GetDecive(),
		swapChain->GetCommandAllocator(swapChain->GetSwapChain()->GetCurrentBackBufferIndex()),
		D3D12_COMMAND_LIST_TYPE_DIRECT);
}

Graphics::~Graphics()
{

}

uint64_t Graphics::Signal()
{
	uint64_t fenceValueForSignal = ++frameFenceValues[swapChain->GetSwapChain()->GetCurrentBackBufferIndex()];

	HRESULT hr;
	GFX_THROW_INFO(commandQueue->GetCommandQueue()->Signal(fenceManager->GetFence(), fenceValueForSignal));

	return fenceValueForSignal;
}

void Graphics::WaitForFenceValue(std::chrono::milliseconds duration)
{
	auto currentBackBufferIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();
	if (fenceManager->GetFence()->GetCompletedValue() < frameFenceValues[currentBackBufferIndex])
	{
		HRESULT hr;
		GFX_THROW_INFO(fenceManager->GetFence()->SetEventOnCompletion(frameFenceValues[currentBackBufferIndex], fenceEvent));
		::WaitForSingleObject(fenceEvent, static_cast<DWORD>(duration.count()));
	}
}

void Graphics::Flush() noexcept
{
	uint64_t fenceValueForSignal = Signal();
	WaitForFenceValue();
}

bool Graphics::CheckTearingSupport()
{
	BOOL allowTearing = FALSE;

	// Rather than create the DXGI 1.5 factory interface directly, we create the
	// DXGI 1.4 interface and query for the 1.5 interface. This is to enable the 
	// graphics debugging tools which will not support the 1.5 factory interface 
	// until a future update.
	wrl::ComPtr<IDXGIFactory4> factory4;
	if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
	{
		wrl::ComPtr<IDXGIFactory5> factory5;
		if (SUCCEEDED(factory4.As(&factory5)))
		{
			if (FAILED(factory5->CheckFeatureSupport(
				DXGI_FEATURE_PRESENT_ALLOW_TEARING,
				&allowTearing, sizeof(allowTearing))))
			{
				allowTearing = FALSE;
			}
		}
	}

	return allowTearing == TRUE;
}

void Graphics::BeginFrame()
{
	auto currentBackBufferIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();
	auto commandAllocator = swapChain->GetCommandAllocator(currentBackBufferIndex);
	auto backBuffer = swapChain->GetBackBuffer(currentBackBufferIndex);

	commandAllocator->Reset();
	commandList->GetCommandList()->Reset(commandAllocator, NULL);

	// Clear the render target.
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer,
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		commandList->GetCommandList()->ResourceBarrier(1, &barrier);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(SCRTDesHeap->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
			currentBackBufferIndex, dxDevice->GetDecive()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

		FLOAT clearColor[] = ClearMainRTColor;
		commandList->GetCommandList()->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
	}
}

void Graphics::EndFrame()
{
	auto currentBackBufferIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();
	auto commandAllocator = swapChain->GetCommandAllocator(currentBackBufferIndex);
	auto backBuffer = swapChain->GetBackBuffer(currentBackBufferIndex);

	// Present
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer,
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		commandList->GetCommandList()->ResourceBarrier(1, &barrier);

		HRESULT hr;
		GFX_THROW_INFO(commandList->GetCommandList()->Close());

		ID3D12CommandList* const commandLists[] = {
			commandList->GetCommandList()
		};
		commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(commandLists), commandLists);

		frameFenceValues[currentBackBufferIndex] = Signal();

		bool enableVSync = VSync;
		UINT syncInterval = enableVSync ? 1 : 0;
		UINT presentFlags = CheckTearingSupport() && !enableVSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
		GFX_THROW_INFO(swapChain->GetSwapChain()->Present(syncInterval, presentFlags));

		WaitForFenceValue();
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
#include "Graphics.h"

Graphics::Graphics(HWND inputHWnd)
{
	hWnd = inputHWnd;
	gpuAdapter = std::make_unique<GPUAdapter>();
	dxDevice = std::make_unique<DXDevice>(gpuAdapter->GetAdapter());
	commandQueue = std::make_unique<CommandQueue>(dxDevice->GetDecive());
	SCRTDesHeap = std::make_unique<DescriptorHeap>(dxDevice->GetDecive(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	swapChain = std::make_unique<SwapChain>(hWnd, commandQueue->GetCommandQueue());
	swapChain->UpdateMainRT(dxDevice->GetDecive(), SCRTDesHeap->GetDescriptorHeap());
	swapChain->CreateComandAllocator(dxDevice->GetDecive(), D3D12_COMMAND_LIST_TYPE_DIRECT);
	commandList = std::make_unique<CommandList>(dxDevice->GetDecive(),
		swapChain->GetCommandAllocator(swapChain->GetSwapChain()->GetCurrentBackBufferIndex()),
		D3D12_COMMAND_LIST_TYPE_DIRECT);
	fence = std::make_unique<Fence>(dxDevice->GetDecive());

	fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent && "Failed to create fence event.");
}

Graphics::~Graphics()
{

}

uint64_t Graphics::Signal()
{
	uint64_t fenceValueForSignal = ++fenceValue;

	HRESULT hr;
	GFX_THROW_INFO(commandQueue->GetCommandQueue()->Signal(fence->GetFence(), fenceValueForSignal));

	return fenceValueForSignal;
}

void Graphics::WaitForFenceValue(std::chrono::milliseconds duration)
{
	if (fence->GetFence()->GetCompletedValue() < fenceValue)
	{
		HRESULT hr;
		GFX_THROW_INFO(fence->GetFence()->SetEventOnCompletion(fenceValue, fenceEvent));
		::WaitForSingleObject(fenceEvent, static_cast<DWORD>(duration.count()));
	}
}

void Graphics::Flush() noexcept
{
	uint64_t fenceValueForSignal = Signal();
	WaitForFenceValue();
}
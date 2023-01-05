#pragma once
#include "WindowResource.h"
#include "GraphicsResource.h"
class SwapChain final
{
public:
	SwapChain(HWND hWnd, ID3D12CommandQueue* commandQueue);
	bool CheckTearingSupport();
	IDXGISwapChain4* GetSwapChain() noexcept;
	void UpdateMainRT(ID3D12Device2* device, ID3D12DescriptorHeap* SWRTDesHeap);
	ID3D12Resource* GetBackBuffer(int i) noexcept;
	void ResetBackBuffer(int i) noexcept;
private:
	wrl::ComPtr<IDXGISwapChain4> swapChain;
	wrl::ComPtr<ID3D12Resource> backBuffers[SwapChainBufferCount];
};

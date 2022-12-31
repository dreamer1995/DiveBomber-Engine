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
	void CreateComandAllocator(ID3D12Device2* device, D3D12_COMMAND_LIST_TYPE type);
	ID3D12Resource* GetBackBuffer(int i) noexcept;
	void ResetBackBuffer(int i) noexcept;
	ID3D12CommandAllocator* GetCommandAllocator(int i) noexcept;
private:
	wrl::ComPtr<IDXGISwapChain4> swapChain;
	wrl::ComPtr<ID3D12Resource> backBuffers[SwapChainBufferCount];
	wrl::ComPtr<ID3D12CommandAllocator> commandAllocators[SwapChainBufferCount];
};

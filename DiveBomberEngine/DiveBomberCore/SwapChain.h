#pragma once
#include "WindowResource.h"
#include "GraphicsResource.h"

class SwapChain
{
public:
	SwapChain(HWND hWnd, ID3D12CommandQueue* commandQueue);
	bool CheckTearingSupport();
	IDXGISwapChain4* GetSwapChain() noexcept;
private:
	wrl::ComPtr<IDXGISwapChain4> swapChain;
};

#pragma once
#include "..\GraphicsResource.h"

namespace DiveBomber::DX
{
	class SwapChain final
	{
	public:
		SwapChain(HWND hWnd, wrl::ComPtr<ID3D12CommandQueue> commandQueue);
		bool CheckTearingSupport();
		wrl::ComPtr<IDXGISwapChain4> GetSwapChain() noexcept;
		void UpdateMainRT(wrl::ComPtr<ID3D12Device2> device, wrl::ComPtr<ID3D12DescriptorHeap> SWRTDesHeap);
		wrl::ComPtr<ID3D12Resource> GetBackBuffer(int i) noexcept;
		void ResetBackBuffer(int i) noexcept;
	private:
		wrl::ComPtr<IDXGISwapChain4> swapChain;
		wrl::ComPtr<ID3D12Resource> backBuffers[SwapChainBufferCount];
	};
}
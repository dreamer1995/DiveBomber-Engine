#pragma once
#include "..\GraphicsResource.h"

namespace DiveBomber::DX
{
	class SwapChain final
	{
	public:
		SwapChain(const HWND hWnd, const wrl::ComPtr<ID3D12CommandQueue> commandQueue);
		[[nodiscard]] bool CheckTearingSupport();
		[[nodiscard]] wrl::ComPtr<IDXGISwapChain4> GetSwapChain() noexcept;
		void UpdateMainRT(const wrl::ComPtr<ID3D12Device2> device, const wrl::ComPtr<ID3D12DescriptorHeap> SWRTDesHeap);
		[[nodiscard]] wrl::ComPtr<ID3D12Resource> GetBackBuffer(const int i) noexcept;
		void ResetBackBuffer(const int i) noexcept;
	private:
		wrl::ComPtr<IDXGISwapChain4> swapChain;
		wrl::ComPtr<ID3D12Resource> backBuffers[SwapChainBufferCount];
	};
}
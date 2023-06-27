#pragma once
#include "..\GraphicsResource.h"

namespace DiveBomber::BindObj
{
	class RenderTarget;
}

namespace DiveBomber::DX
{
	class DescriptorHeap;
	class SwapChain final
	{
	public:
		SwapChain(const HWND hWnd, const wrl::ComPtr<ID3D12CommandQueue> commandQueue);
		[[nodiscard]] bool CheckTearingSupport();
		[[nodiscard]] wrl::ComPtr<IDXGISwapChain4> GetSwapChain() noexcept;
		void UpdateBackBuffer(const wrl::ComPtr<ID3D12Device2> device);
		[[nodiscard]] wrl::ComPtr<ID3D12Resource> GetBackBuffer(const int i) noexcept;
		void ResetBackBuffer() noexcept;
		void ResetSizeBackBuffer(const wrl::ComPtr<ID3D12Device2> device,
			const uint32_t inputWidth, const uint32_t inputHeight);
		[[nodiscard]] CD3DX12_CPU_DESCRIPTOR_HANDLE GetBackBufferDescriptorHandle() const noexcept;
		[[nodiscard]] CD3DX12_CPU_DESCRIPTOR_HANDLE GetBackBufferDescriptorHandle(int i) const noexcept;
	private:
		wrl::ComPtr<IDXGISwapChain4> swapChain;
		//wrl::ComPtr<ID3D12Resource> backBuffers[SwapChainBufferCount];
		std::shared_ptr<BindObj::RenderTarget> backBuffers[SwapChainBufferCount];
		std::shared_ptr<DescriptorHeap> rtvDescHeap;
	};
}
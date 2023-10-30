#pragma once
#include "..\GraphicsHeader.h"

namespace DiveBomber::BindableObject
{
	class RenderTarget;
}

namespace DiveBomber::DX
{
	class DescriptorAllocator;
}

namespace DiveBomber::DX
{
	class SwapChain final
	{
	public:
		SwapChain(const HWND hWnd, const wrl::ComPtr<ID3D12CommandQueue> commandQueue,
			std::shared_ptr<DX::DescriptorAllocator> inputDescriptorAllocator);
		[[nodiscard]] bool CheckTearingSupport();
		[[nodiscard]] wrl::ComPtr<IDXGISwapChain4> GetSwapChain() noexcept;
		[[nodiscard]] std::shared_ptr<BindableObject::RenderTarget> GetCurrentBackBuffer() noexcept;
		[[nodiscard]] wrl::ComPtr<ID3D12Resource> GetBackBuffer(const int i) noexcept;
		void ResetBackBuffer() noexcept;
		void ResetSizeBackBuffer(const uint32_t inputWidth, const uint32_t inputHeight);
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetBackBufferDescriptorHandle() const noexcept;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetBackBufferDescriptorHandle(int i) const noexcept;

	private:
		void UpdateBackBuffer();

	private:
		wrl::ComPtr<IDXGISwapChain4> swapChain;
		//wrl::ComPtr<ID3D12Resource> backBuffers[SwapChainBufferCount];
		std::shared_ptr<BindableObject::RenderTarget> backBuffers[SwapChainBufferCount];
		std::shared_ptr<DX::DescriptorAllocator> descriptorAllocator;
	};
}
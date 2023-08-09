#pragma once
#include "Bindable.h"
#include "BindableTarget.h"
#include "..\GraphicsHeader.h"

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::DX
{
	class DescriptorHeap;
}

namespace DiveBomber::Component
{
	class DescriptorAllocation;
}

namespace DiveBomber::BindObj
{
	class RenderTarget;
	class DepthStencil : public Bindable, public BindableTarget
	{
	public:
		DepthStencil(DEGraphics::Graphics& gfx, UINT inputWidth, UINT inputHeight,
			std::shared_ptr<Component::DescriptorAllocation> inputDescriptorAllocation, UINT inputDepth = 0);
		DepthStencil(wrl::ComPtr<ID3D12Device2> device, UINT inputWidth, UINT inputHeight,
			std::shared_ptr<Component::DescriptorAllocation> inputDescriptorAllocation, UINT inputDepth = 0);

		void Bind(DEGraphics::Graphics& gfx) noxnd override;
		void BindTarget(DEGraphics::Graphics& gfx) noxnd override;
		void BindTarget(DEGraphics::Graphics& gfx, std::shared_ptr<BindableTarget> renderTarget) noxnd override;
		[[nodiscard]] wrl::ComPtr<ID3D12Resource> GetDepthStencilBuffer() const noexcept;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const noexcept;
		void ClearDepth(DEGraphics::Graphics& gfx, FLOAT clearDepth = 1.0f) const noexcept;
		void ClearDepth(wrl::ComPtr<ID3D12GraphicsCommandList2> commandList, FLOAT clearDepth = 1.0f) const noexcept;
		void Resize(DEGraphics::Graphics& gfx, const UINT inputWidth, const UINT inputHeight, const UINT inputDepth = 0);
		void Resize(wrl::ComPtr<ID3D12Device2> device, const UINT inputWidth, const UINT inputHeight, const UINT inputDepth = 0);
	private:
		UINT width;
		UINT height;
		UINT depth;
		wrl::ComPtr<ID3D12Resource> depthStencilBuffer;

		std::shared_ptr<Component::DescriptorAllocation> descriptorAllocation;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
		D3D12_CLEAR_VALUE optimizedClearValue;
		D3D12_DEPTH_STENCIL_VIEW_DESC dsv;
	};
}
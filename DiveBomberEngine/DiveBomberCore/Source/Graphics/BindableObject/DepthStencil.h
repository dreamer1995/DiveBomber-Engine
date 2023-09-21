#pragma once
#include "Bindable.h"
#include "BindableTarget.h"
#include "..\GraphicsHeader.h"

namespace DiveBomber::DX
{
	class DescriptorAllocation;
}

namespace DiveBomber::BindableObject
{
	class RenderTarget;
	class DepthStencil : public Bindable, public BindableTarget
	{
	public:
		DepthStencil(UINT inputWidth, UINT inputHeight,
			std::shared_ptr<DX::DescriptorAllocation> inputDescriptorAllocation, UINT inputDepth = 0);

		void Bind() noxnd override;
		void BindTarget() noxnd override;
		void BindTarget(std::shared_ptr<BindableTarget> renderTarget) noxnd override;
		[[nodiscard]] wrl::ComPtr<ID3D12Resource> GetDepthStencilBuffer() const noexcept;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const noexcept;
		void ClearDepth(FLOAT clearDepth = 1.0f) const noexcept;
		void ClearDepth(wrl::ComPtr<ID3D12GraphicsCommandList7> commandList, FLOAT clearDepth = 1.0f) const noexcept;
		void Resize(const UINT inputWidth, const UINT inputHeight, const UINT inputDepth = 0);

	private:
		UINT width;
		UINT height;
		UINT depth;
		wrl::ComPtr<ID3D12Resource> depthStencilBuffer;

		std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
		D3D12_CLEAR_VALUE optimizedClearValue;
		D3D12_DEPTH_STENCIL_VIEW_DESC dsv;
	};
}
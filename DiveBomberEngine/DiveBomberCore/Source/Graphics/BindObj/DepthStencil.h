#pragma once
#include "Bindable.h"
#include "BindableTarget.h"
#include "..\Graphics.h"

namespace DiveBomber::BindObj
{
	class RenderTarget;
	class DepthStencil : public Bindable, public BindableTarget
	{
	public:
		DepthStencil(DEGraphics::Graphics& gfx, UINT inputWidth, UINT inputHeight,
			std::shared_ptr<DX::DescriptorHeap> inputDescHeap, UINT inputDepth = 0);

		void Bind(DEGraphics::Graphics& gfx) noxnd override;
		void BindTarget(DEGraphics::Graphics& gfx) noxnd override;
		void BindTarget(DEGraphics::Graphics& gfx, std::shared_ptr<BindableTarget> renderTarget) noxnd override;
		[[nodiscard]] wrl::ComPtr<ID3D12Resource> GetDepthStencilBuffer() const noexcept;
		[[nodiscard]] CD3DX12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const noexcept;
		void ClearDepth(DEGraphics::Graphics& gfx, FLOAT depth = 1.0f) const noexcept;
	private:
		UINT width;
		UINT height;
		UINT depth;
		wrl::ComPtr<ID3D12Resource> depthStencilBuffer;
		std::shared_ptr<DX::DescriptorHeap> depthStencilDescHeap;
		CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle;
	};
}
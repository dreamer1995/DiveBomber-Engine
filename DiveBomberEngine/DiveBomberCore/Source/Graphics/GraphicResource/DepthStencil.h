#pragma once
#include "..\..\Resource.h"
#include "..\GraphicsHeader.h"

namespace DiveBomber::DX
{
	class DescriptorAllocator;
	class DescriptorAllocation;
}

namespace DiveBomber::GraphicResource
{
	class RenderTarget;
	class DepthStencil final: public Resource
	{
	public:
		DepthStencil(UINT inputWidth, UINT inputHeight,
			std::shared_ptr<DX::DescriptorAllocator> inputDescriptorAllocator);

		void BindTarget() noxnd;
		void BindTarget(std::shared_ptr<RenderTarget> renderTarget) noxnd;
		[[nodiscard]] wrl::ComPtr<ID3D12Resource> GetDepthStencilBuffer() const noexcept;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCPUDescriptorHandle() const noexcept;
		void ClearDepth(FLOAT clearDepth = 1.0f) const noexcept;
		void ClearDepth(wrl::ComPtr<ID3D12GraphicsCommandList7> commandList, FLOAT clearDepth = 1.0f) const noexcept;
		void Resize(const UINT inputWidth, const UINT inputHeight);

	private:
		UINT width;
		UINT height;
		wrl::ComPtr<ID3D12Resource> depthStencilBuffer;

		std::shared_ptr<DX::DescriptorAllocator> descriptorAllocator;
		std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation;
		D3D12_CPU_DESCRIPTOR_HANDLE dsvCPUHandle;
		D3D12_CLEAR_VALUE optimizedClearValue;
		D3D12_DEPTH_STENCIL_VIEW_DESC dsv;
	};
}
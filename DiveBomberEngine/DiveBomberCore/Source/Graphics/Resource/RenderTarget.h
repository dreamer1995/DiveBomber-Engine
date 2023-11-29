#pragma once
#include "Resource.h"
#include "..\GraphicsHeader.h"

namespace DiveBomber::DX
{
	class DescriptorAllocator;
	class DescriptorAllocation;
}

namespace DiveBomber::DEResource
{
	class DepthStencil;
	class RenderTarget : public Resource
	{
		// just for reference
		//enum class RenderTargetType
		//{
		//	RT_Default,
		//	RT_PreCalSimpleCube,
		//	RT_PreCalMipCube,
		//	RT_PreBRDFCoefficient,
		//	RT_GBuffer,
		//	RT_UVABuffer,
		//	RT_3D
		//};
	public:
		RenderTarget(wrl::ComPtr<ID3D12Resource> inputBuffer,
			std::shared_ptr<DX::DescriptorAllocator> inputRTVDescriptorAllocator);
		RenderTarget(std::shared_ptr<DX::DescriptorAllocator> inputRTVDescriptorAllocator,
			CD3DX12_RESOURCE_DESC inputDesc, bool updateRT = true);

		~RenderTarget();

		void BindTarget() noxnd;
		void BindTarget(std::shared_ptr<DepthStencil> depthStencil) noxnd;
		[[nodiscard]] wrl::ComPtr<ID3D12Resource> GetRenderTargetBuffer() const noexcept;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCPUDescriptorHandle() const noexcept;
		virtual void Resize(CD3DX12_RESOURCE_DESC inputDesc);
		void Resize(const wrl::ComPtr<ID3D12Resource> newbuffer);
		void ReleaseBuffer();

	protected:
		wrl::ComPtr<ID3D12Resource> renderTargetBuffer;

		std::shared_ptr<DX::DescriptorAllocator> rtvDescriptorAllocator;
		std::shared_ptr<DX::DescriptorAllocation> rtvDescriptorAllocation;
		D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUHandle;

		D3D12_CLEAR_VALUE optimizedClearValue;
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
		CD3DX12_RESOURCE_DESC resourceDesc;
	};
}
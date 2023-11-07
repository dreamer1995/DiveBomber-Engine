#pragma once
#include "Bindable.h"
#include "BindableTarget.h"
#include "..\GraphicsHeader.h"

namespace DiveBomber::DX
{
	class DescriptorAllocator;
	class DescriptorAllocation;
}

namespace DiveBomber::BindableObject
{
	class DepthStencil;
	class RenderTarget : public Bindable, public BindableTarget
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
		RenderTarget(UINT inputWidth, UINT inputHeight,
			std::shared_ptr<DX::DescriptorAllocator> inputRTVDescriptorAllocator,
			DXGI_FORMAT inputFormat = DXGI_FORMAT_R8G8B8A8_UNORM, UINT inputMipLevels = 0, bool updateRT = true);

		~RenderTarget();

		virtual void Bind() noxnd override;
		void BindTarget() noxnd override;
		void BindTarget(std::shared_ptr<BindableTarget> depthStencil) noxnd override;
		[[nodiscard]] wrl::ComPtr<ID3D12Resource> GetRenderTargetBuffer() const noexcept;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCPUDescriptorHandle() const noexcept;
		virtual void Resize(const UINT inputWidth, const UINT inputHeight);
		void Resize(wrl::ComPtr<ID3D12Resource> newbuffer);
		void ReleaseBuffer();

	protected:
		UINT width = 1u;
		UINT height = 1u;
		DXGI_FORMAT format;
		UINT mipLevels;
		wrl::ComPtr<ID3D12Resource> renderTargetBuffer;

		std::shared_ptr<DX::DescriptorAllocator> rtvDescriptorAllocator;
		std::shared_ptr<DX::DescriptorAllocation> rtvDescriptorAllocation;
		D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUHandle;

		D3D12_CLEAR_VALUE optimizedClearValue;
		D3D12_RENDER_TARGET_VIEW_DESC rsv;
	};
}
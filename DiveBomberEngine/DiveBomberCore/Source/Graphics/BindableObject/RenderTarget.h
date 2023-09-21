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
			std::shared_ptr<DX::DescriptorAllocation> inputDescriptorAllocation, UINT inputDepth = 0);
		RenderTarget(UINT inputWidth, UINT inputHeight,
			std::shared_ptr<DX::DescriptorAllocation> inputDescriptorAllocation, DXGI_FORMAT inputFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
			UINT inputMipLevels = 0, UINT inputDepth = 0);
		//RenderTarget(DEGraphics::Graphics& gfx, UINT inputWidth, UINT inputHeight, D3D12_DESCRIPTOR_HEAP_TYPE inputType,
		//	DXGI_FORMAT inputFormat = DXGI_FORMAT_B8G8R8A8_UNORM, UINT inputDepth = 0);

		~RenderTarget();

		void Bind() noxnd override;
		void BindTarget() noxnd override;
		void BindTarget(std::shared_ptr<BindableTarget> depthStencil) noxnd override;
		[[nodiscard]] wrl::ComPtr<ID3D12Resource> GetRenderTargetBuffer() const noexcept;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const noexcept;
		void Resize(const UINT inputWidth, const UINT inputHeight, const UINT inputDepth = 0);

	private:
		UINT width;
		UINT height;
		UINT depth;
		DXGI_FORMAT format;
		UINT mipLevels;
		wrl::ComPtr<ID3D12Resource> renderTargetBuffer;

		std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
		D3D12_CLEAR_VALUE optimizedClearValue;
		D3D12_RENDER_TARGET_VIEW_DESC rsv;
	};
}
#pragma once
#include "Bindable.h"
#include "BindableTarget.h"
#include "..\Graphics.h"

namespace DiveBomber::BindObj
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
		RenderTarget(DEGraphics::Graphics& gfx, wrl::ComPtr<ID3D12Resource> inputBuffer,
			std::shared_ptr<DX::DescriptorHeap> inputDescHeap, UINT inputDepth = 0);
		RenderTarget(DEGraphics::Graphics& gfx, UINT inputWidth, UINT inputHeight,
			std::shared_ptr<DX::DescriptorHeap> inputDescHeap, DXGI_FORMAT inputFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
			UINT inputMipLevels = 0, UINT inputDepth = 0);
		//RenderTarget(DEGraphics::Graphics& gfx, UINT inputWidth, UINT inputHeight, D3D12_DESCRIPTOR_HEAP_TYPE inputType,
		//	DXGI_FORMAT inputFormat = DXGI_FORMAT_B8G8R8A8_UNORM, UINT inputDepth = 0);

		void BindTarget(DEGraphics::Graphics& gfx) noxnd override;
		void BindTarget(DEGraphics::Graphics& gfx, std::shared_ptr<BindableTarget> depthStencil) noxnd override;
		[[nodiscard]] wrl::ComPtr<ID3D12Resource> GetRenderTargetBuffer() const noexcept;
		[[nodiscard]] CD3DX12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const noexcept;
	private:
		float width;
		float height;
		UINT depth;
		DXGI_FORMAT format;
		UINT mipLevels;
		wrl::ComPtr<ID3D12Resource> renderTargetBuffer;
		std::shared_ptr<DX::DescriptorHeap> renderTargetDescHeap;
		CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle{};
	};
}
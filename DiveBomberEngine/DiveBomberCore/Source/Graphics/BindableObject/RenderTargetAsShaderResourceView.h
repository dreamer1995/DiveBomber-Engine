#pragma once
#include "RenderTarget.h"

namespace DiveBomber::BindableObject
{
	class RenderTargetAsShaderResourceView final: public RenderTarget
	{
	public:
		RenderTargetAsShaderResourceView(UINT inputWidth, UINT inputHeight,
			std::shared_ptr<DX::DescriptorAllocator> inputRTVDescriptorAllocator,
			std::shared_ptr<DX::DescriptorAllocator> inputSRVDescriptorAllocator,
			DXGI_FORMAT inputFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
			UINT inputMipLevels = 0);

		~RenderTargetAsShaderResourceView();

		virtual void Bind() noxnd override;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle() const noexcept;
		void Resize(const UINT inputWidth, const UINT inputHeight);

	private:
		std::shared_ptr<DX::DescriptorAllocator> srvDescriptorAllocator;
		std::shared_ptr<DX::DescriptorAllocation> srvDescriptorAllocation;
		D3D12_CPU_DESCRIPTOR_HANDLE srvCPUHandle;

		D3D12_SHADER_RESOURCE_VIEW_DESC srv;
	};
}
#pragma once
#include "..\RenderTarget.h"
#include "ShaderInputable.h"

namespace DiveBomber::GraphicResource
{
	class RenderTargetAsShaderResourceView final: public RenderTarget, public ShaderInputable
	{
	public:
		RenderTargetAsShaderResourceView(
			std::shared_ptr<DX::DescriptorAllocator> inputRTVDescriptorAllocator,
			std::shared_ptr<DX::DescriptorAllocator> inputSRVDescriptorAllocator,
			CD3DX12_RESOURCE_DESC inputDesc);

		~RenderTargetAsShaderResourceView();

		//virtual void Bind() noxnd override;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle() const noexcept;
		void Resize(CD3DX12_RESOURCE_DESC inputDesc) override;
		void Resize(UINT width, UINT height) override;
		[[nodiscard]] UINT GetSRVDescriptorHeapOffset() const noexcept override;
		void BindAsShaderResource() noxnd override;

	private:
		std::shared_ptr<DX::DescriptorAllocator> srvDescriptorAllocator;
		std::shared_ptr<DX::DescriptorAllocation> srvDescriptorAllocation;
		D3D12_CPU_DESCRIPTOR_HANDLE srvCPUHandle;

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	};
}
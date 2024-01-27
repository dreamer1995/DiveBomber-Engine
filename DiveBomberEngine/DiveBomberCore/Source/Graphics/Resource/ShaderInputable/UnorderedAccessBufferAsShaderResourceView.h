#pragma once
#include "..\Resource.h"
#include "ShaderInputable.h"
#include "UnorderedAccessBuffer.h"
#include "..\..\GraphicsHeader.h"

namespace DiveBomber::DX
{
	class DescriptorAllocator;
	class DescriptorAllocation;
}

namespace DiveBomber::DEResource
{
	class UnorderedAccessBufferAsShaderResourceView final : public Resource, public ShaderInputable
	{
	public:
		UnorderedAccessBufferAsShaderResourceView(
			std::shared_ptr<DX::DescriptorAllocator> inputDescriptorAllocator,
			CD3DX12_RESOURCE_DESC inputDesc);

		~UnorderedAccessBufferAsShaderResourceView();

		void BindAsShaderResource() noxnd override;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle() const noexcept;
		void Resize(const CD3DX12_RESOURCE_DESC inputDesc);
		void Resize(UINT width, UINT height);

		[[nodiscard]] UINT GetSRVDescriptorHeapOffset() const noexcept override;

		[[nodiscard]] std::shared_ptr<UnorderedAccessBuffer> GetUAVPointer() noexcept;
	private:
		std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;

		D3D12_SHADER_RESOURCE_VIEW_DESC srv;

		std::shared_ptr<UnorderedAccessBuffer> uavPointer;
	};
}
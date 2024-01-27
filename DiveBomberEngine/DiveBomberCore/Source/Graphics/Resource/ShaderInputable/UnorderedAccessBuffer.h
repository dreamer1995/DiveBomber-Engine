#pragma once
#include "..\Resource.h"
#include "ShaderInputable.h"

#include "..\..\GraphicsHeader.h"

namespace DiveBomber::DX
{
	class DescriptorAllocator;
	class DescriptorAllocation;
}

namespace DiveBomber::DEResource
{
	class UnorderedAccessBuffer final : public Resource, public ShaderInputable
	{
	public:
		UnorderedAccessBuffer(
			std::shared_ptr<DX::DescriptorAllocator> inputDescriptorAllocator,
			CD3DX12_RESOURCE_DESC inputDesc);

		UnorderedAccessBuffer(
			std::shared_ptr<DX::DescriptorAllocator> inputDescriptorAllocator,
			wrl::ComPtr<ID3D12Resource> inputUAVBuffer,
			D3D12_UNORDERED_ACCESS_VIEW_DESC inputUAVDesc);

		~UnorderedAccessBuffer();

		[[nodiscard]] wrl::ComPtr<ID3D12Resource> GetUnorderedAccessBuffer() const noexcept;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetUAVCPUDescriptorHandle() const noexcept;
		void Resize(const CD3DX12_RESOURCE_DESC inputDesc);
		void Resize(UINT width, UINT height);
		void Resize(const wrl::ComPtr<ID3D12Resource> inputUAVBuffer);

		void BindAsTarget() noxnd;

		[[nodiscard]] UINT GetSRVDescriptorHeapOffset() const noexcept override;

		[[nodiscard]] CD3DX12_RESOURCE_DESC GetResourceDesc() const noexcept;
	private:
		bool selfManagedBuffer;
		wrl::ComPtr<ID3D12Resource> uavBuffer;

		std::shared_ptr<DX::DescriptorAllocator> descriptorAllocator;

		std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
		
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		CD3DX12_RESOURCE_DESC resourceDesc;
	};
}
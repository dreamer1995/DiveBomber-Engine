#pragma once
#include "Bindable.h"
#include "BindableShaderInput.h"

#include "..\GraphicsHeader.h"

namespace DiveBomber::DX
{
	class DescriptorAllocator;
	class DescriptorAllocation;
}

namespace DiveBomber::BindableObject
{
	class UnorderedAccessBuffer final : public Bindable, public BindableShaderInput
	{
	public:
		UnorderedAccessBuffer(UINT inputWidth, UINT inputHeight,
			std::shared_ptr<DX::DescriptorAllocator> inputDescriptorAllocator,
			DXGI_FORMAT inputFormat = DXGI_FORMAT_B8G8R8A8_UNORM, UINT inputMipLevels = 0);

		~UnorderedAccessBuffer();

		void Bind() noxnd override;

		[[nodiscard]] wrl::ComPtr<ID3D12Resource> GetUnorderedAccessBuffer() const noexcept;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetUAVCPUDescriptorHandle() const noexcept;
		void Resize(const UINT inputWidth, const UINT inputHeight);

		[[nodiscard]] UINT GetSRVDescriptorHeapOffset() const noexcept override;
	private:
		UINT width;
		UINT height;
		DXGI_FORMAT format;
		UINT mipLevels;
		wrl::ComPtr<ID3D12Resource> uavBuffer;

		std::shared_ptr<DX::DescriptorAllocator> descriptorAllocator;

		std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
		
		D3D12_UNORDERED_ACCESS_VIEW_DESC uav;
	};
}
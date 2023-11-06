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

		virtual void Bind() noxnd override;

		virtual void BindAsUAV() noxnd;
		virtual void BindAsSRV() noxnd;

		[[nodiscard]] wrl::ComPtr<ID3D12Resource> GetUnorderedAccessBuffer() const noexcept;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetUAVCPUDescriptorHandle() const noexcept;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle() const noexcept;
		virtual void Resize(const UINT inputWidth, const UINT inputHeight);

		UINT GetSRVDescriptorHeapOffset() const noexcept override;
	private:
		UINT width;
		UINT height;
		DXGI_FORMAT format;
		UINT mipLevels;
		wrl::ComPtr<ID3D12Resource> unorderedAccessBuffer;

		std::shared_ptr<DX::DescriptorAllocator> descriptorAllocator;

		std::shared_ptr<DX::DescriptorAllocation> uavDescriptorAllocation;
		D3D12_CPU_DESCRIPTOR_HANDLE uavCPUHandle;
		
		D3D12_UNORDERED_ACCESS_VIEW_DESC uav;

		std::shared_ptr<DX::DescriptorAllocation> srvDescriptorAllocation;
		D3D12_CPU_DESCRIPTOR_HANDLE srvCPUHandle;

		D3D12_SHADER_RESOURCE_VIEW_DESC srv;
	};
}
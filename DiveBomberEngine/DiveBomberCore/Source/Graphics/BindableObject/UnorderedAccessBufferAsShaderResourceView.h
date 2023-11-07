#pragma once
#include "Bindable.h"
#include "BindableShaderInput.h"
#include "UnorderedAccessBuffer.h"

namespace DiveBomber::DX
{
	class DescriptorAllocator;
	class DescriptorAllocation;
}

#include "..\GraphicsHeader.h"
namespace DiveBomber::BindableObject
{
	class UnorderedAccessBufferAsShaderResourceView final : public Bindable, public BindableShaderInput
	{
	public:
		UnorderedAccessBufferAsShaderResourceView(UINT inputWidth, UINT inputHeight,
			std::shared_ptr<DX::DescriptorAllocator> inputDescriptorAllocator,
			DXGI_FORMAT inputFormat = DXGI_FORMAT_B8G8R8A8_UNORM, UINT inputMipLevels = 0);

		~UnorderedAccessBufferAsShaderResourceView();

		void Bind() noxnd override;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle() const noexcept;
		void Resize(const UINT inputWidth, const UINT inputHeight);

		[[nodiscard]] UINT GetSRVDescriptorHeapOffset() const noexcept override;

		[[nodiscard]] std::shared_ptr<UnorderedAccessBuffer> GetUAVPointer() noexcept;
	private:
		std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;

		D3D12_SHADER_RESOURCE_VIEW_DESC srv;

		std::shared_ptr<UnorderedAccessBuffer> uavPointer;
	};
}
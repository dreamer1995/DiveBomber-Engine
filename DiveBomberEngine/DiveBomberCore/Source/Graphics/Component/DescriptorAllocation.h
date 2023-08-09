#pragma once

#include "..\GraphicsHeader.h"

#include <memory>

namespace DiveBomber::Component
{
	class DescriptorAllocatorPage;

	class DescriptorAllocation final
	{
	public:
		DescriptorAllocation();
		DescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE inputCPUAddress,
			D3D12_GPU_DESCRIPTOR_HANDLE inputGPUAddress,
			uint32_t inputNumHandles, uint32_t inputDescriptorSize,
			std::shared_ptr<DescriptorAllocatorPage> inputPage);
		~DescriptorAllocation();

		// Copies are not allowed.
		DescriptorAllocation(const DescriptorAllocation&) = delete;
		DescriptorAllocation& operator=(const DescriptorAllocation&) = delete;

		// Move is allowed.
		DescriptorAllocation(DescriptorAllocation&& allocation) noexcept;
		DescriptorAllocation& operator=(DescriptorAllocation&& other) noexcept;

		[[nodiscard]] bool IsInvalid() const noexcept;

		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t offset = 0) const noexcept;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t offset = 0) const noexcept;

		[[nodiscard]] uint32_t GetNumHandles() const noexcept;

		[[nodiscard]] std::shared_ptr<DescriptorAllocatorPage> GetDescriptorAllocatorPage() const noexcept;

	private:
		void FreeDescriptorAllocation();

	private:
		D3D12_CPU_DESCRIPTOR_HANDLE CPUAddress;
		D3D12_GPU_DESCRIPTOR_HANDLE GPUAddress;
		uint32_t numHandles;
		uint32_t descriptorSize;

		std::shared_ptr<DescriptorAllocatorPage> page;
	};
}
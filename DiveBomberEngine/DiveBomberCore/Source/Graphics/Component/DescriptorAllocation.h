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
		DescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE inputDescriptor, uint32_t inputNumHandles,
			uint32_t inputDescriptorSize, std::shared_ptr<DescriptorAllocatorPage> inputPage);
		~DescriptorAllocation();

		// Copies are not allowed.
		DescriptorAllocation(const DescriptorAllocation&) = delete;
		DescriptorAllocation& operator=(const DescriptorAllocation&) = delete;

		// Move is allowed.
		DescriptorAllocation(DescriptorAllocation&& allocation) noexcept;
		DescriptorAllocation& operator=(DescriptorAllocation&& other) noexcept;

		[[nodiscard]] bool IsInvalid() const noexcept;

		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle(uint32_t offset = 0) const noexcept;

		[[nodiscard]] uint32_t GetNumHandles() const noexcept;

		[[nodiscard]] std::shared_ptr<DescriptorAllocatorPage> GetDescriptorAllocatorPage() const noexcept;

	private:
		struct Page
		{
		public:
			Page(wrl::ComPtr<ID3D12Device2> device, size_t inputPageSize);
			~Page();
		private:
		};

	private:
		void FreeDescriptorAllocation();
	private:
		D3D12_CPU_DESCRIPTOR_HANDLE descriptor;
		uint32_t numHandles;
		uint32_t descriptorSize;

		std::shared_ptr<DescriptorAllocatorPage> page;
	};
}
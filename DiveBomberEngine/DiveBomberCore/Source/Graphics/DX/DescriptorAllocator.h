#pragma once

#include "..\GraphicsHeader.h"

#include <mutex>
#include <memory>
#include <set>
#include <vector>

namespace DiveBomber::DX
{
	class DescriptorAllocation;
	class DescriptorAllocatorPage;

	class DescriptorAllocator final
	{
	public:
		DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE inputType, uint32_t inputNumDescriptorsPerHeap = 256);
		~DescriptorAllocator();

		[[nodiscard]] std::shared_ptr<DescriptorAllocation> Allocate(const uint32_t numDescriptors = 1);
		void ReleaseStaleDescriptors(const uint64_t frameNumber);
		[[nodiscard]] std::vector<wrl::ComPtr<ID3D12DescriptorHeap>> GetAllDescriptorHeaps() noexcept;

	private:
		[[nodiscard]] std::shared_ptr<DescriptorAllocatorPage> CreateAllocatorPage();

	private:
		D3D12_DESCRIPTOR_HEAP_TYPE type;
		uint32_t numDescriptorsPerHeap;

		std::vector<std::shared_ptr<DescriptorAllocatorPage>> heapPool;
		std::set<size_t> availableHeaps;

		std::mutex allocationMutex;
	};
}
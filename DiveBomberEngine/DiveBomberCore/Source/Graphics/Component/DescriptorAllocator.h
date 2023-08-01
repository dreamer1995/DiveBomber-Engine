#pragma once

#include "..\GraphicsHeader.h"

#include <mutex>
#include <memory>
#include <set>
#include <vector>

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::Component
{
	class DescriptorAllocation;
	class DescriptorAllocatorPage;

	class DescriptorAllocator final
	{
	public:
		DescriptorAllocator(DEGraphics::Graphics& gfx, D3D12_DESCRIPTOR_HEAP_TYPE inputType, uint32_t inputNumDescriptorsPerHeap = 256);
		DescriptorAllocator(wrl::ComPtr<ID3D12Device2> inputDevice, D3D12_DESCRIPTOR_HEAP_TYPE inputType, uint32_t inputNumDescriptorsPerHeap = 256);
		~DescriptorAllocator();

		[[nodiscard]] DescriptorAllocation Allocate(const uint32_t numDescriptors = 1);
		void ReleaseStaleDescriptors(const uint64_t frameNumber);

	private:
		[[nodiscard]] std::shared_ptr<DescriptorAllocatorPage> CreateAllocatorPage();
	private:
		wrl::ComPtr<ID3D12Device2> device;
		D3D12_DESCRIPTOR_HEAP_TYPE type;
		uint32_t numDescriptorsPerHeap;

		std::vector<std::shared_ptr<DescriptorAllocatorPage>> heapPool;
		std::set<size_t> availableHeaps;

		std::mutex allocationMutex;
	};
}
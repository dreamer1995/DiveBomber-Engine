#pragma once

#include "..\GraphicsHeader.h"

#include <map>
#include <memory>
#include <mutex>
#include <queue>

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::Component
{
	class DescriptorAllocation;

	class DescriptorAllocatorPage final : public std::enable_shared_from_this<DescriptorAllocatorPage>
	{
	public:
		DescriptorAllocatorPage(DEGraphics::Graphics& gfx, D3D12_DESCRIPTOR_HEAP_TYPE inputType, uint32_t inputNumDescriptors);
		DescriptorAllocatorPage(wrl::ComPtr<ID3D12Device2> device, D3D12_DESCRIPTOR_HEAP_TYPE inputType, uint32_t inputNumDescriptors);

		[[nodiscard]] D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType() const noexcept;
		[[nodiscard]] bool AvailableSpace(const uint32_t numDescriptors) const;
		[[nodiscard]] uint32_t NumFreeHandles() const noexcept;
		[[nodiscard]] DescriptorAllocation Allocate(const uint32_t numDescriptors);
		void FreeDescriptorAllocation(DescriptorAllocation&& descriptorAllocation, const uint64_t frameNumber);
		void ReleaseStaleDescriptors(const uint64_t frameNumber);

	private:
		[[nodiscard]] uint32_t ComputeOffset(const D3D12_CPU_DESCRIPTOR_HANDLE handle);
		void AddNewBlock(const uint32_t offset, const uint32_t numDescriptors);
		void FreeBlock(uint32_t offset, uint32_t numDescriptors);

	private:
		struct AvailableBlockInfo
		{
		public:
			AvailableBlockInfo(uint32_t inputSize);

			uint32_t size;
			std::multimap<uint32_t, std::map<uint32_t, AvailableBlockInfo>::iterator>::iterator AvailableListBySizeIterator;
		};

		struct StaleDescriptorInfo
		{
		public:
			StaleDescriptorInfo(uint32_t inputOffset, uint32_t inputSize, uint64_t inputFrameNumber);

			uint32_t offset;
			uint32_t size;
			uint64_t frameCount;
		};

	private:
		std::map<uint32_t, AvailableBlockInfo> availableListByOffset;
		std::multimap<uint32_t, std::map<uint32_t, AvailableBlockInfo>::iterator> availableListBySize;
		std::queue<StaleDescriptorInfo> staleDescriptors;

		wrl::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
		D3D12_DESCRIPTOR_HEAP_TYPE type;
		CD3DX12_CPU_DESCRIPTOR_HANDLE baseDescriptorHandle;
		uint32_t descriptorHandleIncrementSize;
		uint32_t numDescriptorsInHeap;
		uint32_t numAvailableHandles;

		std::mutex allocationMutex;
	};
}
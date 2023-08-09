#include "DescriptorAllocatorPage.h"

#include "..\Graphics.h"
#include "..\..\Exception\GraphicsException.h"
#include "DescriptorAllocation.h"

namespace DiveBomber::Component
{
	using namespace DEGraphics;
	using namespace DEException;

	DescriptorAllocatorPage::DescriptorAllocatorPage(Graphics& gfx, D3D12_DESCRIPTOR_HEAP_TYPE inputType, uint32_t inputNumDescriptors)
		:
		DescriptorAllocatorPage(gfx.GetDecive(), inputType, inputNumDescriptors)
	{
	}
	DescriptorAllocatorPage::DescriptorAllocatorPage(wrl::ComPtr<ID3D12Device2> device, D3D12_DESCRIPTOR_HEAP_TYPE inputType, uint32_t inputNumDescriptors)
		:
		type(inputType),
		numDescriptorsInHeap(inputNumDescriptors),
		isShaderInvisible((type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV ||
			type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV))
	{
		const D3D12_DESCRIPTOR_HEAP_FLAGS descriptorHeapFlags = isShaderInvisible ?
			D3D12_DESCRIPTOR_HEAP_FLAG_NONE : D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type = type;
		heapDesc.NumDescriptors = numDescriptorsInHeap;
		heapDesc.Flags = descriptorHeapFlags;

		HRESULT hr;
		GFX_THROW_INFO(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&descriptorHeap)));

		baseCPUDescriptorHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		baseGPUDescriptorHandle = isShaderInvisible ?
			CD3DX12_GPU_DESCRIPTOR_HANDLE{} : descriptorHeap->GetGPUDescriptorHandleForHeapStart();
		descriptorHandleIncrementSize = device->GetDescriptorHandleIncrementSize(type);
		numAvailableHandles = numDescriptorsInHeap;

		// Initialize the free lists
		AddNewBlock(0, numAvailableHandles);
	}

	D3D12_DESCRIPTOR_HEAP_TYPE DescriptorAllocatorPage::GetHeapType() const noexcept
	{
		return type;
	}

	bool DescriptorAllocatorPage::AvailableSpace(const uint32_t numDescriptors) const
	{
		return availableListBySize.lower_bound(numDescriptors) != availableListBySize.end();
	}

	uint32_t DescriptorAllocatorPage::NumFreeHandles() const noexcept
	{
		return numAvailableHandles;
	}

	std::shared_ptr<DescriptorAllocation> DescriptorAllocatorPage::Allocate(const uint32_t numDescriptors)
	{
		std::lock_guard<std::mutex> lock(allocationMutex);

		if (numDescriptors > numAvailableHandles)
		{
			return std::make_shared<DescriptorAllocation>();
		}

		auto smallestBlockIterator = availableListBySize.lower_bound(numDescriptors);
		if (smallestBlockIterator == availableListBySize.end())
		{
			return std::make_shared<DescriptorAllocation>();
		}

		auto blockSize = smallestBlockIterator->first;
		auto offsetIterator = smallestBlockIterator->second;
		auto offset = offsetIterator->first;

		availableListBySize.erase(smallestBlockIterator);
		availableListByOffset.erase(offsetIterator);

		auto newOffset = offset + numDescriptors;
		auto newSize = blockSize - numDescriptors;

		if (newSize > 0)
		{
			AddNewBlock(newOffset, newSize);
		}

		numAvailableHandles -= numDescriptors;

		return std::make_shared<DescriptorAllocation>(
			CD3DX12_CPU_DESCRIPTOR_HANDLE(baseCPUDescriptorHandle, offset, descriptorHandleIncrementSize),
			isShaderInvisible ?
			CD3DX12_GPU_DESCRIPTOR_HANDLE{} : CD3DX12_GPU_DESCRIPTOR_HANDLE(baseGPUDescriptorHandle, offset, descriptorHandleIncrementSize),
			numDescriptors, descriptorHandleIncrementSize, shared_from_this()
		);
	}

	void DescriptorAllocatorPage::FreeDescriptorAllocation(DescriptorAllocation&& descriptorAllocation, const uint64_t frameNumber)
	{
		auto offset = ComputeOffset(descriptorAllocation.GetCPUDescriptorHandle());

		std::lock_guard<std::mutex> lock(allocationMutex);

		staleDescriptors.emplace(offset, descriptorAllocation.GetNumHandles(), frameNumber);
	}

	void DescriptorAllocatorPage::ReleaseStaleDescriptors(const uint64_t frameNumber)
	{
		std::lock_guard<std::mutex> lock(allocationMutex);

		while (!staleDescriptors.empty() && staleDescriptors.front().frameCount <= frameNumber)
		{
			auto& staleDescriptor = staleDescriptors.front();

			auto offset = staleDescriptor.offset;
			auto numDescriptors = staleDescriptor.size;

			FreeBlock(offset, numDescriptors);

			staleDescriptors.pop();
		}
	}

	uint32_t DescriptorAllocatorPage::ComputeOffset(const D3D12_CPU_DESCRIPTOR_HANDLE handle)
	{
		return static_cast<uint32_t>(handle.ptr - baseCPUDescriptorHandle.ptr) / descriptorHandleIncrementSize;
	}

	void DescriptorAllocatorPage::AddNewBlock(const uint32_t offset, const uint32_t numDescriptors)
	{
		auto offsetIterator = availableListByOffset.emplace(offset, numDescriptors);
		auto sizeIterator = availableListBySize.emplace(numDescriptors, offsetIterator.first);
		offsetIterator.first->second.AvailableListBySizeIterator = sizeIterator;
	}

	void DescriptorAllocatorPage::FreeBlock(uint32_t offset, uint32_t numDescriptors)
	{
		// https://www.3dgep.com/learning-directx-12-3/

		auto nextBlockIterator = availableListByOffset.upper_bound(offset);
		auto prevBlockIterator = nextBlockIterator;

		if (prevBlockIterator != availableListByOffset.begin())
		{
			--prevBlockIterator;
		}
		else
		{
			prevBlockIterator = availableListByOffset.end();
		}

		numAvailableHandles += numDescriptors;

		if (prevBlockIterator != availableListByOffset.end() &&
			offset == prevBlockIterator->first + prevBlockIterator->second.size)
		{
			offset = prevBlockIterator->first;
			numDescriptors += prevBlockIterator->second.size;

			availableListBySize.erase(prevBlockIterator->second.AvailableListBySizeIterator);
			availableListByOffset.erase(prevBlockIterator);
		}

		if (nextBlockIterator != availableListByOffset.end() &&
			offset + numDescriptors == nextBlockIterator->first)
		{
			numDescriptors += nextBlockIterator->second.size;

			availableListBySize.erase(nextBlockIterator->second.AvailableListBySizeIterator);
			availableListByOffset.erase(nextBlockIterator);
		}

		AddNewBlock(offset, numDescriptors);
	}

	DescriptorAllocatorPage::AvailableBlockInfo::AvailableBlockInfo(uint32_t inputSize)
		:
		size(inputSize)
	{
	}

	DescriptorAllocatorPage::StaleDescriptorInfo::StaleDescriptorInfo(uint32_t inputOffset, uint32_t inputSize, uint64_t inputFrameNumber)
		:
		offset(inputOffset),
		size(inputSize),
		frameCount(inputFrameNumber)
	{
	}
}

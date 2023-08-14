#include "DescriptorAllocator.h"

#include "..\Graphics.h"
#include "DescriptorAllocatorPage.h"
#include "DescriptorAllocation.h"

#include <format>

namespace DiveBomber::DX
{
	using namespace DEGraphics;

	DescriptorAllocator::DescriptorAllocator(Graphics& gfx, D3D12_DESCRIPTOR_HEAP_TYPE inputType, uint32_t inputNumDescriptorsPerHeap)
		:
		DescriptorAllocator(gfx.GetDecive(), inputType, inputNumDescriptorsPerHeap)
	{
	}
	DescriptorAllocator::DescriptorAllocator(wrl::ComPtr<ID3D12Device2> inputDevice, D3D12_DESCRIPTOR_HEAP_TYPE inputType, uint32_t inputNumDescriptorsPerHeap)
		:
		device(inputDevice),
		type(inputType),
		numDescriptorsPerHeap(inputNumDescriptorsPerHeap)
	{
	}

	DescriptorAllocator::~DescriptorAllocator()
	{
	}

	std::shared_ptr<DescriptorAllocation> DescriptorAllocator::Allocate(const uint32_t numDescriptors)
	{
		std::lock_guard<std::mutex> lock(allocationMutex);

		std::shared_ptr<DescriptorAllocation> descriptorAllocation;
		// If use bindless, apply second heap is not needed
	#if BindlessRendering
		std::shared_ptr<DescriptorAllocatorPage> allocatorPage;

		if (heapPool.empty())
		{
			allocatorPage = CreateAllocatorPage();
		}
		else
		{
			allocatorPage = heapPool.front();
		}

		descriptorAllocation = allocatorPage->Allocate(numDescriptors);

		if (allocatorPage->NumFreeHandles() == 0)
		{
			std::string returnHeapType;
			switch (type)
			{
			case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: 
				returnHeapType = "CBVSRVUAV";
				break;
			case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
				returnHeapType = "RTV";
				break;
			case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
				returnHeapType = "DSV";
				break;
			case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
				returnHeapType = "Sampler";
				break;
			}
			throw std::exception{std::format("There is no more space in {} heap", returnHeapType).c_str()};
		}
	#else
		for (auto iter = availableHeaps.begin(); iter != availableHeaps.end();)
		{
			auto allocatorPage = heapPool[*iter];

			descriptorAllocation = allocatorPage->Allocate(numDescriptors);

			if (allocatorPage->NumFreeHandles() == 0)
			{
				iter = availableHeaps.erase(iter);
			}
			else
			{
				iter++;
			}

			if (!descriptorAllocation->IsInvalid())
			{
				break;
			}
		}

		if (!descriptorAllocation || descriptorAllocation->IsInvalid())
		{
			numDescriptorsPerHeap = std::max(numDescriptorsPerHeap, numDescriptors);
			auto newPage = CreateAllocatorPage();

			descriptorAllocation = newPage->Allocate(numDescriptors);
		}
	#endif

		return descriptorAllocation;
	}

	void DescriptorAllocator::ReleaseStaleDescriptors(const uint64_t frameNumber)
	{
		std::lock_guard<std::mutex> lock(allocationMutex);

		for (size_t i = 0; i < heapPool.size(); i++)
		{
			auto page = heapPool[i];
			
			page->ReleaseStaleDescriptors(frameNumber);

			if (page->NumFreeHandles() > 0)
			{
				availableHeaps.insert(i);
			}
		}
	}

	std::vector<wrl::ComPtr<ID3D12DescriptorHeap>> DescriptorAllocator::GetAllDescriptorHeaps() noexcept
	{
		std::vector<wrl::ComPtr<ID3D12DescriptorHeap>> descriptorHeaps{};

		for (const std::shared_ptr<DescriptorAllocatorPage>& heap : heapPool)
		{
			descriptorHeaps.emplace_back(heap->GetDescriptorHeap());
		}

		return descriptorHeaps;
	}

	std::shared_ptr<DescriptorAllocatorPage> DescriptorAllocator::CreateAllocatorPage()
	{
		auto newPage = std::make_shared<DescriptorAllocatorPage>(device, type, numDescriptorsPerHeap);

		heapPool.emplace_back(newPage);
		availableHeaps.insert(heapPool.size() - 1);

		return newPage;
	}

	
}

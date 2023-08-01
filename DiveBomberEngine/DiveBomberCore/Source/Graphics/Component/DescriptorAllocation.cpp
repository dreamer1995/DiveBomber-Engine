#include "DescriptorAllocation.h"

#include "DescriptorAllocatorPage.h"
#include "..\..\Utility\GlobalParameters.h"

namespace DiveBomber::Component
{
	DescriptorAllocation::DescriptorAllocation()
		:
		descriptor(D3D12_CPU_DESCRIPTOR_HANDLE(0)),
		numHandles(0),
		descriptorSize(0),
		page(nullptr)
	{
	}

	DescriptorAllocation::DescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE inputDescriptor, uint32_t inputNumHandles,
		uint32_t inputDescriptorSize, std::shared_ptr<DescriptorAllocatorPage> inputPage)
		:
		descriptor(inputDescriptor),
		numHandles(inputNumHandles),
		descriptorSize(inputDescriptorSize),
		page(inputPage)
	{
	}

	DescriptorAllocation::~DescriptorAllocation()
	{
		FreeDescriptorAllocation();
	}

	DescriptorAllocation::DescriptorAllocation(DescriptorAllocation&& allocation) noexcept
		:
		descriptor(allocation.descriptor),
		numHandles(allocation.numHandles),
		descriptorSize(allocation.descriptorSize),
		page(std::move(allocation.page))
	{
		allocation.descriptor = D3D12_CPU_DESCRIPTOR_HANDLE(0);
		allocation.numHandles = 0;
		allocation.descriptorSize = 0;
	}

	DescriptorAllocation& DescriptorAllocation::operator=(DescriptorAllocation&& other) noexcept
	{
		FreeDescriptorAllocation();

		descriptor = other.descriptor;
		numHandles = other.numHandles;
		descriptorSize = other.descriptorSize;
		page = std::move(other.page);

		other.descriptor = D3D12_CPU_DESCRIPTOR_HANDLE(0);
		other.numHandles = 0;
		other.descriptorSize = 0;

		return *this;
	}

	void DescriptorAllocation::FreeDescriptorAllocation()
	{
		if (!IsInvalid() && page)
		{
			//page->Free(std::move(*this), Utility::g_FrameCounter);

			descriptor.ptr = 0;
			numHandles = 0;
			descriptorSize = 0;
			page.reset();
		}
	}

	bool DescriptorAllocation::IsInvalid() const noexcept
	{
		return descriptor.ptr == 0;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocation::GetDescriptorHandle(uint32_t offset) const noexcept
	{
		assert(offset < numHandles);
		return { descriptor.ptr + (descriptorSize * offset) };
	}

	uint32_t DescriptorAllocation::GetNumHandles() const noexcept
	{
		return numHandles;
	}

	std::shared_ptr<DescriptorAllocatorPage> DescriptorAllocation::GetDescriptorAllocatorPage() const noexcept
	{
		return page;
	}
}

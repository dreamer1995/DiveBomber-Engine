#include "DescriptorAllocation.h"

#include "DescriptorAllocatorPage.h"
#include "..\..\Utility\GlobalParameters.h"

namespace DiveBomber::Component
{
	DescriptorAllocation::DescriptorAllocation()
		:
		CPUAddress(D3D12_CPU_DESCRIPTOR_HANDLE(0)),
		GPUAddress(D3D12_GPU_DESCRIPTOR_HANDLE(0)),
		numHandles(0),
		descriptorSize(0),
		page(nullptr)
	{
	}

	DescriptorAllocation::DescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE inputCPUAddress,
		D3D12_GPU_DESCRIPTOR_HANDLE inputGPUAddress,
		uint32_t inputNumHandles, uint32_t inputDescriptorSize,
		std::shared_ptr<DescriptorAllocatorPage> inputPage)
		:
		CPUAddress(inputCPUAddress),
		GPUAddress(inputGPUAddress),
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
		CPUAddress(allocation.CPUAddress),
		GPUAddress(allocation.GPUAddress),
		numHandles(allocation.numHandles),
		descriptorSize(allocation.descriptorSize),
		page(std::move(allocation.page))
	{
		allocation.CPUAddress = D3D12_CPU_DESCRIPTOR_HANDLE(0);
		allocation.GPUAddress = D3D12_GPU_DESCRIPTOR_HANDLE(0);
		allocation.numHandles = 0;
		allocation.descriptorSize = 0;
	}

	DescriptorAllocation& DescriptorAllocation::operator=(DescriptorAllocation&& other) noexcept
	{
		FreeDescriptorAllocation();

		CPUAddress = other.CPUAddress;
		GPUAddress = other.GPUAddress;
		numHandles = other.numHandles;
		descriptorSize = other.descriptorSize;
		page = std::move(other.page);

		other.CPUAddress = D3D12_CPU_DESCRIPTOR_HANDLE(0);
		other.GPUAddress = D3D12_GPU_DESCRIPTOR_HANDLE(0);
		other.numHandles = 0;
		other.descriptorSize = 0;

		return *this;
	}

	void DescriptorAllocation::FreeDescriptorAllocation()
	{
		if (!IsInvalid() && page)
		{
			//page->Free(std::move(*this), Utility::g_FrameCounter);

			CPUAddress.ptr = 0;
			GPUAddress.ptr = 0;
			numHandles = 0;
			descriptorSize = 0;
			page.reset();
		}
	}

	bool DescriptorAllocation::IsInvalid() const noexcept
	{
		return CPUAddress.ptr == 0;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocation::GetCPUDescriptorHandle(uint32_t offset) const noexcept
	{
		assert(offset < numHandles);
		return { CPUAddress.ptr + (descriptorSize * offset) };
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocation::GetGPUDescriptorHandle(uint32_t offset) const noexcept
	{
		assert(offset < numHandles);
		return { GPUAddress.ptr + (descriptorSize * offset) };
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

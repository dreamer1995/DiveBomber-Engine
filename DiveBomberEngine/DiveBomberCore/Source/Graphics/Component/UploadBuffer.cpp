#include "UploadBuffer.h"

#include "..\Graphics.h"
#include "..\..\Exception\GraphicsException.h"

namespace DiveBomber::Component
{
	using namespace DEGraphics;
	using namespace DEException;

	UploadBuffer::UploadBuffer(Graphics& gfx, size_t inputPageSize)
		:
		UploadBuffer(gfx.GetDecive(), inputPageSize)
	{
	}

	UploadBuffer::UploadBuffer(wrl::ComPtr<ID3D12Device2> inputDevice, size_t inputPageSize)
	{
		pageSize = inputPageSize;
		device = inputDevice;
	}

	UploadBuffer::~UploadBuffer()
	{
	}

	size_t UploadBuffer::GetPageSize() const
	{
		return pageSize;
	}

	UploadBuffer::AllocationInfo UploadBuffer::Allocate(const size_t size, const size_t alignment)
	{
		if (size > pageSize)
		{
			throw std::bad_alloc();
		}

		if (!currentPage || !currentPage->AvailableSpace(size, alignment))
		{
			currentPage = RequestPage();
		}

		return currentPage->Allocate(size, alignment);
	}

	void UploadBuffer::Reset()
	{
		currentPage = nullptr;
		availablePages = pagePool;

		for (std::shared_ptr<Page> page : availablePages)
		{
			page->Reset();
		}
	}

	std::shared_ptr<UploadBuffer::Page> UploadBuffer::RequestPage()
	{
		std::shared_ptr<Page> page;

		if (!availablePages.empty())
		{
			page = availablePages.front();
			availablePages.pop_front();
		}
		else
		{
			page = std::make_shared<Page>(device, pageSize);
			pagePool.emplace_back(page);
		}

		return page;
	}

	UploadBuffer::Page::Page(wrl::ComPtr<ID3D12Device2> device, size_t inputPageSize)
	{
		pageSize = inputPageSize;

		const CD3DX12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_UPLOAD };
		const CD3DX12_RESOURCE_DESC resDes = CD3DX12_RESOURCE_DESC::Buffer(pageSize);

		HRESULT hr;
		GFX_THROW_INFO(device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDes,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&resourceBuffer))
		);

		GPUAddress = resourceBuffer->GetGPUVirtualAddress();
		resourceBuffer->Map(0, nullptr, &CPUAddress);
	}

	UploadBuffer::Page::~Page()
	{
		resourceBuffer->Unmap(0, nullptr);
		CPUAddress = nullptr;
		GPUAddress = D3D12_GPU_VIRTUAL_ADDRESS(0);
	}

	bool UploadBuffer::Page::AvailableSpace(size_t size, size_t alignment) const
	{
		size_t alignedSize = Utility::AlignUp(size, alignment);
		size_t alignedOffset = Utility::AlignUp(offset, alignment);

		return alignedSize + alignedOffset <= pageSize;
	}

	UploadBuffer::AllocationInfo UploadBuffer::Page::Allocate(const size_t size, const size_t alignment)
	{
		if (!AvailableSpace(size, alignment))
		{
			// Can't allocate space from page.
			throw std::bad_alloc();
		}

		size_t alignedSize = Utility::AlignUp(size, alignment);
		offset = Utility::AlignUp(offset, alignment);

		AllocationInfo allocationInfo;
		allocationInfo.CPUAddress = static_cast<uint8_t*>(CPUAddress) + offset;
		allocationInfo.GPUAddress = GPUAddress + offset;

		offset += alignedSize;

		return allocationInfo;
	}
	void UploadBuffer::Page::Reset()
	{
		offset = 0;
	}
}

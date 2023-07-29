#pragma once

#include "..\GraphicsHeader.h"

#include <memory>
#include <deque>

#define _KB(x) (x * 1024)
#define _MB(x) (x * 1024 * 1024)

#define _64KB _KB(64)
#define _1MB _MB(1)
#define _2MB _MB(2)
#define _4MB _MB(4)
#define _8MB _MB(8)
#define _16MB _MB(16)
#define _32MB _MB(32)
#define _64MB _MB(64)
#define _128MB _MB(128)
#define _256MB _MB(256)

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::Component
{
	class UploadBuffer final
	{
	public:
		struct AllocationInfo
		{
			void* CPUAddress;
			D3D12_GPU_VIRTUAL_ADDRESS GPUAddress;
		};
	public:
		UploadBuffer(DEGraphics::Graphics& gfx, size_t inputPageSize = _2MB);
		UploadBuffer(wrl::ComPtr<ID3D12Device2> inputDevice, size_t inputPageSize = _2MB);
		~UploadBuffer();

		[[nodiscard]] size_t GetPageSize() const;
		[[nodiscard]] AllocationInfo Allocate(const size_t size, const size_t alignment);
		void Reset();

	private:
		struct Page
		{
		public:
			Page(wrl::ComPtr<ID3D12Device2> device, size_t inputPageSize);
			~Page();

			[[nodiscard]] bool AvailableSpace(size_t size, size_t alignment) const;
			AllocationInfo Allocate(const size_t size, const size_t alignment);
			void Reset();
		private:
			wrl::ComPtr<ID3D12Resource> resourceBuffer;
			void* CPUAddress = nullptr;
			D3D12_GPU_VIRTUAL_ADDRESS GPUAddress = D3D12_GPU_VIRTUAL_ADDRESS(0);
			size_t pageSize;
			size_t offset = 0;
		};

	private:
		[[nodiscard]] std::shared_ptr<Page> RequestPage();

	private:
		wrl::ComPtr<ID3D12Device2> device;
		size_t pageSize;

		std::deque<std::shared_ptr<Page>> pagePool;
		std::deque<std::shared_ptr<Page>> availablePages;
		std::shared_ptr<Page> currentPage;
	};
}
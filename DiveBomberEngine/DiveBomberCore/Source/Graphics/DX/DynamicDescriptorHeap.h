#pragma once

#include "..\GraphicsHeader.h"

#include <memory>
#include <queue>
#include <functional>

namespace DiveBomber::DX
{
	class DynamicDescriptorHeap final
	{
	public:
		struct AllocationInfo
		{
			void* CPUAddress;
			D3D12_GPU_VIRTUAL_ADDRESS GPUAddress;
		};
	public:
		DynamicDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE inputHeapType, uint32_t inputNumDescriptorsPerHeap = 1024);
		~DynamicDescriptorHeap();

		void BuildDescriptorTableCache();
		void StageDescriptors(uint32_t rootParameterIndex, uint32_t offset, uint32_t numDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptors);
		void CommitStagedDescriptors(std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_DESCRIPTOR_HANDLE)> setFunc);
		[[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE CopyDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor);
		void Reset() noexcept;

	private:
		struct DescriptorTableCache
		{

		};

	private:
		[[nodiscard]] wrl::ComPtr<ID3D12DescriptorHeap> RequestDescriptorHeap();
		[[nodiscard]] wrl::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap();
		[[nodiscard]] uint32_t ComputeStaleDescriptorCount() const;

	private:
		static const uint32_t maxDescriptorTables = 32;

		D3D12_DESCRIPTOR_HEAP_TYPE type;

		uint32_t numDescriptorsPerHeap;

		uint32_t descriptorHandleIncrementSize;

		std::unique_ptr<D3D12_CPU_DESCRIPTOR_HANDLE[]> descriptorHandleCache;

		DescriptorTableCache descriptorTableCache[maxDescriptorTables];

		uint32_t descriptorTableBitMask;
		uint32_t staleDescriptorTableBitMask;

		std::queue<wrl::ComPtr<ID3D12DescriptorHeap>> descriptorHeapPool;
		std::queue<wrl::ComPtr<ID3D12DescriptorHeap>> availableDescriptorHeaps;

		wrl::ComPtr<ID3D12DescriptorHeap> currentDescriptorHeap;
		CD3DX12_GPU_DESCRIPTOR_HANDLE currentGPUDescriptorHandle;
		CD3DX12_CPU_DESCRIPTOR_HANDLE currentCPUDescriptorHandle;

		uint32_t numFreeHandles;
	};
}
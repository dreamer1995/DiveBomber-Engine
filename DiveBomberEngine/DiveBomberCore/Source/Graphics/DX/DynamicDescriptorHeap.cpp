#include "DynamicDescriptorHeap.h"

#include "..\GraphicsSource.h"

namespace DiveBomber::DX
{
	using namespace DEGraphics;
	using namespace DEException;
	
	DynamicDescriptorHeap::DynamicDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE inputHeapType, uint32_t inputNumDescriptorsPerHeap)
		:
		type(inputHeapType),
		numDescriptorsPerHeap(inputNumDescriptorsPerHeap),
		descriptorTableBitMask(0),
		staleDescriptorTableBitMask(0),
		currentCPUDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE(0)),
		currentGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE(0)),
		numFreeHandles(0)
	{
		descriptorHandleIncrementSize = Graphics::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(type);

		descriptorHandleCache = std::make_unique<D3D12_CPU_DESCRIPTOR_HANDLE[]>(numDescriptorsPerHeap);
	}

	DynamicDescriptorHeap::~DynamicDescriptorHeap()
	{

	}

	void DynamicDescriptorHeap::BuildDescriptorTableCache()
	{

	}
}

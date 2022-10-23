#pragma once
#include "GraphicsResource.h"

class DescriptorHeap
{
public:
	DescriptorHeap(ID3D12Device2* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);
	ID3D12DescriptorHeap* GetDescriptorHeap()noexcept;
private:
	wrl::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
};


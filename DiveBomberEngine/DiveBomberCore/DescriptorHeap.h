#pragma once
#include "GraphicsResource.h"

class DescriptorHeap final
{
public:
	DescriptorHeap(ID3D12Device2* device, D3D12_DESCRIPTOR_HEAP_TYPE type);
	ID3D12DescriptorHeap* GetDescriptorHeap()noexcept;
private:
	wrl::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
};


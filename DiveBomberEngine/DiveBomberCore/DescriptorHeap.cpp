#include "DescriptorHeap.h"
DescriptorHeap::DescriptorHeap(ID3D12Device2* device, D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	HRESULT hr;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = SwapChainBufferCount;
	desc.Type = type;

	GFX_THROW_INFO(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));
}
ID3D12DescriptorHeap* DescriptorHeap::GetDescriptorHeap()noexcept
{
	return descriptorHeap.Get();
}
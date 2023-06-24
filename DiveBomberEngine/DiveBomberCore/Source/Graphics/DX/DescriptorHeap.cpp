#include "DescriptorHeap.h"

namespace DiveBomber::DX
{
	using namespace DEException;

	DescriptorHeap::DescriptorHeap(wrl::ComPtr<ID3D12Device2> device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT RTNum)
	{
		HRESULT hr;

		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = RTNum;
		desc.Type = type;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 0;

		GFX_THROW_INFO(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));
	}
	wrl::ComPtr<ID3D12DescriptorHeap> DescriptorHeap::GetDescriptorHeap() const noexcept
	{
		return descriptorHeap;
	}
}
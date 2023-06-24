#pragma once
#include "..\GraphicsResource.h"

namespace DiveBomber::DX
{
	class DescriptorHeap final
	{
	public:
		DescriptorHeap(wrl::ComPtr<ID3D12Device2> device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT RTNum);
		[[nodiscard]] wrl::ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() const noexcept;
	private:
		wrl::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	};
}
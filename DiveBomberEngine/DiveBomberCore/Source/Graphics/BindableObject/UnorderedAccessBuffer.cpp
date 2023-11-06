#include "UnorderedAccessBuffer.h"

#include "..\Graphics.h"
#include "..\..\Exception\GraphicsException.h"
#include "..\DX\DescriptorAllocator.h"
#include "..\DX\DescriptorAllocation.h"
#include "..\DX\ResourceStateTracker.h"

namespace DiveBomber::BindableObject
{
	using namespace DEGraphics;
	using namespace DEException;
	using namespace DX;

	UnorderedAccessBuffer::UnorderedAccessBuffer(UINT inputWidth, UINT inputHeight,
		std::shared_ptr<DX::DescriptorAllocator> inputDescriptorAllocator,
		DXGI_FORMAT inputFormat, UINT inputMipLevels)
		:
		width(inputWidth),
		height(inputHeight),
		descriptorAllocator(inputDescriptorAllocator),
		uavDescriptorAllocation(descriptorAllocator->Allocate(1u)),
		uavCPUHandle(uavDescriptorAllocation->GetCPUDescriptorHandle()),
		uav(),
		srvDescriptorAllocation(descriptorAllocator->Allocate(1u)),
		srvCPUHandle(srvDescriptorAllocation->GetCPUDescriptorHandle()),
		srv(),
		mipLevels(inputMipLevels),
		format(inputFormat)
	{
		Resize(inputWidth, inputHeight);
	}

	UnorderedAccessBuffer::~UnorderedAccessBuffer()
	{
		ResourceStateTracker::RemoveGlobalResourceState(uavBuffer);
	}

	void UnorderedAccessBuffer::Bind() noxnd
	{
	}

	void UnorderedAccessBuffer::BindAsUAV() noxnd
	{
		ResourceStateTracker::AddGlobalResourceState(uavBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}

	void UnorderedAccessBuffer::BindAsSRV() noxnd
	{
		ResourceStateTracker::AddGlobalResourceState(uavBuffer, D3D12_RESOURCE_STATE_COMMON);
	}

	wrl::ComPtr<ID3D12Resource> UnorderedAccessBuffer::GetUnorderedAccessBuffer() const noexcept
	{
		return uavBuffer;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE UnorderedAccessBuffer::GetUAVCPUDescriptorHandle() const noexcept
	{
		return uavCPUHandle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE UnorderedAccessBuffer::GetSRVCPUDescriptorHandle() const noexcept
	{
		return srvCPUHandle;
	}

	UINT UnorderedAccessBuffer::GetSRVDescriptorHeapOffset() const noexcept
	{
		return srvDescriptorAllocation->GetBaseOffset();
	}

	void UnorderedAccessBuffer::Resize(const UINT inputWidth, const UINT inputHeight)
	{
		width = std::max(1u, inputWidth);
		height = std::max(1u, inputHeight);

		HRESULT hr;

		auto device = Graphics::GetInstance().GetDevice();

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		auto resDes = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height,
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		GFX_THROW_INFO(device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDes,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&uavBuffer)
		));

		device->CreateUnorderedAccessView(uavBuffer.Get(), nullptr, nullptr, uavCPUHandle);

		device->CreateShaderResourceView(uavBuffer.Get(), nullptr, srvCPUHandle);
	}
}

#include "UnorderedAccessBuffer.h"

#include "..\..\GraphicsSource.h"
#include "..\..\DX\DescriptorAllocator.h"
#include "..\..\DX\DescriptorAllocation.h"
#include "..\..\DX\ResourceStateTracker.h"
#include "..\..\DX\CommandList.h"

namespace DiveBomber::GraphicResource
{
	using namespace DEGraphics;
	using namespace DEException;
	using namespace DX;

	UnorderedAccessBuffer::UnorderedAccessBuffer(
		std::shared_ptr<DX::DescriptorAllocator> inputDescriptorAllocator,
		CD3DX12_RESOURCE_DESC inputDesc)
		:
		Resource(L"L"),
		descriptorAllocator(inputDescriptorAllocator),
		descriptorAllocation(descriptorAllocator->Allocate(1u)),
		cpuHandle(descriptorAllocation->GetCPUDescriptorHandle()),
		uavDesc(),
		resourceDesc(inputDesc),
		selfManagedBuffer(true)
	{
		Resize(resourceDesc);
	}

	UnorderedAccessBuffer::UnorderedAccessBuffer(
		std::shared_ptr<DescriptorAllocator> inputDescriptorAllocator,
		wrl::ComPtr<ID3D12Resource> inputUAVBuffer,
		D3D12_UNORDERED_ACCESS_VIEW_DESC inputUAVDesc)
		:
		Resource(L"L"),
		descriptorAllocator(inputDescriptorAllocator),
		descriptorAllocation(descriptorAllocator->Allocate(1u)),
		cpuHandle(descriptorAllocation->GetCPUDescriptorHandle()),
		uavDesc(inputUAVDesc),
		uavBuffer(inputUAVBuffer),
		resourceDesc(inputUAVBuffer->GetDesc()),
		selfManagedBuffer(false)
	{
		Resize(uavBuffer);
	}

	UnorderedAccessBuffer::~UnorderedAccessBuffer()
	{
		if (selfManagedBuffer)
		{
			ResourceStateTracker::RemoveGlobalResourceState(uavBuffer);
		}
	}

	void UnorderedAccessBuffer::BindAsTarget() noxnd
	{
		Graphics::GetInstance().GetCommandList()->AddTransitionBarrier(uavBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
	}

	wrl::ComPtr<ID3D12Resource> UnorderedAccessBuffer::GetUnorderedAccessBuffer() const noexcept
	{
		return uavBuffer;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE UnorderedAccessBuffer::GetUAVCPUDescriptorHandle() const noexcept
	{
		return cpuHandle;
	}

	UINT UnorderedAccessBuffer::GetSRVDescriptorHeapOffset() const noexcept
	{
		return descriptorAllocation->GetBaseOffset();
	}

	CD3DX12_RESOURCE_DESC DiveBomber::GraphicResource::UnorderedAccessBuffer::GetResourceDesc() const noexcept
	{
		return resourceDesc;
	}

	void UnorderedAccessBuffer::Resize(const CD3DX12_RESOURCE_DESC inputDesc)
	{
		if (!selfManagedBuffer)
		{
			throw std::exception{ "Try to overwrite external buffer!" };
		}

		resourceDesc = inputDesc;

		HRESULT hr;

		auto device = Graphics::GetInstance().GetDevice();

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		GFX_THROW_INFO(device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			nullptr,
			IID_PPV_ARGS(&uavBuffer)
		));
		ResourceStateTracker::AddGlobalResourceState(uavBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		device->CreateUnorderedAccessView(uavBuffer.Get(), nullptr, nullptr, cpuHandle);
	}

	void DiveBomber::GraphicResource::UnorderedAccessBuffer::Resize(UINT width, UINT height)
	{
		// Don't allow 0 size swap chain back buffers.
		width = std::max(1u, width);
		height = std::max(1u, height);

		resourceDesc.Width = width;
		resourceDesc.Height = height;
		Resize(resourceDesc);
	}

	void UnorderedAccessBuffer::Resize(const wrl::ComPtr<ID3D12Resource> inputUAVBuffer)
	{
		if (selfManagedBuffer)
		{
			throw std::exception{ "Try to overwrite internal buffer!" };
		}

		if (uavBuffer != inputUAVBuffer)
		{
			ResourceStateTracker::RemoveGlobalResourceState(uavBuffer);
		}
		uavBuffer = inputUAVBuffer;
		resourceDesc = CD3DX12_RESOURCE_DESC(uavBuffer->GetDesc());

		Graphics::GetInstance().GetDevice()->CreateUnorderedAccessView(uavBuffer.Get(), nullptr, &uavDesc, cpuHandle);
	}
}

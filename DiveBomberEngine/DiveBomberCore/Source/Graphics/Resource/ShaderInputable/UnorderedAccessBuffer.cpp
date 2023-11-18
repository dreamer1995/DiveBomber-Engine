#include "UnorderedAccessBuffer.h"

#include "..\..\GraphicsSource.h"
#include "..\..\DX\DescriptorAllocator.h"
#include "..\..\DX\DescriptorAllocation.h"
#include "..\..\DX\ResourceStateTracker.h"
#include "..\..\DX\CommandList.h"

namespace DiveBomber::DEResource
{
	using namespace DEGraphics;
	using namespace DEException;
	using namespace DX;

	UnorderedAccessBuffer::UnorderedAccessBuffer(UINT inputWidth, UINT inputHeight,
		std::shared_ptr<DX::DescriptorAllocator> inputDescriptorAllocator,
		DXGI_FORMAT inputFormat, UINT inputMipLevels)
		:
		Resource(L"L"),
		width(inputWidth),
		height(inputHeight),
		descriptorAllocator(inputDescriptorAllocator),
		descriptorAllocation(descriptorAllocator->Allocate(1u)),
		cpuHandle(descriptorAllocation->GetCPUDescriptorHandle()),
		uav(),
		mipLevels(inputMipLevels),
		format(inputFormat)
	{
		Resize(inputWidth, inputHeight);
	}

	UnorderedAccessBuffer::~UnorderedAccessBuffer()
	{
		ResourceStateTracker::RemoveGlobalResourceState(uavBuffer);
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

	void UnorderedAccessBuffer::Resize(const UINT inputWidth, const UINT inputHeight)
	{
		width = std::max(1u, inputWidth);
		height = std::max(1u, inputHeight);

		HRESULT hr;

		auto device = Graphics::GetInstance().GetDevice();

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		auto resDes = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height,
			1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		GFX_THROW_INFO(device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDes,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			nullptr,
			IID_PPV_ARGS(&uavBuffer)
		));
		ResourceStateTracker::AddGlobalResourceState(uavBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		device->CreateUnorderedAccessView(uavBuffer.Get(), nullptr, nullptr, cpuHandle);
	}
}

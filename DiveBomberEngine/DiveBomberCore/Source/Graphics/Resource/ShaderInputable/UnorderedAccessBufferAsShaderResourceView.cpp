#include "UnorderedAccessBufferAsShaderResourceView.h"

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

	UnorderedAccessBufferAsShaderResourceView::UnorderedAccessBufferAsShaderResourceView(
		std::shared_ptr<DescriptorAllocator> inputDescriptorAllocator,
		CD3DX12_RESOURCE_DESC inputDesc)
		:
		Resource(L"?"),
		descriptorAllocation(inputDescriptorAllocator->Allocate(1u)),
		cpuHandle(descriptorAllocation->GetCPUDescriptorHandle()),
		srv()
	{
		uavPointer = std::make_shared<UnorderedAccessBuffer>(inputDescriptorAllocator, inputDesc);
		Resize(inputDesc);
	}

	UnorderedAccessBufferAsShaderResourceView::~UnorderedAccessBufferAsShaderResourceView()
	{
	}

	void UnorderedAccessBufferAsShaderResourceView::BindAsShaderResource() noxnd
	{
		Graphics::GetInstance().GetCommandList()->AddTransitionBarrier(uavPointer->GetUnorderedAccessBuffer(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, true);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE UnorderedAccessBufferAsShaderResourceView::GetSRVCPUDescriptorHandle() const noexcept
	{
		return cpuHandle;
	}

	UINT UnorderedAccessBufferAsShaderResourceView::GetSRVDescriptorHeapOffset() const noexcept
	{
		return descriptorAllocation->GetBaseOffset();
	}

	std::shared_ptr<UnorderedAccessBuffer> UnorderedAccessBufferAsShaderResourceView::GetUAVPointer() noexcept
	{
		return uavPointer;
	}

	void UnorderedAccessBufferAsShaderResourceView::Resize(const CD3DX12_RESOURCE_DESC inputDesc)
	{
		uavPointer->Resize(inputDesc);

		auto device = Graphics::GetInstance().GetDevice();
		device->CreateShaderResourceView(uavPointer->GetUnorderedAccessBuffer().Get(), nullptr, cpuHandle);
	}

	void DiveBomber::DEResource::UnorderedAccessBufferAsShaderResourceView::Resize(UINT width, UINT height)
	{
		// Don't allow 0 size swap chain back buffers.
		width = std::max(1u, width);
		height = std::max(1u, height);

		CD3DX12_RESOURCE_DESC resourceDesc = uavPointer->GetResourceDesc();
		resourceDesc.Width = width;
		resourceDesc.Height = height;
		Resize(resourceDesc);
	}
}

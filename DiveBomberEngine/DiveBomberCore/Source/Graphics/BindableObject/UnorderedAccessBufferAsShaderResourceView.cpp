#include "UnorderedAccessBufferAsShaderResourceView.h"

#include "..\Graphics.h"
#include "..\..\Exception\GraphicsException.h"
#include "..\DX\DescriptorAllocator.h"
#include "..\DX\DescriptorAllocation.h"
#include "..\DX\ResourceStateTracker.h"
#include "..\DX\CommandList.h"

namespace DiveBomber::BindableObject
{
	using namespace DEGraphics;
	using namespace DEException;
	using namespace DX;

	UnorderedAccessBufferAsShaderResourceView::UnorderedAccessBufferAsShaderResourceView(UINT inputWidth, UINT inputHeight,
		std::shared_ptr<DescriptorAllocator> inputDescriptorAllocator,
		DXGI_FORMAT inputFormat, UINT inputMipLevels)
		:
		descriptorAllocation(inputDescriptorAllocator->Allocate(1u)),
		cpuHandle(descriptorAllocation->GetCPUDescriptorHandle()),
		srv()
	{
		uavPointer = std::make_shared<UnorderedAccessBuffer>(inputWidth, inputHeight, inputDescriptorAllocator, inputFormat, inputMipLevels);
		Resize(inputWidth, inputHeight);
	}

	UnorderedAccessBufferAsShaderResourceView::~UnorderedAccessBufferAsShaderResourceView()
	{
	}

	void UnorderedAccessBufferAsShaderResourceView::Bind() noxnd
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

	void UnorderedAccessBufferAsShaderResourceView::Resize(const UINT inputWidth, const UINT inputHeight)
	{
		uavPointer->Resize(inputWidth, inputHeight);

		auto device = Graphics::GetInstance().GetDevice();
		device->CreateShaderResourceView(uavPointer->GetUnorderedAccessBuffer().Get(), nullptr, cpuHandle);
	}
}

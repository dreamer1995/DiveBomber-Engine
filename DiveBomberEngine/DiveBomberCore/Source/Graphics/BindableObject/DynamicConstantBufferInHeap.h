#pragma once
#include "DynamicConstantBuffer.h"

#include "..\DX\DescriptorAllocator.h"
#include "..\DX\DescriptorAllocation.h"

namespace DiveBomber::BindableObject
{
	class DynamicConstantBufferInHeap final : public DynamicConstantBuffer
	{
	public:
		DynamicConstantBufferInHeap(const std::string& inputTag,
			const DynamicConstantProcess::CookedLayout& inputLayout)
			:
			DynamicConstantBuffer(inputTag, *inputLayout.ShareRoot(), DynamicConstantProcess::Buffer(inputLayout), 999u)
		{
			descriptorAllocation = DEGraphics::Graphics::GetInstance().GetDescriptorAllocator()->Allocate(1u);
		}

		DynamicConstantBufferInHeap(const std::string& inputTag,
			const DynamicConstantProcess::Buffer& inputBuffer)
			:
			DynamicConstantBuffer(inputTag, inputBuffer.GetRootLayoutElement(), inputBuffer, 999u)
		{
			descriptorAllocation = DEGraphics::Graphics::GetInstance().GetDescriptorAllocator()->Allocate(1u);
			UpdateCBV();
		}

		DynamicConstantBufferInHeap(const std::string& inputTag,
			const DynamicConstantProcess::LayoutElement& inputLayout, const DynamicConstantProcess::Buffer& inputBuffer)
			:
			DynamicConstantBuffer(inputTag, inputLayout, inputBuffer, 999u)
		{
			descriptorAllocation = DEGraphics::Graphics::GetInstance().GetDescriptorAllocator()->Allocate(1u);
			UpdateCBV();
		}

		void Bind() noxnd override
		{
		}

		virtual void Update(const DynamicConstantProcess::Buffer& buffer) override
		{
			DynamicConstantBuffer::Update(buffer);
			UpdateCBV();
		}

		[[nodiscard]] UINT GetCBVDescriptorHeapOffset()
		{
			return descriptorAllocation->GetBaseOffset();
		}

	private:
		void UpdateCBV()
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc;
			constantBufferViewDesc.BufferLocation = DynamicConstantBuffer::constantBuffer->GetGPUVirtualAddress();
			constantBufferViewDesc.SizeInBytes = Utility::AlignUp((UINT)DynamicConstantBuffer::bufferSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

			DEGraphics::Graphics::GetInstance().GetDevice()->CreateConstantBufferView(&constantBufferViewDesc, descriptorAllocation->GetCPUDescriptorHandle());
		}

	private:
		std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation;
	};
}
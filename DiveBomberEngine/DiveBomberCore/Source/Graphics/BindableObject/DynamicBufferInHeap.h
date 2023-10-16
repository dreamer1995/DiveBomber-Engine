#pragma once
#include "DynamicConstantBuffer.h"

#include "..\DX\DescriptorAllocator.h"
#include "..\DX\DescriptorAllocation.h"

namespace DiveBomber::BindableObject
{
	class DynamicBufferInHeap : public DynamicConstantBuffer
	{
	public:
		DynamicBufferInHeap(const std::string& inputTag,
			const DynamicConstantProcess::CookedLayout& inputLayout)
			:
			DynamicConstantBuffer(inputTag, *inputLayout.ShareRoot(), DynamicConstantProcess::Buffer(inputLayout), 999u)
		{
			descriptorAllocation = DEGraphics::Graphics::GetInstance().GetDescriptorAllocator()->Allocate(1u);
		}

		DynamicBufferInHeap(const std::string& inputTag,
			const DynamicConstantProcess::Buffer& inputBuffer)
			:
			DynamicConstantBuffer(inputTag, inputBuffer.GetRootLayoutElement(), inputBuffer, 999u)
		{
			descriptorAllocation = DEGraphics::Graphics::GetInstance().GetDescriptorAllocator()->Allocate(1u);
		}

		DynamicBufferInHeap(const std::string& inputTag,
			const DynamicConstantProcess::LayoutElement& inputLayout, const DynamicConstantProcess::Buffer& inputBuffer)
			:
			DynamicConstantBuffer(inputTag, inputLayout, inputBuffer, 999u)
		{
			descriptorAllocation = DEGraphics::Graphics::GetInstance().GetDescriptorAllocator()->Allocate(1u);
		}

		virtual void Bind() noxnd override = 0;

		virtual void Update(const DynamicConstantProcess::Buffer& buffer) override = 0;

		[[nodiscard]] UINT GetCBVDescriptorHeapOffset()
		{
			return descriptorAllocation->GetBaseOffset();
		}

	protected:
		std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation;
	};
}
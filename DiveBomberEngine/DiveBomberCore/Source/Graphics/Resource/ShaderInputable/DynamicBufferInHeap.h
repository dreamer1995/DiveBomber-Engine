#pragma once
#include "..\DynamicConstantBuffer.h"
#include "ShaderInputable.h"

#include "..\..\DX\DescriptorAllocator.h"
#include "..\..\DX\DescriptorAllocation.h"

namespace DiveBomber::DEResource
{
	class DynamicBufferInHeap : public DynamicConstantBuffer, public ShaderInputable
	{
	public:
		DynamicBufferInHeap(const std::wstring& inputName,
			const DynamicConstantProcess::CookedLayout& inputLayout)
			:
			DynamicConstantBuffer(inputName, inputLayout, 999u),
			descriptorAllocation(DEGraphics::Graphics::GetInstance().GetDescriptorAllocator()->Allocate(1u))
		{
		}

		DynamicBufferInHeap(const std::wstring& inputName,
			const DynamicConstantProcess::Buffer& inputBuffer)
			:
			DynamicBufferInHeap(inputName, inputBuffer.GetRootLayoutElement(), inputBuffer)
		{
		}

		DynamicBufferInHeap(const std::wstring& inputName,
			const DynamicConstantProcess::LayoutElement& inputLayout,
			const DynamicConstantProcess::Buffer& inputBuffer)
			:
			DynamicConstantBuffer(inputName, inputLayout, inputBuffer, 999u),
			descriptorAllocation(DEGraphics::Graphics::GetInstance().GetDescriptorAllocator()->Allocate(1u))
		{
		}

		virtual void Update(const DynamicConstantProcess::Buffer& buffer) override
		{
			DynamicConstantBuffer::Update(buffer);
			UpdateCBV();
		}

		[[nodiscard]] UINT GetSRVDescriptorHeapOffset() const noexcept override
		{
			return descriptorAllocation->GetBaseOffset();
		}

	protected:
		std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation;

	private:
		virtual void UpdateCBV() = 0;
	};
}
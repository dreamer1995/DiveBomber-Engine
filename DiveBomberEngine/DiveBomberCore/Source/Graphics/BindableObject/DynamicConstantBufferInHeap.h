#pragma once
#include "DynamicBufferInHeap.h"

namespace DiveBomber::BindableObject
{
	class DynamicConstantBufferInHeap final : public DynamicBufferInHeap
	{
	public:
		DynamicConstantBufferInHeap(const std::string& inputTag,
			const DynamicConstantProcess::CookedLayout& inputLayout)
			:
			DynamicBufferInHeap(inputTag, inputLayout)
		{
		}

		DynamicConstantBufferInHeap(const std::string& inputTag,
			const DynamicConstantProcess::Buffer& inputBuffer)
			:
			DynamicBufferInHeap(inputTag, inputBuffer)
		{
			UpdateCBV();
		}

		DynamicConstantBufferInHeap(const std::string& inputTag,
			const DynamicConstantProcess::LayoutElement& inputLayout, const DynamicConstantProcess::Buffer& inputBuffer)
			:
			DynamicBufferInHeap(inputTag, inputLayout, inputBuffer)
		{
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

	private:
		void UpdateCBV()
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc;
			constantBufferViewDesc.BufferLocation = constantBuffer->GetGPUVirtualAddress();
			constantBufferViewDesc.SizeInBytes = Utility::AlignUp((UINT)bufferSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

			DEGraphics::Graphics::GetInstance().GetDevice()->CreateConstantBufferView(&constantBufferViewDesc, descriptorAllocation->GetCPUDescriptorHandle());
		}
	};
}
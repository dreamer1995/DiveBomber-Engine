#pragma once
#include "DynamicBufferInHeap.h"

namespace DiveBomber::GraphicResource
{
	class DynamicConstantBufferInHeap final : public DynamicBufferInHeap
	{
	public:
		DynamicConstantBufferInHeap(const std::wstring& inputName,
			const DynamicConstantProcess::CookedLayout& inputLayout)
			:
			DynamicBufferInHeap(inputName, inputLayout)
		{
		}

		DynamicConstantBufferInHeap(const std::wstring& inputName,
			const DynamicConstantProcess::Buffer& inputBuffer)
			:
			DynamicConstantBufferInHeap(inputName, inputBuffer.GetRootLayoutElement(), inputBuffer)
		{
		}

		DynamicConstantBufferInHeap(const std::wstring& inputName,
			const DynamicConstantProcess::LayoutElement& inputLayout,
			const DynamicConstantProcess::Buffer& inputBuffer)
			:
			DynamicBufferInHeap(inputName, inputLayout, inputBuffer)
		{
			UpdateCBV();
		}

private:
		virtual void UpdateCBV() override
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc;
			constantBufferViewDesc.BufferLocation = constantBuffer->GetGPUVirtualAddress();
			constantBufferViewDesc.SizeInBytes = Utility::AlignUp((UINT)bufferSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

			DEGraphics::Graphics::GetInstance().GetDevice()->CreateConstantBufferView(&constantBufferViewDesc, descriptorAllocation->GetCPUDescriptorHandle());
		}
	};
}
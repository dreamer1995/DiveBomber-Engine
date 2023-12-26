#pragma once
#include "DynamicBufferInHeap.h"

namespace DiveBomber::DEResource
{
	class DynamicStructuredBufferInHeap final : public DynamicBufferInHeap
	{
	public:
		DynamicStructuredBufferInHeap(const std::wstring& inputName,
			const DynamicConstantProcess::CookedLayout& inputLayout, size_t inputNumElements = 1)
			:
			DynamicBufferInHeap(inputName, inputLayout),
			numElements(inputNumElements)
		{
		}

		DynamicStructuredBufferInHeap(const std::wstring& inputName,
			const DynamicConstantProcess::Buffer& inputBuffer, size_t inputNumElements = 1)
			:
			DynamicStructuredBufferInHeap(inputName, inputBuffer.GetRootLayoutElement(), inputBuffer, inputNumElements)
		{
		}

		DynamicStructuredBufferInHeap(const std::wstring& inputName,
			const DynamicConstantProcess::LayoutElement& inputLayout, const DynamicConstantProcess::Buffer& inputBuffer, size_t inputNumElements = 1)
			:
			DynamicBufferInHeap(inputName, inputLayout, inputBuffer),
			numElements(inputNumElements)
		{
			UpdateCBV();
		}

		void SetNumElements(size_t size)
		{
			numElements = size;
		}
	private:
		virtual void UpdateCBV() override
		{
			const D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc
			{
				.Format = DXGI_FORMAT_UNKNOWN,
				.ViewDimension = D3D12_SRV_DIMENSION_BUFFER,
				.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
				.Buffer =
					{
						.FirstElement = 0u,
						.NumElements = (UINT)numElements,
						.StructureByteStride = static_cast<UINT>(dynamicBuffer.GetRootLayoutElement().GetSizeInBytes()),
					},
			};

			DEGraphics::Graphics::GetInstance().GetDevice()->CreateShaderResourceView(constantBuffer.Get(), &srvDesc, descriptorAllocation->GetCPUDescriptorHandle());
		}

	private:
		size_t numElements;
	};
}
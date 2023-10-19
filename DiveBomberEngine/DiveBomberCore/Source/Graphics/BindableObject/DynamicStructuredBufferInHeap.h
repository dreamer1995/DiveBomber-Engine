#pragma once
#include "DynamicBufferInHeap.h"

namespace DiveBomber::BindableObject
{
	class DynamicStructuredBufferInHeap final : public DynamicBufferInHeap
	{
	public:
		DynamicStructuredBufferInHeap(const std::string& inputTag,
			const DynamicConstantProcess::CookedLayout& inputLayout, size_t inputNumElements = 1)
			:
			DynamicBufferInHeap(inputTag, inputLayout),
			numElements(inputNumElements)
		{
		}

		DynamicStructuredBufferInHeap(const std::string& inputTag,
			const DynamicConstantProcess::Buffer& inputBuffer, size_t inputNumElements = 1)
			:
			DynamicBufferInHeap(inputTag, inputBuffer),
			numElements(inputNumElements)
		{
			UpdateCBV();
		}

		DynamicStructuredBufferInHeap(const std::string& inputTag,
			const DynamicConstantProcess::LayoutElement& inputLayout, const DynamicConstantProcess::Buffer& inputBuffer, size_t inputNumElements = 1)
			:
			DynamicBufferInHeap(inputTag, inputLayout, inputBuffer),
			numElements(inputNumElements)
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

		void SetNumElements(size_t size)
		{
			numElements = size;
		}
	private:
		void UpdateCBV()
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
						.StructureByteStride = static_cast<UINT>(dynamicBuffer.GetSizeInBytes()) / (UINT)numElements,
					},
			};

			DEGraphics::Graphics::GetInstance().GetDevice()->CreateShaderResourceView(constantBuffer.Get(), &srvDesc, descriptorAllocation->GetCPUDescriptorHandle());
		}

	private:
		size_t numElements;
	};
}
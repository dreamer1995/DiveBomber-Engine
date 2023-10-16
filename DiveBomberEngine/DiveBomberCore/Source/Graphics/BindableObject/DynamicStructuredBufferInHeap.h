#pragma once
#include "DynamicBufferInHeap.h"

namespace DiveBomber::BindableObject
{
	class DynamicStructuredBufferInHeap final : public DynamicBufferInHeap
	{
	public:
		DynamicStructuredBufferInHeap(const std::string& inputTag,
			const DynamicConstantProcess::CookedLayout& inputLayout)
			:
			DynamicBufferInHeap(inputTag, inputLayout)
		{
		}

		DynamicStructuredBufferInHeap(const std::string& inputTag,
			const DynamicConstantProcess::Buffer& inputBuffer)
			:
			DynamicBufferInHeap(inputTag, inputBuffer)
		{
			UpdateCBV();
		}

		DynamicStructuredBufferInHeap(const std::string& inputTag,
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
			const D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc
			{
				.Format = DXGI_FORMAT_UNKNOWN,
				.ViewDimension = D3D12_SRV_DIMENSION_BUFFER,
				.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(0,1,2,3) ,
				.Buffer =
					{
						.FirstElement = 0u,
						.NumElements = static_cast<UINT>(dynamicBuffer.GetSizeInBytes()) / static_cast<UINT>(dynamicBuffer.GetRootLayoutElement().GetSizeInBytes()),
						.StructureByteStride = static_cast<UINT>(dynamicBuffer.GetRootLayoutElement().GetSizeInBytes()),
					},
			};

			DEGraphics::Graphics::GetInstance().GetDevice()->CreateShaderResourceView(constantBuffer.Get(), &srvDesc, descriptorAllocation->GetCPUDescriptorHandle());
		}
	};
}
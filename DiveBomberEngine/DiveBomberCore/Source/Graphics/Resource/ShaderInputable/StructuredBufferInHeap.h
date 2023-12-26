#pragma once
#include "BufferInHeap.h"

namespace DiveBomber::DEResource
{
	template<typename C>
	class StructuredBufferInHeap final : public BufferInHeap<C>
	{
	public:
		StructuredBufferInHeap(const std::wstring& inputName, size_t inputNumElements = 1)
			:
			BufferInHeap<C>(inputName),
			numElements(inputNumElements)
		{
		}

		StructuredBufferInHeap(const std::wstring& inputName, const C& constantData, size_t inputNumElements = 1)
			:
			StructuredBufferInHeap<C>(inputName, &constantData, sizeof(constantData) * inputNumElements)
		{
		}

		StructuredBufferInHeap(const std::wstring& inputName, const C* constantData, size_t inputDataSize, size_t inputNumElements = 1)
			:
			BufferInHeap<C>(inputName, constantData, inputDataSize),
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
						.StructureByteStride = static_cast<UINT>(ConstantBuffer<C>::bufferSize) / (UINT)numElements,
					},
			};

			DEGraphics::Graphics::GetInstance().GetDevice()->CreateShaderResourceView(ConstantBuffer<C>::constantBuffer.Get(), &srvDesc, BufferInHeap<C>::descriptorAllocation->GetCPUDescriptorHandle());
		}

	private:
		size_t numElements;
	};
}
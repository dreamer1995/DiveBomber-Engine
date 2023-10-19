#pragma once
#include "BufferInHeap.h"

#include "..\DX\DescriptorAllocator.h"
#include "..\DX\DescriptorAllocation.h"

namespace DiveBomber::BindableObject
{
	template<typename C>
	class StructuredBufferInHeap final : public BufferInHeap<C>
	{
	public:
		StructuredBufferInHeap(const std::string& inputTag, size_t inputNumElements = 1)
			:
			BufferInHeap<C>(inputTag),
			numElements(inputNumElements)
		{
		}

		StructuredBufferInHeap(const std::string& inputTag, const C& constantData, size_t inputNumElements = 1)
			:
			BufferInHeap<C>(inputTag, &constantData),
			numElements(inputNumElements)
		{
			UpdateCBV();
		}

		StructuredBufferInHeap(const std::string& inputTag, const C* constantData, size_t inputDataSize, size_t inputNumElements = 1)
			:
			BufferInHeap<C>(inputTag, constantData, inputDataSize),
			numElements(inputNumElements)
		{
			UpdateCBV();
		}

		void Bind() noxnd override
		{
		}

		virtual void Update(const C& constantData) override
		{
			Update(&constantData, sizeof(constantData));
		}

		virtual void Update(const C* constantData, size_t dataSize) override
		{
			ConstantBuffer<C>::Update(constantData, dataSize);
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
						.StructureByteStride = static_cast<UINT>(ConstantBuffer<C>::bufferSize) / (UINT)numElements,
					},
			};

			DEGraphics::Graphics::GetInstance().GetDevice()->CreateShaderResourceView(ConstantBuffer<C>::constantBuffer.Get(), &srvDesc, BufferInHeap<C>::descriptorAllocation->GetCPUDescriptorHandle());
		}

	private:
		size_t numElements;
	};
}
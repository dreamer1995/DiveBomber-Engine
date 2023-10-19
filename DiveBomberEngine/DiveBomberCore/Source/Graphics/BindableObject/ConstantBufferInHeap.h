#pragma once
#include "BufferInHeap.h"

#include "..\DX\DescriptorAllocator.h"
#include "..\DX\DescriptorAllocation.h"

namespace DiveBomber::BindableObject
{
	template<typename C>
	class ConstantBufferInHeap final : public BufferInHeap<C>
	{
	public:
		ConstantBufferInHeap(const std::string& inputTag)
			:
			BufferInHeap<C>(inputTag)
		{
		}

		ConstantBufferInHeap(const std::string& inputTag, const C& constantData)
			:
			BufferInHeap<C>(inputTag, &constantData)
		{
			UpdateCBV();
		}

		ConstantBufferInHeap(const std::string& inputTag, const C* constantData, size_t inputDataSize)
			:
			BufferInHeap<C>(inputTag, constantData, inputDataSize)
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
	private:
		void UpdateCBV()
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc;
			constantBufferViewDesc.BufferLocation = ConstantBuffer<C>::constantBuffer->GetGPUVirtualAddress();
			constantBufferViewDesc.SizeInBytes = Utility::AlignUp(ConstantBuffer<C>::bufferSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

			DEGraphics::Graphics::GetInstance().GetDevice()->CreateConstantBufferView(&constantBufferViewDesc, BufferInHeap<C>::descriptorAllocation->GetCPUDescriptorHandle());
		}
	};
}
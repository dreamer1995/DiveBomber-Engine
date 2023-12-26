#pragma once
#include "BufferInHeap.h"

namespace DiveBomber::DEResource
{
	template<typename C>
	class ConstantBufferInHeap final : public BufferInHeap<C>
	{
	public:
		ConstantBufferInHeap(const std::wstring& inputName)
			:
			BufferInHeap<C>(inputName)
		{
		}

		ConstantBufferInHeap(const std::wstring& inputName, const C& constantData)
			:
			ConstantBufferInHeap(inputName, &constantData, sizeof(constantData))
		{
		}

		ConstantBufferInHeap(const std::wstring& inputName, const C* constantData,
			size_t inputDataSize)
			:
			BufferInHeap<C>(inputName, constantData, inputDataSize)
		{
			UpdateCBV();
		}

	private:
		virtual void UpdateCBV() override
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc;
			constantBufferViewDesc.BufferLocation = ConstantBuffer<C>::constantBuffer->GetGPUVirtualAddress();
			constantBufferViewDesc.SizeInBytes = Utility::AlignUp((UINT)ConstantBuffer<C>::bufferSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

			DEGraphics::Graphics::GetInstance().GetDevice()->CreateConstantBufferView(&constantBufferViewDesc, BufferInHeap<C>::descriptorAllocation->GetCPUDescriptorHandle());
		}
	};
}
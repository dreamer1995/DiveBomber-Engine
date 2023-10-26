#pragma once
#include "ConstantBuffer.h"

#include "..\..\DX\DescriptorAllocator.h"
#include "..\..\DX\DescriptorAllocation.h"

namespace DiveBomber::BindableObject
{
	template<typename C>
	class ConstantBufferInHeap : public ConstantBuffer<C>
	{
	public:
		ConstantBufferInHeap(const std::string& inputTag)
			:
			ConstantBuffer<C>(inputTag, 999u)
		{
			descriptorAllocation = DEGraphics::Graphics::GetInstance().GetDescriptorAllocator()->Allocate(1u);
		}

		ConstantBufferInHeap(const std::string& inputTag, const C& constantData)
			:
			ConstantBufferInHeap(inputTag, &constantData, sizeof(constantData))
		{
		}

		ConstantBufferInHeap(const std::string& inputTag, const C* constantData,
			size_t inputDataSize, bool updateCBV = true)
			:
			ConstantBuffer<C>(inputTag, constantData, inputDataSize, 999u)
		{
			descriptorAllocation = DEGraphics::Graphics::GetInstance().GetDescriptorAllocator()->Allocate(1u);
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
			if (dataSize != ConstantBuffer<C>::bufferSize)
			{
				UpdateCBV();
			}
		}

		[[nodiscard]] UINT GetCBVDescriptorHeapOffset()
		{
			return descriptorAllocation->GetBaseOffset();
		}

	protected:
		std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation;

	private:
		void UpdateCBV()
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc;
			constantBufferViewDesc.BufferLocation = ConstantBuffer<C>::constantBuffer->GetGPUVirtualAddress();
			constantBufferViewDesc.SizeInBytes = Utility::AlignUp((UINT)ConstantBuffer<C>::bufferSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

			DEGraphics::Graphics::GetInstance().GetDevice()->CreateConstantBufferView(&constantBufferViewDesc, descriptorAllocation->GetCPUDescriptorHandle());
		}
	};
}
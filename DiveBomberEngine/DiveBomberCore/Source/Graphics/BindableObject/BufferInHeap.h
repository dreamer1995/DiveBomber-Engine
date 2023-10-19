#pragma once
#include "ConstantBuffer.h"

#include "..\DX\DescriptorAllocator.h"
#include "..\DX\DescriptorAllocation.h"

namespace DiveBomber::BindableObject
{
	template<typename C>
	class BufferInHeap : public ConstantBuffer<C>
	{
	public:
		BufferInHeap(const std::string& inputTag)
			:
			ConstantBuffer<C>(inputTag, 999u)
		{
			descriptorAllocation = DEGraphics::Graphics::GetInstance().GetDescriptorAllocator()->Allocate(1u);
		}

		BufferInHeap(const std::string& inputTag, const C& constantData)
			:
			ConstantBuffer<C>(inputTag, &constantData, sizeof(constantData), 999u)
		{
			descriptorAllocation = DEGraphics::Graphics::GetInstance().GetDescriptorAllocator()->Allocate(1u);
		}

		BufferInHeap(const std::string& inputTag, const C* constantData, size_t inputDataSize)
			:
			ConstantBuffer<C>(inputTag, constantData, inputDataSize, 999u)
		{
			descriptorAllocation = DEGraphics::Graphics::GetInstance().GetDescriptorAllocator()->Allocate(1u);
		}

		void Bind() noxnd override = 0;

		virtual void Update(const C& constantData) override = 0;

		virtual void Update(const C* constantData, size_t dataSize) override = 0;

		[[nodiscard]] UINT GetCBVDescriptorHeapOffset()
		{
			return descriptorAllocation->GetBaseOffset();
		}

	protected:
		std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation;
	};
}
#pragma once
#include "..\ConstantBuffer.h"
#include "ShaderInputable.h"

#include "..\..\DX\DescriptorAllocator.h"
#include "..\..\DX\DescriptorAllocation.h"

namespace DiveBomber::DEResource
{
	template<typename C>
	class BufferInHeap : public ConstantBuffer<C>, public ShaderInputable
	{
	public:
		BufferInHeap(const std::wstring& inputName)
			:
			ConstantBuffer<C>(inputName, 999u),
			descriptorAllocation(DEGraphics::Graphics::GetInstance().GetDescriptorAllocator()->Allocate(1u))
		{
		}

		BufferInHeap(const std::wstring& inputName, const C& constantData)
			:
			BufferInHeap(inputName, &constantData, sizeof(constantData))
		{
		}

		BufferInHeap(const std::wstring& inputName, const C* constantData,
			size_t inputDataSize)
			:
			ConstantBuffer<C>(inputName, constantData, inputDataSize, 999u),
			descriptorAllocation(DEGraphics::Graphics::GetInstance().GetDescriptorAllocator()->Allocate(1u))
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

		[[nodiscard]] UINT GetSRVDescriptorHeapOffset() const noexcept override
		{
			return descriptorAllocation->GetBaseOffset();
		}

	protected:
		std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation;

	private:
		virtual void UpdateCBV() = 0;
	};
}
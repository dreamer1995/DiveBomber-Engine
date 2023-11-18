#pragma once
#include "..\ConstantBuffer.h"
#include "ShaderInputable.h"

#include "..\..\DX\DescriptorAllocator.h"
#include "..\..\DX\DescriptorAllocation.h"

namespace DiveBomber::DEResource
{
	template<typename C>
	class ConstantBufferInHeap : public ConstantBuffer<C>, public ShaderInputable
	{
	public:
		ConstantBufferInHeap(const std::wstring& inputName)
			:
			ConstantBuffer<C>(inputName, 999u)
		{
			descriptorAllocation = DEGraphics::Graphics::GetInstance().GetDescriptorAllocator()->Allocate(1u);
		}

		ConstantBufferInHeap(const std::wstring& inputName, const C& constantData)
			:
			ConstantBufferInHeap(inputName, &constantData, sizeof(constantData))
		{
		}

		ConstantBufferInHeap(const std::wstring& inputName, const C* constantData,
			size_t inputDataSize, bool updateCBV = true)
			:
			ConstantBuffer<C>(inputName, constantData, inputDataSize, 999u)
		{
			descriptorAllocation = DEGraphics::Graphics::GetInstance().GetDescriptorAllocator()->Allocate(1u);
			UpdateCBV();
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

		[[nodiscard]] UINT GetSRVDescriptorHeapOffset() const noexcept override
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
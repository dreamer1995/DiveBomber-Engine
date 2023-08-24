#pragma once
#include "ConstantBuffer.h"

#include "..\DX\DescriptorAllocator.h"
#include "..\DX\DescriptorAllocation.h"

namespace DiveBomber::BindableObject
{
	template<typename C>
	class ConstantBufferInHeap final : public ConstantBuffer<C>
	{
	public:
		ConstantBufferInHeap(DEGraphics::Graphics& gfx, const std::string& inputTag)
			:
			ConstantBuffer<C>(gfx, inputTag, 99u)
		{
			descriptorAllocation = gfx.GetDescriptorAllocator()->Allocate(1u);
		}

		ConstantBufferInHeap(DEGraphics::Graphics& gfx, const std::string& inputTag, const C* constantData, size_t inputDataSize)
			:
			ConstantBuffer<C>(gfx, inputTag, constantData, inputDataSize, 99u)
		{
			descriptorAllocation = gfx.GetDescriptorAllocator()->Allocate(1u);
			UpdateCBV(gfx);
		}

		void Bind(DEGraphics::Graphics& gfx) noxnd override
		{
		}

		virtual void Update(DEGraphics::Graphics& gfx, const C* constantData, size_t dataSize) override
		{
			ConstantBuffer<C>::Update(gfx, constantData, dataSize);
			UpdateCBV(gfx);
		}

		void UpdateCBV(DEGraphics::Graphics& gfx)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc;
			constantBufferViewDesc.BufferLocation = ConstantBuffer<C>::constantBuffer->GetGPUVirtualAddress();
			constantBufferViewDesc.SizeInBytes = Utility::AlignUp((UINT)ConstantBuffer<C>::bufferSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

			gfx.GetDecive()->CreateConstantBufferView(&constantBufferViewDesc, descriptorAllocation->GetCPUDescriptorHandle());
		}

		[[nodiscard]] UINT GetCBVDescriptorHeapOffset()
		{
			return descriptorAllocation->GetBaseOffset();
		}
	private:
		std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation;
	};
}
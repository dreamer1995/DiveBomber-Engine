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
		ConstantBufferInHeap(DEGraphics::Graphics& gfx, const std::string& inputTag, const C& constantData)
			:
			ConstantBuffer<C>(gfx, inputTag, constantData, 99u)
		{
			descriptorAllocation = gfx.GetDescriptorAllocator()->Allocate(1u);

			UpdateCBV(gfx, sizeof(constantData));
		}

		void Bind(DEGraphics::Graphics& gfx) noxnd override
		{
		}

		virtual void Update(DEGraphics::Graphics& gfx, const C& constantData) override
		{
			ConstantBuffer<C>::Update(gfx, constantData);
			UpdateCBV(gfx, sizeof(constantData));
		}

		void UpdateCBV(DEGraphics::Graphics& gfx, UINT size)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc;
			constantBufferViewDesc.BufferLocation = ConstantBuffer<C>::constantBuffer->GetGPUVirtualAddress();
			//constantBufferViewDesc.SizeInBytes = Utility::AlignUp(size, 256u);
			constantBufferViewDesc.SizeInBytes = 256u;

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
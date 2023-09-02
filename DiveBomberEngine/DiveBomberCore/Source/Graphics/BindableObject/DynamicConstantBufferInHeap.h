#pragma once
#include "DynamicConstantBuffer.h"

#include "..\DX\DescriptorAllocator.h"
#include "..\DX\DescriptorAllocation.h"

namespace DiveBomber::BindableObject
{
	class DynamicConstantBufferInHeap final : public DynamicConstantBuffer
	{
	public:
		DynamicConstantBufferInHeap(DEGraphics::Graphics& gfx, const std::string& inputTag,
			const DynamicConstantProcess::LayoutElement& inputLayout)
			:
			DynamicConstantBuffer(gfx, inputTag, inputLayout, nullptr, 999u)
		{
			descriptorAllocation = gfx.GetDescriptorAllocator()->Allocate(1u);
		}

		DynamicConstantBufferInHeap(DEGraphics::Graphics& gfx, const std::string& inputTag,
			const DynamicConstantProcess::Buffer* inputBuffer)
			:
			DynamicConstantBuffer(gfx, inputTag, inputBuffer->GetRootLayoutElement(), inputBuffer, 999u)
		{
			descriptorAllocation = gfx.GetDescriptorAllocator()->Allocate(1u);
			UpdateCBV(gfx);
		}

		DynamicConstantBufferInHeap(DEGraphics::Graphics& gfx, const std::string& inputTag,
			const DynamicConstantProcess::LayoutElement& inputLayout, const DynamicConstantProcess::Buffer* inputBuffer)
			:
			DynamicConstantBuffer(gfx, inputTag, inputLayout, inputBuffer, 999u)
		{
			descriptorAllocation = gfx.GetDescriptorAllocator()->Allocate(1u);
			UpdateCBV(gfx);
		}

		void Bind(DEGraphics::Graphics& gfx) noxnd override
		{
		}

		virtual void Update(DEGraphics::Graphics& gfx, const DynamicConstantProcess::Buffer* buffer) override
		{
			Update(gfx, buffer, buffer->GetSizeInBytes());
		}

		virtual void Update(DEGraphics::Graphics& gfx, const DynamicConstantProcess::Buffer* buffer, size_t dataSize) override
		{
			DynamicConstantBuffer::Update(gfx, buffer, dataSize);
			UpdateCBV(gfx);
		}

		void UpdateCBV(DEGraphics::Graphics& gfx)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc;
			constantBufferViewDesc.BufferLocation = DynamicConstantBuffer::constantBuffer->GetGPUVirtualAddress();
			constantBufferViewDesc.SizeInBytes = Utility::AlignUp((UINT)DynamicConstantBuffer::bufferSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

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
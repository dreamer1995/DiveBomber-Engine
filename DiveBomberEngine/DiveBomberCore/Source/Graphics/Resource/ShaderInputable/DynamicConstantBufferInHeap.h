#pragma once
#include "..\DynamicConstantBuffer.h"
#include "ShaderInputable.h"

#include "..\..\DX\DescriptorAllocator.h"
#include "..\..\DX\DescriptorAllocation.h"

namespace DiveBomber::DEResource
{
	class DynamicConstantBufferInHeap : public DynamicConstantBuffer, public ShaderInputable
	{
	public:
		DynamicConstantBufferInHeap(const std::wstring& inputName,
			const DynamicConstantProcess::CookedLayout& inputLayout)
			:
			DynamicConstantBuffer(inputName, inputLayout, 999u)
		{
			descriptorAllocation = DEGraphics::Graphics::GetInstance().GetDescriptorAllocator()->Allocate(1u);
		}

		DynamicConstantBufferInHeap(const std::wstring& inputName,
			const DynamicConstantProcess::Buffer& inputBuffer, bool updateCBV = true)
			:
			DynamicConstantBufferInHeap(inputName, inputBuffer.GetRootLayoutElement(), inputBuffer, updateCBV)
		{
		}

		DynamicConstantBufferInHeap(const std::wstring& inputName,
			const DynamicConstantProcess::LayoutElement& inputLayout,
			const DynamicConstantProcess::Buffer& inputBuffer, bool updateCBV = true)
			:
			DynamicConstantBuffer(inputName, inputLayout, inputBuffer, 999u)
		{
			descriptorAllocation = DEGraphics::Graphics::GetInstance().GetDescriptorAllocator()->Allocate(1u);
			if (updateCBV)
			{
				UpdateCBV(DynamicConstantBuffer::bufferSize);
			}
		}

		virtual void Update(const DynamicConstantProcess::Buffer& buffer) override
		{
			if (buffer.GetSizeInBytes() != DynamicConstantBuffer::bufferSize)
			{
				UpdateCBV(buffer.GetSizeInBytes());
			}
			DynamicConstantBuffer::Update(buffer);
		}

		[[nodiscard]] UINT GetSRVDescriptorHeapOffset() const noexcept override
		{
			return descriptorAllocation->GetBaseOffset();
		}

	protected:
		std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation;

	private:
		void UpdateCBV(size_t inputBufferSize)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc;
			constantBufferViewDesc.BufferLocation = constantBuffer->GetGPUVirtualAddress();
			constantBufferViewDesc.SizeInBytes = Utility::AlignUp((UINT)inputBufferSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

			DEGraphics::Graphics::GetInstance().GetDevice()->CreateConstantBufferView(&constantBufferViewDesc, descriptorAllocation->GetCPUDescriptorHandle());
		}
	};
}
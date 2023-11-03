#pragma once
#include "DynamicConstantBuffer.h"
#include "..\BindableShaderInput.h"

#include "..\..\DX\DescriptorAllocator.h"
#include "..\..\DX\DescriptorAllocation.h"

namespace DiveBomber::BindableObject
{
	class DynamicConstantBufferInHeap : public DynamicConstantBuffer, public BindableShaderInput
	{
	public:
		DynamicConstantBufferInHeap(const std::string& inputTag,
			const DynamicConstantProcess::CookedLayout& inputLayout)
			:
			DynamicConstantBuffer(inputTag, inputLayout, 999u)
		{
			descriptorAllocation = DEGraphics::Graphics::GetInstance().GetDescriptorAllocator()->Allocate(1u);
		}

		DynamicConstantBufferInHeap(const std::string& inputTag,
			const DynamicConstantProcess::Buffer& inputBuffer, bool updateCBV = true)
			:
			DynamicConstantBufferInHeap(inputTag, inputBuffer.GetRootLayoutElement(), inputBuffer, updateCBV)
		{
		}

		DynamicConstantBufferInHeap(const std::string& inputTag,
			const DynamicConstantProcess::LayoutElement& inputLayout,
			const DynamicConstantProcess::Buffer& inputBuffer, bool updateCBV = true)
			:
			DynamicConstantBuffer(inputTag, inputLayout, inputBuffer, 999u)
		{
			descriptorAllocation = DEGraphics::Graphics::GetInstance().GetDescriptorAllocator()->Allocate(1u);
			if (updateCBV)
			{
				UpdateCBV();
			}
		}

		void Bind() noxnd override
		{
		}

		virtual void Update(const DynamicConstantProcess::Buffer& buffer) override
		{
			DynamicConstantBuffer::Update(buffer);
			UpdateCBV();
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
			constantBufferViewDesc.BufferLocation = constantBuffer->GetGPUVirtualAddress();
			constantBufferViewDesc.SizeInBytes = Utility::AlignUp((UINT)bufferSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

			DEGraphics::Graphics::GetInstance().GetDevice()->CreateConstantBufferView(&constantBufferViewDesc, descriptorAllocation->GetCPUDescriptorHandle());
		}
	};
}
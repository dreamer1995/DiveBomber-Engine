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
				UpdateCBV();
			}
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
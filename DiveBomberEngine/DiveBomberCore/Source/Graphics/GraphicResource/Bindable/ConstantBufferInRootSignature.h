#pragma once
#include "Bindable.h"
#include "..\ConstantBuffer.h"

namespace DiveBomber::GraphicResource
{
	template<typename C>
	class ConstantBufferInRootSignature : public ConstantBuffer<C>, public Bindable
	{
	public:
		ConstantBufferInRootSignature(const std::wstring& inputName,
			const C& constantData, UINT inputSlot)
			:
			ConstantBufferInRootSignature(inputName, &constantData, sizeof(constantData), inputSlot)
		{
		}

		ConstantBufferInRootSignature(const std::wstring& inputName,
			const C* constantData, size_t inputdataSize, UINT inputSlot)
			:
			ConstantBuffer<C>(inputName, constantData, inputdataSize, inputSlot)
		{
		}

		ConstantBufferInRootSignature(const std::wstring& inputName, UINT inputSlot)
			:
			ConstantBuffer<C>(inputName, inputSlot)
		{
		}

		~ConstantBufferInRootSignature()
		{
		}

		void Bind() noxnd override
		{
			DEGraphics::Graphics::GetInstance().GetGraphicsCommandList()->SetGraphicsRootConstantBufferView
			(
				ConstantBuffer<C>::slot, ConstantBuffer<C>::constantBuffer->GetGPUVirtualAddress()
			);

			DEGraphics::Graphics::GetInstance().GetGraphicsCommandList()->SetComputeRootConstantBufferView
			(
				ConstantBuffer<C>::slot, ConstantBuffer<C>::constantBuffer->GetGPUVirtualAddress()
			);
		}
	};
}
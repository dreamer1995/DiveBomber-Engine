#pragma once
#include "..\GraphicsHeader.h"

#include "..\BindableObject\ConstantBufferInHeap.h"
#include "..\BindableObject\DynamicConstantBufferInHeap.h"

#include <vector>

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::BindableObject
{
	class Texture;
}

namespace DiveBomber::Component
{
	class Material final
	{
	public:
		Material(DEGraphics::Graphics& gfx);
		void AddTexture(const std::shared_ptr<BindableObject::Texture> texture, UINT slot) noexcept;

		template<typename C>
		void AddConstant(const std::shared_ptr<BindableObject::ConstantBufferInHeap<C>> constant, UINT slot) noexcept
		{
			if (slot >= numConstantIndices)
			{
				UINT needInsert = slot - numConstantIndices + 1;
				std::vector<UINT>::iterator it = shaderResourceIndices.begin();
				shaderResourceIndices.insert(it + numConstantIndices, needInsert, 0u);
				numConstantIndices = slot + 1;
			}
			shaderResourceIndices[slot] = constant->GetCBVDescriptorHeapOffset();
		}

		void AddConstant(const std::shared_ptr<BindableObject::DynamicConstantBufferInHeap> constant, UINT slot) noexcept;

		void Bind(DEGraphics::Graphics& gfx) noxnd;
	private:
		std::shared_ptr<BindableObject::ConstantBuffer<UINT>> indexConstantBuffer;
		UINT numConstantIndices = 0;
		UINT numTextureIndices = 0;
		std::vector<UINT> shaderResourceIndices;
	};
}
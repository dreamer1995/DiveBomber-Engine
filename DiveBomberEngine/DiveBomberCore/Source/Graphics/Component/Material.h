#pragma once
#include "..\GraphicsHeader.h"

#include "..\BindableObject\ConstantBufferInHeap.h"

#include <map>
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
		struct IndexConstant
		{
			UINT transformIndex[1] = { 0 };
			UINT texureIndex[2] = { 0 };
		};
	public:
		Material(DEGraphics::Graphics& gfx);
		void AddTexture(const std::shared_ptr<BindableObject::Texture> texture, UINT slot) noexcept;

		template<typename C>
		void AddConstant(const std::shared_ptr<BindableObject::ConstantBufferInHeap<C>> constant, UINT slot) noexcept
		{
			indexConstant.transformIndex[slot] = constant->GetCBVDescriptorHeapOffset();
		}

		void Bind(DEGraphics::Graphics& gfx) noxnd;
	private:
		std::shared_ptr<BindableObject::ConstantBuffer<IndexConstant>> indexConstantBuffer;
		IndexConstant indexConstant;
	};
}
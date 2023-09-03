#pragma once
#include "..\GraphicsHeader.h"

#include "..\BindableObject\ConstantBufferInHeap.h"
#include "..\BindableObject\DynamicConstantBufferInHeap.h"

#include <vector>
#include <unordered_map>

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
		Material(DEGraphics::Graphics& inputGfx, const std::wstring inputName);
		void SetTexture(const std::shared_ptr<BindableObject::Texture> texture, UINT slot) noexcept;

		template<typename C>
		void SetConstant(const std::shared_ptr<BindableObject::ConstantBufferInHeap<C>> constant, UINT slot) noexcept
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

		void SetConstant(const std::string constantName, const std::shared_ptr<BindableObject::DynamicConstantBufferInHeap> constant, UINT slot) noexcept;

		void Bind(DEGraphics::Graphics& gfx) noxnd;

		[[nodiscard]] std::wstring GetName() const noexcept;

		void SetMaterialParameterScalar(std::string constantName, std::string key, float scalar) const noexcept;
		void SetMaterialParameterVector(std::string constantName, std::string key, DirectX::XMFLOAT4 vector) const noexcept;
	private:
		DEGraphics::Graphics& gfx;
		std::wstring name;
		std::shared_ptr<BindableObject::ConstantBuffer<UINT>> indexConstantBuffer;
		UINT numConstantIndices = 0;
		UINT numTextureIndices = 0;
		std::vector<UINT> shaderResourceIndices;
		std::unordered_map<std::string, std::shared_ptr<BindableObject::DynamicConstantBufferInHeap>> dynamicConstantMap;
	};
}
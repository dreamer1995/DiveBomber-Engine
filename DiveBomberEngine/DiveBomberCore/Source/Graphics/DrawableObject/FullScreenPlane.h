#pragma once
#include "Drawable.h"

#include <unordered_map>

namespace DiveBomber::BindableObject
{
	class Bindable;
	class IndexBuffer;
	class PipelineStateObject;
	class RenderTargetAsShaderResourceView;
}

namespace DiveBomber::Component
{
	class Mesh;
	class Material;
}

namespace DiveBomber::DrawableObject
{
	class FullScreenPlane final : public Drawable
	{
	public:
		FullScreenPlane(const std::wstring inputName = L"FullScreenPlane");
		~FullScreenPlane();

		void Bind() const noxnd override;
		[[nodiscard]] DirectX::XMMATRIX GetTransformXM() const noexcept override;

		void SetTexture(const std::string textureName, const std::shared_ptr<BindableObject::RenderTargetAsShaderResourceView> texture);

	private:
		std::shared_ptr<Component::Material> material;
	};
}
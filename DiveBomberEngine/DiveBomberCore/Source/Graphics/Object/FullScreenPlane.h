#pragma once
#include "Drawable.h"

namespace DiveBomber::DEResource
{
	class Bindable;
	class IndexBuffer;
	class PipelineStateObject;
	class ShaderInputable;
}

namespace DiveBomber::Component
{
	class Mesh;
	class Material;
}

namespace DiveBomber::DEObject
{
	class FullScreenPlane final : public Drawable
	{
	public:
		FullScreenPlane(const std::wstring inputName = L"FullScreenPlane");
		~FullScreenPlane();

		void Bind() const noxnd override;
		[[nodiscard]] DirectX::XMMATRIX GetTransformXM() const noexcept override;

		void SetTexture(const std::shared_ptr<DEResource::ShaderInputable> texture);

	private:
		std::shared_ptr<Component::Material> material;
		std::shared_ptr<Component::Mesh> mesh;
	};
}
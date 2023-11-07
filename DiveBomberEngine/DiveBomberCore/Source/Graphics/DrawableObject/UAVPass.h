#pragma once
#include "Drawable.h"

namespace DiveBomber::BindableObject
{
	class PipelineStateObject;
	class BindableShaderInput;
}

namespace DiveBomber::Component
{
	class Material;
}

namespace DiveBomber::DrawableObject
{
	class UAVPass final
	{
	public:
		UAVPass(const std::wstring inputName = L"FullScreenPlane");
		~UAVPass();

		void Bind() const noxnd;

		void SetTexture(const std::shared_ptr<BindableObject::BindableShaderInput> texture);

	private:
		std::wstring name;
		std::shared_ptr<Component::Material> material;
		std::shared_ptr<BindableObject::PipelineStateObject> pso;
	};
}
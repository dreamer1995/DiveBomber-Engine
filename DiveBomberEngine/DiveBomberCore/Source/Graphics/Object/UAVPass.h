#pragma once
#include "Drawable.h"

namespace DiveBomber::DEResource
{
	class PipelineStateObject;
	class ShaderInputable;
}

namespace DiveBomber::Component
{
	class Material;
}

namespace DiveBomber::DEObject
{
	class UAVPass final
	{
	public:
		UAVPass(const std::wstring inputName = L"FullScreenPlane");
		~UAVPass();

		void Execute() const noxnd;

		void SetTexture(const std::shared_ptr<DEResource::ShaderInputable> texture);

	private:
		std::wstring name;
		std::shared_ptr<Component::Material> material;
		std::shared_ptr<DEResource::PipelineStateObject> pso;
	};
}
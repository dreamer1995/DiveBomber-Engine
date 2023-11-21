#pragma once
#include "..\..\Window\DEWindows.h"
#include "..\..\Utility\Common.h"

#include <string>
#include <memory>

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

		void SetTexture(const std::shared_ptr<DEResource::ShaderInputable> texture, const UINT slot);

	private:
		std::wstring name;
		std::shared_ptr<Component::Material> material;
		std::shared_ptr<DEResource::PipelineStateObject> pso;
	};
}
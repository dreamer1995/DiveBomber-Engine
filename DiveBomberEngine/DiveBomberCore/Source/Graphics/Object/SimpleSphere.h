#pragma once
#include "Object.h"

#include <unordered_map>

namespace DiveBomber::DEResource
{
	class Bindable;
	class IndexBuffer;
	class PipelineStateObject;
}

namespace DiveBomber::Component
{
	class Mesh;
	class Material;
}

namespace DiveBomber::DEObject
{
	class SimpleSphere final : public Object
	{
	public:
		SimpleSphere(const std::wstring inputName = L"Sphere");
		~SimpleSphere();

		void Render() const noxnd override;

		[[nodiscard]] std::shared_ptr<Component::Material> GetMaterialByName(std::wstring name) const noexcept;
		[[nodiscard]] std::unordered_map<std::wstring, std::shared_ptr<Component::Material>> GetMaterials() const noexcept;
	private:
		std::unordered_map<std::wstring, std::shared_ptr<Component::Mesh>> meshMap;
		std::unordered_map<std::wstring, std::shared_ptr<Component::Material>> materialMap;
	};
}
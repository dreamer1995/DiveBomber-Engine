#pragma once
#include "..\Utility\Common.h"
#include <string>
#include <memory>
#include <map>
#include <vector>

namespace DiveBomber::DEObject
{
	class Drawable;
}

namespace DiveBomber::Component
{
	class Camera;
}

namespace DiveBomber::RenderPipeline
{
	class RenderPipelineGraph;
}

namespace DiveBomber::DEScene
{
	class Scene final
	{
	public:
		Scene();
		~Scene();

		void LoadSceneFromFile(const std::wstring name) noexcept;
		void Render() noxnd;
		[[nodiscard]] std::shared_ptr<Component::Camera> GetMainCamera() const noexcept;
		[[nodiscard]] std::shared_ptr<DEObject::Drawable> FindSceneObjectByName(std::wstring name) const noexcept;
		[[nodiscard]] std::multimap<std::wstring, std::shared_ptr<DEObject::Drawable>> GetSceneObjects() const noexcept;
	private:
		const std::wstring name;
		std::unique_ptr<RenderPipeline::RenderPipelineGraph> mainRenderPipeline;
		std::multimap<std::wstring, std::shared_ptr<DEObject::Drawable>> drawableObjects;
		std::shared_ptr<Component::Camera> mainCamera;
	};
}
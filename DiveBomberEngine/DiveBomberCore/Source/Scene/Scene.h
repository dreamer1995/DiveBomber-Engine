#pragma once
#include "..\Utility\Common.h"
#include <string>
#include <memory>
#include <map>
#include <vector>

namespace DiveBomber::DrawableObject
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
		std::shared_ptr<Component::Camera> GetMainCamera() const noexcept;
		std::shared_ptr<DrawableObject::Drawable> FindSceneObjectByName(std::wstring name) const noexcept;
	private:
		const std::wstring name;
		std::unique_ptr<RenderPipeline::RenderPipelineGraph> mainRenderPipeline;
		std::multimap <std::wstring, std::shared_ptr<DrawableObject::Drawable>> drawableObjects;
		std::shared_ptr<Component::Camera> mainCamera;
	};
}
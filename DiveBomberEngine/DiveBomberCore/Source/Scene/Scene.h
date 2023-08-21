#pragma once
#include "..\Utility\Common.h"
#include <string>
#include <memory>
#include <map>
#include <vector>

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

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

		void LoadSceneFromFile(DEGraphics::Graphics& gfx, const std::wstring name) noexcept;
		void Render(DEGraphics::Graphics& gfx) noxnd;
		std::shared_ptr<Component::Camera> GetMainCamera() const noexcept;
		std::shared_ptr<DrawableObject::Drawable> GetSceneObject() const noexcept;
	private:
		const std::wstring name;
		std::unique_ptr<RenderPipeline::RenderPipelineGraph> mainRenderPipeline;
		std::vector<std::shared_ptr<DrawableObject::Drawable>> drawableObjects;
		std::shared_ptr<Component::Camera> mainCamera;
	};
}
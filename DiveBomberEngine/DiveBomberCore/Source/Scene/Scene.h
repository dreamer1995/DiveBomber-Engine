#pragma once
#include "..\Utility\Common.h"
#include <string>
#include <memory>
#include <map>
#include <vector>

namespace DiveBomber::DEObject
{
	class Object;
}

namespace DiveBomber::DEComponent
{
	class Camera;
}

namespace DiveBomber::RenderPipeline
{
	class DeferredRenderPipeLine;
}

namespace DiveBomber::DEScene
{
	class Scene final
	{
	public:
		Scene();
		~Scene();

		void LoadSceneFromFile(const std::wstring inputName) noexcept;
		void Render() noxnd;
		[[nodiscard]] std::shared_ptr<DEComponent::Camera> GetMainCamera() const noexcept;
		[[nodiscard]] std::shared_ptr<DEObject::Object> FindSceneObjectByName(std::wstring name) const noexcept;
		[[nodiscard]] std::multimap<std::wstring, std::shared_ptr<DEObject::Object>> GetSceneObjects() const noexcept;
		[[nodiscard]] std::wstring GetName() const noexcept;
	private:
		std::wstring name = L"";
		std::unique_ptr<RenderPipeline::DeferredRenderPipeLine> mainRenderPipeline;
		std::multimap<std::wstring, std::shared_ptr<DEObject::Object>> drawableObjects;
		std::shared_ptr<DEComponent::Camera> mainCamera;
	};
}
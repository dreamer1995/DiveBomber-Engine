#pragma once
#include "..\Utility\Common.h"
#include "..\Utility\DEJson.h"
#include "..\Resource.h"
#include "..\ConfigDrivenResource.h"

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
	namespace fs = std::filesystem;

	class Scene final : public DiveBomber::Resource, public DiveBomber::ConfigDrivenResource
	{
	public:
		Scene(const fs::path inputPath);
		~Scene();

		void LoadSceneFromFile(const std::wstring inputName) noexcept;
		void Render() noxnd;
		[[nodiscard]] std::shared_ptr<DEComponent::Camera> GetMainCamera() const noexcept;
		[[nodiscard]] std::shared_ptr<DEObject::Object> FindSceneObjectByName(std::wstring name) const noexcept;
		[[nodiscard]] std::multimap<std::wstring, std::shared_ptr<DEObject::Object>> GetSceneObjects() const noexcept;
		void AddObjects(const std::shared_ptr<DEObject::Object> object) noexcept;

		void DrawDetailPanel() override;
		void CreateConfig() override;
		void SaveConfig() override;

	private:
		void GetConfig();

	private:
		std::unique_ptr<RenderPipeline::DeferredRenderPipeLine> mainRenderPipeline;
		std::multimap<std::wstring, std::shared_ptr<DEObject::Object>> drawableObjects;
		std::shared_ptr<DEComponent::Camera> mainCamera;
	};
}
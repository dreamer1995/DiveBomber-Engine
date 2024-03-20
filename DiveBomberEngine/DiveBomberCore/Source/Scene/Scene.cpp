#include "Scene.h"

#include "..\Graphics\Graphics.h"
#include "..\Graphics\DX\CommandQueue.h"
#include "..\Graphics\Object\SimpleSphere.h"
#include "..\Graphics\RenderPipeline\DeferredRenderPipeLine.h"
#include "..\Component\Camera\Camera.h"

namespace DiveBomber::DEScene
{
	using namespace DEGraphics;
	using namespace GraphicResource;
	using namespace DEObject;
	using namespace DEComponent;
	using namespace DX;
	using namespace RenderPipeline;

	Scene::Scene(const fs::path inputPath)
		:
		Resource(inputPath.stem()),
		ConfigDrivenResource(inputPath.wstring() + L".deasset")
	{
		GetConfig();
	}

	Scene::~Scene()
	{
	}

	void Scene::LoadScene() noexcept
	{
		mainRenderPipeline = std::make_unique<DeferredRenderPipeLine>();

		drawableObjects.emplace(L"Sphere01", std::make_shared<SimpleSphere>(L"Sphere01"));
		auto another = std::make_shared<SimpleSphere>(L"Sphere02");
		another->SetPos({ 2.0f,0,0 });
		drawableObjects.emplace(another->GetName(), another);
		std::shared_ptr<CommandQueue> commandQueue = Graphics::GetInstance().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
		
		uint64_t fenceValue = Graphics::GetInstance().ExecuteCommandList(D3D12_COMMAND_LIST_TYPE_COPY);
		commandQueue->WaitForFenceValue(fenceValue);

		Camera::CameraAttributes cameraAttr{};
		cameraAttr.position.z = -6.0f;
		cameraAttr.projectionAttributes.isPerspective = true;
		cameraAttr.isSceneCamera = true;
		mainCamera = std::make_shared<Camera>(L"Main Camera", cameraAttr);
		mainCamera->BindToGraphics(mainCamera);
	}

	void Scene::Render() noxnd
	{
		for (auto& drawableObject : drawableObjects)
		{
			mainRenderPipeline->SubmitObject(drawableObject.second);
		}

		mainRenderPipeline->Render();
	}

	std::shared_ptr<Camera> Scene::GetMainCamera() const noexcept
	{
		return mainCamera;
	}

	std::shared_ptr<Object> Scene::FindSceneObjectByName(std::wstring name) const noexcept
	{
		auto returnObject = drawableObjects.find(name);
		if (returnObject != drawableObjects.end())
		{
			return returnObject->second;
		}
		else
		{
			return nullptr;
		}
	}

	std::multimap<std::wstring, std::shared_ptr<Object>> Scene::GetSceneObjects() const noexcept
	{
		return drawableObjects;
	}

	void DiveBomber::DEScene::Scene::AddObjects(const std::shared_ptr<DEObject::Object> object) noexcept
	{
		drawableObjects.emplace(object->GetName(), object);

		json objectConfig = object->GetConfig();

		config["Object"].emplace_back(objectConfig);
	}

	void DiveBomber::DEScene::Scene::DrawDetailPanel()
	{
	}

	void DiveBomber::DEScene::Scene::CreateConfig()
	{
		json config;
		config["ConfigFileType"] = 3u;
		config["Object"] = {};

		// write prettified JSON to another file
		std::ofstream outFile(configFilePath);
		outFile << std::setw(4) << config << std::endl;
		outFile.close();
	}

	void DiveBomber::DEScene::Scene::SaveConfig()
	{
	}

	void DiveBomber::DEScene::Scene::GetConfig()
	{
		fs::path configFileCachePath(ProjectDirectoryW L"Cache\\Scene\\" + configFilePath.filename().wstring());

		ReadConfig(configFileCachePath);
	}
}
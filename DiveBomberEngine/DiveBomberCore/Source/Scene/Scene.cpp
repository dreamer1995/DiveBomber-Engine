#include "Scene.h"

#include "..\Graphics\Graphics.h"
#include "..\Graphics\DX\CommandQueue.h"
#include "..\Graphics\Object\SimpleSphere.h"
#include "..\Graphics\Component\Camera.h"
#include "..\Graphics\RenderPipeline\DeferredRenderPipeLine.h"

namespace DiveBomber::DEScene
{
	using namespace DEGraphics;
	using namespace DEResource;
	using namespace DEObject;
	using namespace Component;
	using namespace DX;
	using namespace RenderPipeline;

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	void Scene::LoadSceneFromFile(const std::wstring inputName) noexcept
	{
		name = inputName;

		mainRenderPipeline = std::make_unique<DeferredRenderPipeLine>();

		drawableObjects.emplace(L"Sphere01", std::make_shared<SimpleSphere>(L"Sphere01"));
		auto another = std::make_shared<SimpleSphere>(L"Sphere02");
		another->SetPos({ 2.0f,0,0 });
		drawableObjects.emplace(another->GetName(), another);
		std::shared_ptr<CommandQueue> commandQueue = Graphics::GetInstance().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
		
		uint64_t fenceValue = Graphics::GetInstance().ExecuteCommandList(D3D12_COMMAND_LIST_TYPE_COPY);
		commandQueue->WaitForFenceValue(fenceValue);

		Camera::CameraAttributes cameraAttr;
		cameraAttr.position.z = -6.0f;
		mainCamera = std::make_shared<Camera>(L"Main Camera", cameraAttr, false);
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

	std::wstring DiveBomber::DEScene::Scene::GetName() const noexcept
	{
		return name;
	}
}
#include "Scene.h"

#include "..\Graphics\Graphics.h"
#include "..\Graphics\DX\CommandQueue.h"
#include "..\Graphics\DrawableObject\SimpleSphere.h"
#include "..\Graphics\Component\Camera.h"
#include "..\Graphics\RenderPipeline\RenderPipelineGraph.h"

namespace DiveBomber::DEScene
{
	using namespace DEGraphics;
	using namespace DrawableObject;
	using namespace Component;
	using namespace DX;
	using namespace RenderPipeline;

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	void Scene::LoadSceneFromFile(Graphics& gfx, const std::wstring name) noexcept
	{
		mainRenderPipeline = std::make_unique<RenderPipelineGraph>();

		std::shared_ptr<CommandQueue> commandQueue = gfx.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
		drawableObjects.emplace_back(std::make_shared<SimpleSphere>(gfx));
		auto another = std::make_shared<SimpleSphere>(gfx);
		another->SetPos({ 2.0f,0,0 });
		drawableObjects.emplace_back(another);
		uint64_t fenceValue = gfx.ExecuteCommandList(D3D12_COMMAND_LIST_TYPE_COPY);
		commandQueue->WaitForFenceValue(fenceValue);

		Camera::CameraAttributes cameraAttr;
		cameraAttr.position.z = -6.0f;
		mainCamera = std::make_shared<Camera>(gfx, "Main Camera", cameraAttr, false);
		mainCamera->BindToGraphics(gfx, mainCamera);
	}

	void Scene::Render(Graphics& gfx) noxnd
	{
		mainRenderPipeline->Bind(gfx);
		for (auto& drawableObject : drawableObjects)
		{
			//drawableObject.second->Bind(gfx);
			drawableObject->Bind(gfx);
		}
	}

	std::shared_ptr<Camera> Scene::GetMainCamera() const noexcept
	{
		return mainCamera;
	}

	std::shared_ptr<Drawable> Scene::GetSceneObject() const noexcept
	{
		if (drawableObjects.size() > 0)
		{
			return drawableObjects[0];
		}

		return nullptr;
	}
}
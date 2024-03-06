#pragma once
#include "Utility/Common.h"

#include <thread>
#include <queue>
#include <string>

namespace DiveBomber::DEWindow
{
	class Window;
}

namespace DiveBomber::DEConsole
{
	class Console;
}

namespace DiveBomber::Utility
{
	class Timer;
}

namespace DiveBomber::DEScene
{
	class Scene;
}

namespace DiveBomber::DX
{
	class ShaderManager;
	class GlobalResourceManager;
}

namespace DiveBomber
{
	class ConfigDrivenResource;

	class DiveBomberCore final
	{
	public:
		DiveBomberCore();
		~DiveBomberCore();

		[[nodiscard]] static DiveBomberCore& GetInstance();
		static void Destructor() noexcept;

		[[nodiscard]] int GameLoop();
		void ExecuteConsoleCommand();
		void UpdateRenderStatus();
		[[nodiscard]] std::shared_ptr<DEScene::Scene> GetCurrentScene();
		[[nodiscard]] std::shared_ptr<ConfigDrivenResource> GetCurrentSelectedResource();
		void SetCurrentSelectedResource(std::shared_ptr<ConfigDrivenResource> resource);

		[[nodiscard]] float& GetSceneCameraSpeed() noexcept;

	private:
		void Start();
		void Update();
		void ProcessInput();
		void GameLogic();
		void RenderLogic();

	private:
		static std::unique_ptr<DiveBomberCore> instance;
		std::unique_ptr<DEConsole::Console> console;
		std::queue<std::thread> threadTasks;
		std::wstring command;

		std::unique_ptr<Utility::Timer> coreTimer;

		std::shared_ptr<DEScene::Scene> currentScene;
		std::shared_ptr<ConfigDrivenResource> currentSelectedResource;

		float sceneCameraSpeed = 1.0f;
	};
}
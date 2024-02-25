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

namespace DiveBomber::UI
{
	class DetailModifier;
}

namespace DiveBomber
{
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
		[[nodiscard]] std::shared_ptr<UI::DetailModifier> GetCurrentSelectedDetail();
		void SetCurrentSelectedDetail(std::shared_ptr<UI::DetailModifier> detail);

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
		std::shared_ptr<UI::DetailModifier> currentSelectedDetail;

		float sceneCameraSpeed = 1.0f;
	};
}
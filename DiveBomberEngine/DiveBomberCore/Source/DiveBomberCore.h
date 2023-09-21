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

namespace DiveBomber::BindableObject
{
	class GlobalBindableManager;
}

namespace DiveBomber::DX
{
	class ShaderManager;
}

namespace DiveBomber
{
	class DiveBomberCore final
	{
	public:
		DiveBomberCore();
		~DiveBomberCore();
		[[nodiscard]] int GameLoop();
		void ExecuteConsoleCommand();
		void RefreshRenderReport();

	private:
		void Start();
		void Update();
		void ProcessInput();
		void GameLogic();
		void RenderLogic();

		std::unique_ptr<DEConsole::Console> console;
		std::queue<std::thread> threadTasks;
		std::wstring command;

		std::unique_ptr<Utility::Timer> coreTimer;

		std::unique_ptr<DEScene::Scene> currentScene;
	};
}
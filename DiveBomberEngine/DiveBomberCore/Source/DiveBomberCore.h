#pragma once
#include "Utility/Common.h"
#include "Graphics\BindableObject\GlobalBindableManager.h"

#include <thread>
#include <queue>
#include <string>

namespace DiveBomber
{
	namespace DEWindow
	{
		class Window;
	}

	namespace DEConsole
	{
		class Console;
	}

	namespace Utility
	{
		class Timer;
	}

	namespace DEScene
	{
		class Scene;
	}

	namespace DEGraphics
	{
		class Graphics;
	}

	namespace BindableObject
	{
		class GlobalBindableManager;
	}

	class DiveBomberCore final
	{
	public:
		DiveBomberCore();
		~DiveBomberCore();
		[[nodiscard]] int GameLoop();
		void ExecuteConsoleCommand();
		void RefreshRenderReport();

		template<class T, typename...Params>
		std::shared_ptr<T> ResolveBindable(DEGraphics::Graphics& gfx, Params&&...p) noxnd
		{
			return globalBindableManager->Resolve<T>(gfx, std::forward<Params>(p)...);
		}

	private:
		void Start();
		void Update();
		void ProcessInput();
		void GameLogic();
		void RenderLogic();

		std::unique_ptr<DEWindow::Window> wnd;
		std::unique_ptr<DEConsole::Console> console;
		std::queue<std::thread> threadTasks;
		std::wstring command;

		std::unique_ptr<Utility::Timer> coreTimer;

		std::unique_ptr<DEScene::Scene> currentScene;

		std::unique_ptr<BindableObject::GlobalBindableManager> globalBindableManager;
	};
}
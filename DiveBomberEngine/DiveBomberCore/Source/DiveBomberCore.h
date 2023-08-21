#pragma once
#include "Utility/Common.h"

#include <thread>
#include <queue>
#include <string>
#include <unordered_map>

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
		class Bindable;
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
		std::shared_ptr<T> Resolve(DEGraphics::Graphics& gfx, Params&&...p) noxnd
		{
			const auto key = T::GenerateUID(std::forward<Params>(p)...);
			const auto i = binds.find(key);
			if (i == binds.end())
			{
				auto bind = std::make_shared<T>(gfx, std::forward<Params>(p)...);
				binds[key] = bind;
				return bind;
			}
			else
			{
				return std::static_pointer_cast<T>(i->second);
			}
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

		std::unordered_map<std::string, std::shared_ptr<BindableObject::Bindable>> binds;
	};
}
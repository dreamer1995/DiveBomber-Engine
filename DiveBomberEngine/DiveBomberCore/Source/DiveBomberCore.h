#pragma once
#include "Window\Window.h"
#include "Console\Console.h"
#include "Utility\Timer.h"

#include "Graphics\BindObj\VertexBuffer.h"
#include "Graphics\BindObj\IndexBuffer.h"
#include "Graphics\BindObj\Geometry\Sphere.h"

#include <thread>

namespace DiveBomber
{
	class DiveBomberCore final
	{
	public:
		DiveBomberCore();
		~DiveBomberCore();
		int GameLoop();
		void ExecuteConsoleCommand();
		void RefreshRenderReport();

	private:
		void Start();
		void Update();
		void ProcessInput();
		void GameLogic();
		void RenderLogic();

		std::unique_ptr<DEWindow::Window> wnd;
		std::unique_ptr<DEConsole::Console> console;
		std::vector<std::thread> threadTasks;
		std::wstring command;

		Utility::Timer coreTimer;

		std::shared_ptr<BindObj::VertexBuffer> vertexBuffer;
		std::shared_ptr<BindObj::IndexBuffer> indexBuffer;
		std::shared_ptr<BindObj::IndexedTriangleList> mesh;
	};
}
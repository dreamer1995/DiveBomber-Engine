#pragma once
#include "Window.h"
#include "Exception.h"
#include "Console.h"
#include "Timer.h"
#include "RenderStatistics.h"

#include <thread>
class DiveBomberCore
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

	std::unique_ptr<Window> wnd;
	std::unique_ptr<Console> console;
	std::vector<std::thread> threadTasks;
	std::wstring command;

	Timer coreTimer;
};


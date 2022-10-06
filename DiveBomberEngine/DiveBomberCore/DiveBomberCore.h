#include "Window.h"
#include "Exception.h"

#pragma once
class DiveBomberCore
{
public:
	DiveBomberCore();
	~DiveBomberCore();
	int GameLoop();

private:
	void Start();
	void Update();
	void ProcessInput();
	void GameLogic();

	std::unique_ptr<Window> wnd;
};


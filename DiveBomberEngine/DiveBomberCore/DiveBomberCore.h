#include "Window.h"

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

	std::unique_ptr<Window> wnd;
};


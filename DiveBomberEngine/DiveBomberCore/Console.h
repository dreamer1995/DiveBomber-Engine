#pragma once
#include "Window.h"

class Console
{
public:
	Console();
	~Console();
	Console(const Console&) = delete;
	Console& operator=(const Console&) = delete;
	void GetInput(std::wstring& command);

	bool waitForInput = true;
};


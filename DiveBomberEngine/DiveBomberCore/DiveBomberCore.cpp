#include "DiveBomberCore.h"

DiveBomberCore::DiveBomberCore()
{
	wnd = std::make_unique<Window>(1280, 720, L"DiveBomber Engine");
}

DiveBomberCore::~DiveBomberCore()
{}

int DiveBomberCore::GameLoop()
{
	Start();
	while (true)
	{
		// process all messages pending, but to not block for new messages
		if (const auto ecode = Window::ProcessMessages())
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		Update();
	}
}

void DiveBomberCore::Start()
{

}

void DiveBomberCore::Update()
{

}
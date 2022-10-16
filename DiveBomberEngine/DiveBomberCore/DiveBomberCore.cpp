#include "SystemConfig.h"
#include "DiveBomberCore.h"
#include <iostream>

DiveBomberCore::DiveBomberCore()
{
	wnd = std::make_unique<Window>(L"DiveBomber Engine");
	console = std::make_unique<Console>();
}

DiveBomberCore::~DiveBomberCore()
{
	console->waitForInput = false;
	FreeConsole();
	for (std::thread& task : threadTasks)
	{
		task.join();
	}
}

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
	threadTasks.emplace_back(std::thread{ &Console::GetInput, console.get(), std::ref(command)});
}

void DiveBomberCore::Update()
{
	ProcessInput();
}

void DiveBomberCore::ProcessInput()
{
	while (const auto e = wnd->kbd.ReadKey())
	{
		if (!(e->IsKeyDown()))
		{
			continue;
		}

		switch (e->GetCode())
		{
			//case VK_ESCAPE:
			//	if( wnd.CursorEnabled() )
			//	{
			//		wnd.DisableCursor();
			//		wnd.mouse.EnableRaw();
			//	}
			//	else
			//	{
			//		wnd.EnableCursor();
			//		wnd.mouse.DisableRaw();
			//	}
			//	break;
		case VK_F1:
			wnd->SetTitle(L"Keyboard Detected!");
			std::cout << "Keyboard Detected!" << std::endl;
			//OutputDebugString(L"Keyboard Detected!¹þ¹þ");
			break;
		}
	}

	static float cameraSpeed = 1.0f;
	while (!wnd->mouse.IsEmpty())
	{
		const auto e = wnd->mouse.Read();

		switch (e->GetType())
		{
		case Mouse::Event::Type::MBE_RDown:
		{
			//wnd->DisableCursor();
			//wnd->mouse.EnableRaw();
			wnd->SetTitle(L"RMB Down!");
			break;
		}
		case Mouse::Event::Type::MBE_RUp:
		{
			wnd->SetTitle(L"RMB Up!");
			break;
		}
		case Mouse::Event::Type::MBE_LDown:
		{
			wnd->SetTitle(L"LMB Down!");
			break;
		}
		case Mouse::Event::Type::MBE_LUp:
		{
			wnd->SetTitle(L"LMB Up!");
			break;
		}
		case Mouse::Event::Type::MBE_WheelDown:
		{
			wnd->SetTitle(L"Wheel Down!");
			break;
		}
		case Mouse::Event::Type::MBE_WheelUp:
		{
			wnd->SetTitle(L"Wheel Up!");
			break;
		}
		case Mouse::Event::Type::MBE_WheelFront:
		{
			cameraSpeed += 0.3f;
			wnd->SetTitle(std::to_wstring(cameraSpeed));
			break;
		}
		case Mouse::Event::Type::MBE_WheelBack:
		{
			cameraSpeed -= 0.3f;
			wnd->SetTitle(std::to_wstring(cameraSpeed));
			break;
		}
		}
	}

	if (wnd->kbd.KeyIsDown('W'))
	{
		wnd->SetTitle(L"[W]¼ü°´ÏÂ£¡");
	}

	ExecuteCommand();
}

void DiveBomberCore::ExecuteCommand()
{
	if (!command.empty())
	{
		std::wcout << L"[Execute]" << command << std::endl;
		command.clear();
	}
};
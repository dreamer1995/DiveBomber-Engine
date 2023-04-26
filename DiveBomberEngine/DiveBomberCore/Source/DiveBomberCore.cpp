#include "DiveBomberCore.h"

#include "..\Config\SystemConfig.h"
#include "Utility\RenderStatistics.h"

#include <iostream>

namespace DiveBomber
{
	using namespace DEWindow;
	using namespace DEConsole;
	using namespace Utility;
	using namespace Hardware;

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
				wnd->Gfx().Flush();

				// if return optional has value, means we're quitting so return exit code
				return *ecode;
			}
			Update();
		}
	}

	void DiveBomberCore::Start()
	{
		threadTasks.emplace_back(std::thread{ &Console::GetInput, console.get(), std::ref(command) });
	}

	void DiveBomberCore::Update()
	{
		g_DeltaTime = coreTimer.Mark() * TimerSpeed;
		RefreshRenderReport();

		ProcessInput();
		RenderLogic();
	}

	void DiveBomberCore::ProcessInput()
	{
		ExecuteConsoleCommand();

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
				if (wnd->kbd.KeyIsDown(VK_MENU))
				{
			case VK_F11:
				wnd->SetFullScreen(!wnd->isFullScreen);
				break;
				}
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
	}

	void DiveBomberCore::ExecuteConsoleCommand()
	{
		if (!command.empty())
		{
			if (console->waitForInput)
			{
				std::wcout << L"[Execute]" << command << std::endl;
			}
			command.clear();
			threadTasks.emplace_back(std::thread{ &Console::GetInput, console.get(), std::ref(command) });
		}
	};

	void DiveBomberCore::RenderLogic()
	{
		wnd->Gfx().BeginFrame();
		wnd->Gfx().EndFrame();
	}

	void DiveBomberCore::RefreshRenderReport()
	{
		g_FrameCounter++;

		//static double elapsedSeconds = 0.0;
		//static uint64_t elapsedFrames = 0;

		//elapsedSeconds += g_DeltaTime;
		//elapsedFrames++;

		//if (elapsedSeconds > 1.0f)
		//{
		//	g_FramePerSnd = elapsedFrames / elapsedSeconds / 1000;
		//	std::wcout << g_FramePerSnd << std::endl;
		//	elapsedSeconds = 0.0;
		//	elapsedFrames = 0;
		//}

		//std::wcout << wnd->Gfx().GetWidth() << std::endl;
		//std::wcout << wnd->windowWidth << std::endl;
	}
}
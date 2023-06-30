#include "DiveBomberCore.h"

#include "..\Config\SystemConfig.h"
#include "Utility\RenderStatistics.h"
#include "Graphics/Component/Camera.h"

#include <iostream>

namespace DiveBomber
{
	using namespace DEWindow;
	using namespace DEConsole;
	using namespace Utility;
	using namespace Hardware;

	DiveBomberCore::DiveBomberCore()
	{
		if (EnableConsole)
		{
			console = std::make_unique<Console>();
			threadTasks.emplace_back(std::thread{
				&Console::GetInput, console.get(), std::ref(command)
				});
		}
			
		wnd = std::make_unique<Window>(L"DiveBomber Engine");
	}

	DiveBomberCore::~DiveBomberCore()
	{
		if (EnableConsole)
		{
			console->waitForInput = false;
			FreeConsole();
			for (std::thread& task : threadTasks)
			{
				task.join();
			}
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
		using namespace DiveBomber::RenderPipeline;

		mainRenderPipeline = std::make_unique<RenderPipelineGraph>();
		mainRenderPipeline->LoadContent(wnd->Gfx());
	}

	void DiveBomberCore::Update()
	{
		g_FrameCounter++;
		g_DeltaTime = coreTimer.Mark() * TimerSpeed;
		g_GameTime += g_DeltaTime;
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
			case VK_F1:
				//showDemoWindow = true;
				break;
			//case VK_F3:
				//isWireframe = true;
				//break;
			//case VK_F4:
				//wnd->Gfx().isWireFrame = !wnd->Gfx().isWireFrame;
				//break;
			//case VK_F5:
				//if (uvPannel->showUV)
				//{
				//	uvPannel->showUV = false;
				//}
				//else
				//{
				//	uvPannel->showUV = true;
				//}
				//break;
				
			case VK_F11:
				if (wnd->kbd.KeyIsDown(VK_MENU))
				{
					wnd->SetFullScreen(!wnd->isFullScreen);
				}
				break;

			//case VK_SPACE:
				//if (isRotate)
				//{
				//	isRotate = false;
				//}
				//else
				//{
				//	isRotate = true;
				//}
				//break;

			//case VK_RETURN:
				//savingDepth = true;
				//break;
			}
		}

		std::shared_ptr<Component::Camera> mainCamera = mainRenderPipeline->GetMainCamera();
		float deltaTime = (float)g_DeltaTime;

		static float cameraSpeed = 1.0f;
		while (!wnd->mouse.IsEmpty())
		{
			const auto e = wnd->mouse.Read();

			switch (e->GetType())
			{
			case Mouse::Event::Type::MBE_RDown:
			{
				wnd->DisableCursor();
				wnd->mouse.EnableRaw();
				break;
			}
			case Mouse::Event::Type::MBE_RUp:
			{
				wnd->EnableCursor();
				wnd->mouse.DisableRaw();
				break;
			}

			case Mouse::Event::Type::MBE_LUp:
			{
				wnd->EnableCursor();
				wnd->mouse.DisableRaw();
				break;
			}
			case Mouse::Event::Type::MBE_LDown:
			{
				if (wnd->kbd.KeyIsDown(VK_MENU))
				{
					wnd->DisableCursor();
					wnd->mouse.EnableRaw();
					break;
				}

				if (wnd->kbd.KeyIsDown('L') || wnd->kbd.KeyIsDown(VK_SHIFT))
				{
					wnd->DisableCursor();
					wnd->mouse.EnableRaw();
					break;
				}
				break;
			}
			case Mouse::Event::Type::MBE_WheelFront:
			{
				if (wnd->mouse.RightIsDown())
				{
					cameraSpeed += 0.3;
				}
				else
				{
					mainCamera->Translate({ 0.0f,0.0f,10.0f * deltaTime });
				}
				break;
			}
			case Mouse::Event::Type::MBE_WheelBack:
			{
				if (wnd->mouse.RightIsDown())
				{
					cameraSpeed -= 0.3;
				}
				else
				{
					mainCamera->Translate({ 0.0f,0.0f,10.0f * -deltaTime });
				}
				break;
			}
			case Mouse::Event::Type::MBE_WheelDown:
			{
				wnd->DisableCursor();
				wnd->mouse.EnableRaw();
				break;
			}
			case Mouse::Event::Type::MBE_WheelUp:
			{
				wnd->EnableCursor();
				wnd->mouse.DisableRaw();
				break;
			}
			}
			cameraSpeed = std::clamp(cameraSpeed, 0.3f, 9.9f);
		}

		if (!wnd->CursorEnabled())
		{
			if (wnd->kbd.KeyIsDown('W'))
			{
				mainCamera->Translate({ 0.0f,0.0f,deltaTime * cameraSpeed });
			}
			if (wnd->kbd.KeyIsDown('A'))
			{
				mainCamera->Translate({ -deltaTime * cameraSpeed,0.0f,0.0f });
			}
			if (wnd->kbd.KeyIsDown('S'))
			{
				mainCamera->Translate({ 0.0f,0.0f,-deltaTime * cameraSpeed });
			}
			if (wnd->kbd.KeyIsDown('D'))
			{
				mainCamera->Translate({ deltaTime * cameraSpeed,0.0f,0.0f });
			}
			if (wnd->kbd.KeyIsDown('E'))
			{
				mainCamera->Translate({ 0.0f,deltaTime * cameraSpeed,0.0f });
			}
			if (wnd->kbd.KeyIsDown('Q'))
			{
				mainCamera->Translate({ 0.0f,-deltaTime * cameraSpeed,0.0f });
			}
		}

		if (wnd->kbd.KeyIsDown('F'))
		{
			mainCamera->LookZero({ 0.0f, 0.0f, 0.0f });
		}

		while (const auto delta = wnd->mouse.ReadRawDelta())
		{
			if (!wnd->CursorEnabled() && wnd->mouse.RightIsDown())
			{
				mainCamera->Rotate((float)delta->x, (float)delta->y);
			}
			else if (!wnd->CursorEnabled() && wnd->kbd.KeyIsDown(VK_MENU) && wnd->mouse.LeftIsDown())
			{
				mainCamera->RotateAround((float)delta->x, (float)delta->y, { 0.0f, 0.0f, 0.0f });
			}
			else if (!wnd->CursorEnabled() && wnd->mouse.WheelIsDown())
			{
				static float mKeyMoveSpeed = 0.1f;
				mainCamera->Translate({ -(float)delta->x * deltaTime * mKeyMoveSpeed,(float)delta->y * deltaTime * mKeyMoveSpeed,0.0f });
			}
			else if (!wnd->CursorEnabled() && (wnd->kbd.KeyIsDown('L') || wnd->kbd.KeyIsDown(VK_SHIFT)) && wnd->mouse.LeftIsDown())
			{
				//dLight.Rotate((float)delta->x, (float)delta->y);
				std::wcout << L"Light Rotated!" << std::endl;
			}
		}
	}

	void DiveBomberCore::ExecuteConsoleCommand()
	{
		if (!EnableConsole)
			return;
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

		mainRenderPipeline->Bind(wnd->Gfx());

		wnd->Gfx().EndFrame();
	}

	void DiveBomberCore::RefreshRenderReport()
	{
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
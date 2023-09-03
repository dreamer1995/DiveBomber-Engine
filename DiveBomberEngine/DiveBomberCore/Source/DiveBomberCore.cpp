#include "DiveBomberCore.h"

#include "Window\Window.h"
#include "Graphics\Graphics.h"
#include "Console\Console.h"
#include "Utility\Timer.h"
#include "Hardware\Keyboard.h"
#include "Hardware\Mouse.h"
#include "Graphics\Component\Camera.h"
#include "Utility\GlobalParameters.h"
#include "Scene\Scene.h"
#include "Graphics\BindableObject\GlobalBindableManager.h"
#include "Graphics\DX\ShaderManager.h"

#include "Graphics\DrawableObject\SimpleSphere.h"
#include "Graphics\Component\Material.h"

#include <iostream>

namespace DiveBomber
{
	using namespace DEWindow;
	using namespace DEConsole;
	using namespace Utility;
	using namespace Hardware;
	using namespace BindableObject;
	using namespace DEScene;
	using namespace DrawableObject;
	using namespace DX;
	using namespace Component;

	DiveBomberCore::DiveBomberCore()
	{
		coreTimer = std::make_unique<Timer>();

		if (EnableConsole)
		{
			console = std::make_unique<Console>();
			threadTasks.emplace(std::thread{
				&Console::GetInput, console.get(), std::ref(command)
				});
		}
			
		wnd = std::make_unique<Window>(L"DiveBomber Engine", *this);

		globalBindableManager = std::make_shared<GlobalBindableManager>();
		shaderManager = std::make_shared<ShaderManager>();
	}

	DiveBomberCore::~DiveBomberCore()
	{
		if (EnableConsole)
		{
			console->SetWaitForInput(false);
			FreeConsole();
			while (!threadTasks.empty())
			{
				std::thread& task = threadTasks.front();
				task.join();
				threadTasks.pop();
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
		currentScene = std::make_unique<Scene>();

		currentScene->LoadSceneFromFile(wnd->Gfx(), L"Test Scene");
	}

	void DiveBomberCore::Update()
	{
		g_FrameCounter++;
		g_RawDeltaTime = coreTimer->Mark();
		g_EngineTime += g_RawDeltaTime;
		g_DeltaTime = g_RawDeltaTime * g_TimerSpeed;
		g_GameTime += g_DeltaTime;

		RefreshRenderReport();

		ProcessInput();
		GameLogic();
		RenderLogic();
	}

	void DiveBomberCore::RenderLogic()
	{
		wnd->Gfx().BeginFrame();

		currentScene->Render(wnd->Gfx());

		wnd->Gfx().EndFrame();
	}

	void DiveBomberCore::ProcessInput()
	{
		ExecuteConsoleCommand();

		while (const auto e = wnd->kbd->ReadKey())
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
				if (wnd->kbd->KeyIsDown(VK_MENU))
				{
					wnd->SetFullScreen(!wnd->isFullScreen);
				}
				break;

			case VK_SPACE:
				if (g_TimerSpeed != 0.0f)
				{
					g_TimerSpeed = 0.0f;
				}
				else
				{
					g_TimerSpeed = TimerSpeed;
				}
				break;

				//case VK_RETURN:
					//savingDepth = true;
					//break;
			}
		}

		std::shared_ptr<Camera> mainCamera = currentScene->GetMainCamera();
		float deltaTime = (float)g_RawDeltaTime;

		static float cameraSpeed = 1.0f;
		while (!wnd->mouse->IsEmpty())
		{
			const auto e = wnd->mouse->Read();

			switch (e->GetType())
			{
			case Mouse::Event::Type::MBE_RDown:
			{
				wnd->DisableCursor();
				wnd->mouse->EnableRaw();
				break;
			}
			case Mouse::Event::Type::MBE_RUp:
			{
				wnd->EnableCursor();
				wnd->mouse->DisableRaw();
				break;
			}

			case Mouse::Event::Type::MBE_LUp:
			{
				wnd->EnableCursor();
				wnd->mouse->DisableRaw();
				break;
			}
			case Mouse::Event::Type::MBE_LDown:
			{
				if (wnd->kbd->KeyIsDown(VK_MENU))
				{
					wnd->DisableCursor();
					wnd->mouse->EnableRaw();
					break;
				}

				if (wnd->kbd->KeyIsDown('L') || wnd->kbd->KeyIsDown(VK_SHIFT))
				{
					wnd->DisableCursor();
					wnd->mouse->EnableRaw();
					break;
				}
				break;
			}
			case Mouse::Event::Type::MBE_WheelFront:
			{
				if (wnd->mouse->RightIsDown())
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
				if (wnd->mouse->RightIsDown())
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
				wnd->mouse->EnableRaw();
				break;
			}
			case Mouse::Event::Type::MBE_WheelUp:
			{
				wnd->EnableCursor();
				wnd->mouse->DisableRaw();
				break;
			}
			}
			cameraSpeed = std::clamp(cameraSpeed, 0.3f, 9.9f);
		}

		if (!wnd->CursorEnabled())
		{
			if (wnd->kbd->KeyIsDown('W'))
			{
				mainCamera->Translate({ 0.0f,0.0f,deltaTime * cameraSpeed });
			}
			if (wnd->kbd->KeyIsDown('A'))
			{
				mainCamera->Translate({ -deltaTime * cameraSpeed,0.0f,0.0f });
			}
			if (wnd->kbd->KeyIsDown('S'))
			{
				mainCamera->Translate({ 0.0f,0.0f,-deltaTime * cameraSpeed });
			}
			if (wnd->kbd->KeyIsDown('D'))
			{
				mainCamera->Translate({ deltaTime * cameraSpeed,0.0f,0.0f });
			}
			if (wnd->kbd->KeyIsDown('E'))
			{
				mainCamera->Translate({ 0.0f,deltaTime * cameraSpeed,0.0f });
			}
			if (wnd->kbd->KeyIsDown('Q'))
			{
				mainCamera->Translate({ 0.0f,-deltaTime * cameraSpeed,0.0f });
			}
		}

		if (wnd->kbd->KeyIsDown('F'))
		{
			mainCamera->LookZero({ 0.0f, 0.0f, 0.0f });
		}

		while (const auto delta = wnd->mouse->ReadRawDelta())
		{
			if (!wnd->CursorEnabled() && wnd->mouse->RightIsDown())
			{
				mainCamera->Rotate((float)delta->x, (float)delta->y);
			}
			else if (!wnd->CursorEnabled() && wnd->kbd->KeyIsDown(VK_MENU) && wnd->mouse->LeftIsDown())
			{
				mainCamera->RotateAround((float)delta->x, (float)delta->y, { 0.0f, 0.0f, 0.0f });
			}
			else if (!wnd->CursorEnabled() && wnd->mouse->WheelIsDown())
			{
				static float mKeyMoveSpeed = 0.1f;
				mainCamera->Translate({ -(float)delta->x * deltaTime * mKeyMoveSpeed,(float)delta->y * deltaTime * mKeyMoveSpeed,0.0f });
			}
			else if (!wnd->CursorEnabled() && (wnd->kbd->KeyIsDown('L') || wnd->kbd->KeyIsDown(VK_SHIFT)) && wnd->mouse->LeftIsDown())
			{
				//dLight.Rotate((float)delta->x, (float)delta->y);
				std::wcout << L"Light Rotated!" << std::endl;
			}
		}

		// Listen to CTRL+S for shader live update in a very simple fashion (from http://www.lofibucket.com/articles/64k_intro.html)
		static ULONGLONG lastLoadTime = GetTickCount64();
		if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState('S'))
		{
			const ULONGLONG tickCount = GetTickCount64();
			if (tickCount - lastLoadTime > 200)
			{
				// temp strategy 
				Sleep(100);
				shaderManager->ReCompileShader();
			}
			lastLoadTime = tickCount;
		}
	}

	void DiveBomberCore::ExecuteConsoleCommand()
	{
		if (!EnableConsole)
			return;
		if (!console->GetWaitForInput())
		{
			if (!command.empty())
			{
				std::wcout << L"[Execute]" << command << std::endl;
				if (command == L"refresh shader")
				{
					// temp strategy 
					Sleep(100);
					shaderManager->ReCompileShader();
				}
			}

			command.clear();

			std::thread& task = threadTasks.front();
			task.join();
			threadTasks.pop();

			console->SetWaitForInput(true);
			threadTasks.emplace(std::thread{ &Console::GetInput, console.get(), std::ref(command) });
		}
	};

	void DiveBomberCore::RefreshRenderReport()
	{
		static double elapsedSeconds = 0.0;
		static uint64_t elapsedFrames = 0;

		elapsedSeconds += g_RawDeltaTime;
		elapsedFrames++;

		if (elapsedSeconds > 1.0f)
		{
			g_FramePerSnd = float(elapsedFrames / elapsedSeconds);
			std::wcout << g_FramePerSnd << std::endl;
			elapsedSeconds = 0.0;
			elapsedFrames = 0;
		}

		//std::wcout << g_FramePerSnd << std::endl;
		//std::wcout << g_GameTime << std::endl;
	}

	void DiveBomberCore::GameLogic()
	{
		// Update the model matrix.
		float angle = (float)Utility::g_GameTime;

		{
			auto drawable = currentScene->FindSceneObjectByName(L"Sphere01");
			auto sphere = std::dynamic_pointer_cast<SimpleSphere>(drawable);
			if (sphere)
				sphere->SetRotation({ 0, angle ,0 });
			std::shared_ptr<Material> material = sphere->GetMaterialByName(sphere->GetName() + L"Material");
			//material->SetMaterialParameterVector(Utility::ToNarrow(sphere->GetName()) + "BaseMat0", "baseColor", { (float)g_GameTime * (float)g_DeltaTime,0.0f ,0.0f ,0.0f });
		}
		{
			auto drawable = currentScene->FindSceneObjectByName(L"Sphere02");
			auto sphere = std::dynamic_pointer_cast<SimpleSphere>(drawable);
			if (sphere)
				sphere->SetRotation({ 0, -angle ,0 });
		}
	}

	std::shared_ptr<GlobalBindableManager> DiveBomberCore::GetGlobalBindableManager() noxnd
	{
		return globalBindableManager;
	}

	std::shared_ptr<ShaderManager> DiveBomberCore::GetShaderManager() noxnd
	{
		return shaderManager;
	}
}
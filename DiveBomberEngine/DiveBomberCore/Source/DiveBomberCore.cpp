#include "DiveBomberCore.h"

#include "Window\Window.h"
#include "Graphics\Graphics.h"
#include "Graphics\DX\GlobalResourceManager.h"
#include "Graphics\DX\ShaderManager.h"
#include "Graphics\Object\SimpleSphere.h"
#include "Graphics\UI\UIManager.h"
#include "Console\Console.h"
#include "Utility\Timer.h"
#include "Utility\GlobalParameters.h"
#include "Hardware\Keyboard.h"
#include "Hardware\Mouse.h"
#include "Component\Camera\Camera.h"
#include "Component\Material.h"
#include "Scene\Scene.h"

#include <..\imgui\imgui.h>
#include <iostream>

namespace DiveBomber
{
	std::unique_ptr<DiveBomberCore> DiveBomberCore::instance;

	using namespace DEWindow;
	using namespace DEGraphics;
	using namespace DEConsole;
	using namespace Utility;
	using namespace Hardware;
	using namespace DEResource;
	using namespace DEScene;
	using namespace DEObject;
	using namespace DX;
	using namespace DEComponent;

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

		if (EditorMode)
		{
			// init imgui
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGui::StyleColorsDark();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
			io.ConfigFlags |= ImGuiConfigFlags_IsSRGB;
		}

		Graphics::GetInstance().PostInitializeGraphics();
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
		GlobalResourceManager::Destructor();
		ShaderManager::Destructor();
		Window::Destructor();
		Graphics::Destructor();
		if (EditorMode)
		{
			ImGui::DestroyContext();
		}
	}

	DiveBomberCore& DiveBomberCore::GetInstance()
	{
		if (instance == nullptr)
		{
			instance = std::make_unique<DiveBomberCore>();
		}
		return *instance;
	}

	void DiveBomberCore::Destructor() noexcept
	{
		if (instance != nullptr)
		{
			instance.reset();
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
				Graphics::GetInstance().Flush();

				// if return optional has value, means we're quitting so return exit code
				return *ecode;
			}

			Update();
		}
	}

	void DiveBomberCore::Start()
	{
		currentScene = std::make_shared<Scene>();

		currentScene->LoadSceneFromFile(L"Test Scene");
	}

	void DiveBomberCore::Update()
	{
		UpdateRenderStatus();

		ProcessInput();
		GameLogic();
		RenderLogic();
	}

	void DiveBomberCore::RenderLogic()
	{
		Graphics::GetInstance().BeginFrame();

		// Need more consideration
		if (EditorMode && g_EnableEditorUI)
		{
			UI::UIManager::GetInstance().DrawUI();
		}
		currentScene->Render();

		Graphics::GetInstance().EndFrame();
	}

	void DiveBomberCore::ProcessInput()
	{
		ExecuteConsoleCommand();

#define	wnd Window::GetInstance()

		while (const auto e = wnd.kbd->ReadKey())
		{
			if (!(e->IsKeyDown()))
			{
				continue;
			}

			switch (e->GetCode())
			{
			case VK_F1:
				g_EnableEditorUI = !g_EnableEditorUI;
				break;
				//case VK_F3:
					//isWireframe = true;
					//break;
				//case VK_F4:
					//wnd.Gfx().isWireFrame = !wnd.Gfx().isWireFrame;
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
				if (wnd.kbd->KeyIsDown(VK_MENU))
				{
					wnd.SetFullScreen(!wnd.isFullScreen);
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

		while (!wnd.mouse->IsEmpty())
		{
			const auto e = wnd.mouse->Read();

			switch (e->GetType())
			{
			case Mouse::Event::Type::MBE_RDown:
			{
				wnd.DisableCursor();
				wnd.mouse->EnableRaw();
				break;
			}
			case Mouse::Event::Type::MBE_RUp:
			{
				wnd.EnableCursor();
				wnd.mouse->DisableRaw();
				break;
			}

			case Mouse::Event::Type::MBE_LUp:
			{
				wnd.EnableCursor();
				wnd.mouse->DisableRaw();
				break;
			}
			case Mouse::Event::Type::MBE_LDown:
			{
				if (wnd.kbd->KeyIsDown(VK_MENU))
				{
					wnd.DisableCursor();
					wnd.mouse->EnableRaw();
					break;
				}

				if (wnd.kbd->KeyIsDown('L') || wnd.kbd->KeyIsDown(VK_SHIFT))
				{
					wnd.DisableCursor();
					wnd.mouse->EnableRaw();
					break;
				}
				break;
			}
			case Mouse::Event::Type::MBE_WheelFront:
			{
				if (wnd.mouse->RightIsDown())
				{
					sceneCameraSpeed += 0.3;
				}
				else
				{
					mainCamera->Translate({ 0.0f,0.0f,10.0f * deltaTime });
				}
				break;
			}
			case Mouse::Event::Type::MBE_WheelBack:
			{
				if (wnd.mouse->RightIsDown())
				{
					sceneCameraSpeed -= 0.3;
				}
				else
				{
					mainCamera->Translate({ 0.0f,0.0f,10.0f * -deltaTime });
				}
				break;
			}
			case Mouse::Event::Type::MBE_WheelDown:
			{
				wnd.DisableCursor();
				wnd.mouse->EnableRaw();
				break;
			}
			case Mouse::Event::Type::MBE_WheelUp:
			{
				wnd.EnableCursor();
				wnd.mouse->DisableRaw();
				break;
			}
			}
			sceneCameraSpeed = std::clamp(sceneCameraSpeed, 0.3f, 9.9f);
		}

		if (!wnd.CursorEnabled())
		{
			if (wnd.kbd->KeyIsDown('W'))
			{
				mainCamera->Translate({ 0.0f,0.0f,deltaTime * sceneCameraSpeed });
			}
			if (wnd.kbd->KeyIsDown('A'))
			{
				mainCamera->Translate({ -deltaTime * sceneCameraSpeed,0.0f,0.0f });
			}
			if (wnd.kbd->KeyIsDown('S'))
			{
				mainCamera->Translate({ 0.0f,0.0f,-deltaTime * sceneCameraSpeed });
			}
			if (wnd.kbd->KeyIsDown('D'))
			{
				mainCamera->Translate({ deltaTime * sceneCameraSpeed,0.0f,0.0f });
			}
			if (wnd.kbd->KeyIsDown('E'))
			{
				mainCamera->Translate({ 0.0f,deltaTime * sceneCameraSpeed,0.0f });
			}
			if (wnd.kbd->KeyIsDown('Q'))
			{
				mainCamera->Translate({ 0.0f,-deltaTime * sceneCameraSpeed,0.0f });
			}
		}

		if (wnd.kbd->KeyIsDown('F'))
		{
			mainCamera->LookZero({ 0.0f, 0.0f, 0.0f });
		}

		while (const auto delta = wnd.mouse->ReadRawDelta())
		{
			if (!wnd.CursorEnabled() && wnd.mouse->RightIsDown())
			{
				mainCamera->Rotate((float)delta->x, (float)delta->y);
			}
			else if (!wnd.CursorEnabled() && wnd.kbd->KeyIsDown(VK_MENU) && wnd.mouse->LeftIsDown())
			{
				mainCamera->RotateAround((float)delta->x, (float)delta->y, { 0.0f, 0.0f, 0.0f });
			}
			else if (!wnd.CursorEnabled() && wnd.mouse->WheelIsDown())
			{
				static float mKeyMoveSpeed = 0.1f;
				mainCamera->Translate({ -(float)delta->x * deltaTime * mKeyMoveSpeed,(float)delta->y * deltaTime * mKeyMoveSpeed,0.0f });
			}
			else if (!wnd.CursorEnabled() && (wnd.kbd->KeyIsDown('L') || wnd.kbd->KeyIsDown(VK_SHIFT)) && wnd.mouse->LeftIsDown())
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
				ShaderManager::GetInstance().ReLoadShader();
			}
			lastLoadTime = tickCount;
		}

#undef wnd
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
					ShaderManager::GetInstance().ReLoadShader();
				}

				if (command == L"refresh material")
				{
					auto objectsMap = currentScene->GetSceneObjects();
					for (auto& objectPair : objectsMap)
					{
						auto sphere = std::dynamic_pointer_cast<SimpleSphere>(objectPair.second);
						if (sphere)
						{
							auto materialMap = sphere->GetMaterials();
							for (auto& materialPair : materialMap)
							{
								materialPair.second->GetConfig();
								materialPair.second->ReloadConfig();
							}
						}
					}

					std::cout << "Refreshed all materials!" << std::endl;
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

	void DiveBomberCore::UpdateRenderStatus()
	{
		// Standard
		g_FrameCounter++;
		g_RawDeltaTime = coreTimer->Mark();
		g_EngineTime += g_RawDeltaTime;
		g_DeltaTime = g_RawDeltaTime * g_TimerSpeed;
		g_GameTime += g_DeltaTime;
		static double elapsedSeconds = 0.0;
		static uint64_t elapsedFrames = 0;

		// FPS
		elapsedSeconds += g_RawDeltaTime;
		elapsedFrames++;

		if (elapsedSeconds > 0.5f)
		{
			g_FramePerSnd = float(elapsedFrames / elapsedSeconds);
			// std::wcout << g_FramePerSnd << std::endl;
			elapsedSeconds = 0.0;
			elapsedFrames = 0;
		}
	}

	std::shared_ptr<Scene> DiveBomberCore::GetCurrentScene()
	{
		return currentScene;
	}

	std::shared_ptr<Object> DiveBomberCore::GetCurrentSelectedObject()
	{
		return currentSelectedObject;
	}

	void DiveBomberCore::SetCurrentSelectedObject(std::shared_ptr<Object> object)
	{
		currentSelectedObject = object;
	}

	float& DiveBomberCore::GetSceneCameraSpeed() noexcept
	{
		return sceneCameraSpeed;
	}

	void DiveBomberCore::GameLogic()
	{
		{
			auto drawable = currentScene->FindSceneObjectByName(L"Sphere01");
			auto sphere = std::dynamic_pointer_cast<SimpleSphere>(drawable);
			if (sphere)
			{
				auto rotation = sphere->GetRotation();
				sphere->SetRotation({ rotation.x, rotation.y + (float)Utility::g_DeltaTime ,rotation.z });
			}
			std::shared_ptr<Material> material = sphere->GetMaterialByName(sphere->GetName() + L"Material");
			material->SetMaterialParameterVector(Utility::ToNarrow(sphere->GetName()) + "BaseMat0", "baseColor", { std::abs(std::sinf((float)g_GameTime * 2)),0.0f,0.0f,0.0f });
		}
		{
			auto drawable = currentScene->FindSceneObjectByName(L"Sphere02");
			auto sphere = std::dynamic_pointer_cast<SimpleSphere>(drawable);
			if (sphere)
			{
				auto rotation = sphere->GetRotation();
				sphere->SetRotation({ rotation.x, rotation.y - (float)Utility::g_DeltaTime ,rotation.z });
			}
		}
	}
}
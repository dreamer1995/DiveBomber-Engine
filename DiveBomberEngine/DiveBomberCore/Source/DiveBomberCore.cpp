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
		if(EnableConsole)
			console = std::make_unique<Console>();
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
		if(EnableConsole)
			threadTasks.emplace_back(std::thread{ &Console::GetInput, console.get(), std::ref(command) });

		//auto commandQueue = wnd->Gfx().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
		//wnd->Gfx().copyCommandList = commandQueue->GetCommandList();

		using namespace DiveBomber::BindObj;
		using namespace DiveBomber::BindObj::VertexProcess;
		VertexLayout vl;
		vl.Append(VertexLayout::Position3D);
		vl.Append(VertexLayout::Normal);
		vl.Append(VertexLayout::Tangent);
		vl.Append(VertexLayout::Binormal);
		vl.Append(VertexLayout::Texture2D);
		mesh = std::make_shared<BindObj::IndexedTriangleList> (Sphere::MakeNormalUVed(vl, true));
		mesh->Transform(dx::XMMatrixScaling(1, 1, 1));
		const auto geometryTag = "$sphere." + std::to_string(1);
		vertexBuffer = std::make_shared<VertexBuffer>(wnd->Gfx(), geometryTag, mesh->vertices);
		indexBuffer = std::make_shared<IndexBuffer>(wnd->Gfx(), geometryTag, mesh->indices);

		auto vlv = vertexBuffer->GetLayout().GetD3DLayout();
		//auto vlv = std::vector<D3D12_INPUT_ELEMENT_DESC>();

		wnd->Gfx().Load(vlv);
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
				wnd->Gfx().eyePosition.z += 0.5f;
				wnd->SetTitle(std::to_wstring(cameraSpeed));
				break;
			}
			case Mouse::Event::Type::MBE_WheelBack:
			{
				cameraSpeed -= 0.3f;
				wnd->Gfx().eyePosition.z -= 0.5f;
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

		vertexBuffer->Bind(wnd->Gfx());
		indexBuffer->Bind(wnd->Gfx());

		wnd->Gfx().OnRender((float)Utility::g_GameTime);

		wnd->Gfx().GetCommandList()->DrawIndexedInstanced(indexBuffer->GetCount(), 1, 0, 0, 0);
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
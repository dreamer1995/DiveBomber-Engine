#include "Window.h"

#include "..\Exception\WindowException.h"
#include "..\..\EngineResource\Icon\resource.h"
#include "..\Hardware\Keyboard.h"
#include "..\Hardware\Mouse.h"
#include "..\Graphics\Graphics.h"
#include "..\DiveBomberCore.h"
#include "..\Utility\GlobalParameters.h"

#include <..\imgui\backends\imgui_impl_win32.h>
#include <shellapi.h> // For CommandLineToArgvW
#include <optional>
#include <cassert>
#include <memory>
#include <vector>
// #include "..\..\ThirdParty\WindowsMessageMap.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace DiveBomber::DEWindow
{
	using namespace DEGraphics;
	using namespace DEException;
	using namespace Hardware;

	// Window Class Stuff
	Window::WindowClass Window::WindowClass::wndClass;
	std::unique_ptr<Window> Window::instance;

	Window::WindowClass::WindowClass() noexcept
		:
		hInst(HINSTANCE(GetModuleHandle(nullptr)))
	{
		WNDCLASSEX wClass = { 0 };
		wClass.cbSize = sizeof(wClass);
		wClass.style = CS_HREDRAW | CS_VREDRAW;
		wClass.lpfnWndProc = HandleMsgSetup;
		wClass.cbClsExtra = 0;
		wClass.cbWndExtra = 0;
		wClass.hInstance = GetInstance();
		wClass.hIcon = static_cast<HICON>(LoadImage(
			GetInstance(), MAKEINTRESOURCE(IDI_ICON1),
			IMAGE_ICON, 32, 32, 0
		));
		wClass.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
		wClass.hbrBackground = /*(HBRUSH)(COLOR_BACKGROUND)*/nullptr;
		wClass.lpszMenuName = nullptr;
		wClass.lpszClassName = GetName();
		wClass.hIconSm = static_cast<HICON>(LoadImage(
			GetInstance(), MAKEINTRESOURCE(IDI_ICON1),
			IMAGE_ICON, 16, 16, 0
		));

		static HRESULT hr = ::RegisterClassEx(&wClass);
		assert(SUCCEEDED(hr));
	}

	Window::WindowClass::~WindowClass()
	{
		UnregisterClass(wndClassName, GetInstance());
	}

	const wchar_t* Window::WindowClass::GetName() noexcept
	{
		return wndClassName;
	}

	HINSTANCE Window::WindowClass::GetInstance() noexcept
	{
		return wndClass.hInst;
	}

	Window::Window()
		:
		screenWidth(GetSystemMetrics(SM_CXSCREEN)),
		screenHeight(GetSystemMetrics(SM_CYSCREEN)),
		title(MainWindowTitle)
	{
		// First check if a window with the given name already exists.
		//WindowNameMap::iterator windowIter = gs_WindowByName.find(windowName);
		//if (windowIter != gs_WindowByName.end())
		//{
		//	return windowIter->second;
		//}

		// Windows 10 Creators update adds Per Monitor V2 DPI awareness context.
		// Using this awareness context allows the client area of the window 
		// to achieve 100% scaling while still allowing non-client window content to 
		// be rendered in a DPI sensitive fashion.
		SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

		// calculate window size based on desired client region size
		RECT wRct;
		wRct.left = 0;
		wRct.right = windowWidth + wRct.left;
		wRct.top = 0;
		wRct.bottom = windowHeight + wRct.top;

		if (::AdjustWindowRect(&wRct, WS_OVERLAPPEDWINDOW, FALSE) == 0)
		{
			throw WND_LAST_EXCEPT();
		}

		// Center the window within the screen. Clamp to 0, 0 for the top-left corner.
		int windowX = std::max<int>(0, (screenWidth - windowWidth) / 2);
		int windowY = std::max<int>(0, (screenHeight - windowHeight) / 2);

		hWnd = ::CreateWindowEx(
			0L,
			WindowClass::GetName(),
			title,
			WS_OVERLAPPEDWINDOW,
			windowX,
			windowY,
			wRct.right - wRct.left,
			wRct.bottom - wRct.top,
			nullptr,
			nullptr,
			WindowClass::GetInstance(),
			this
		);

		::GetWindowRect(hWnd, &windowRect);

		assert(hWnd && "Failed to create window");

		// register mouse raw input device
		RAWINPUTDEVICE rid;
		rid.usUsagePage = 0x01; // mouse page
		rid.usUsage = 0x02; // mouse usage
		rid.dwFlags = 0;
		rid.hwndTarget = nullptr;
		if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
		{
			throw WND_LAST_EXCEPT();
		}

		kbd = std::make_unique<Keyboard>();
		mouse = std::make_unique<Mouse>();

		// newly created windows start off as hidden
		::ShowWindow(hWnd, SW_SHOW);

		if (EditorMode)
		{
			// Init ImGui Win32 Impl
			ImGui_ImplWin32_Init(hWnd);
		}
	}

	LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		// use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
		if (msg == WM_NCCREATE)
		{
			// extract ptr to window class from creation data
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
			// set WinAPI-managed user data to store ptr to window instance
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
			// set message proc to normal (non-setup) handler now that setup is finished
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
			// forward message to window instance handler
			return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
		}
		// if we get a message before the WM_NCCREATE message, handle with default handler
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		// retrieve ptr to window instance
		Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		// forward message to window instance handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		//static WindowsMessageMap wMM;
		//OutputDebugString(wMM(msg, lParam, wParam).c_str());
		ImGuiIO imio;

		if (EditorMode && Utility::g_EnableEditorUI)
		{
			if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			{
				return true;
			}
			imio = ImGui::GetIO();
		}

		switch (msg)
		{
			// we don't want the DefProc to handle this message because
			// we want our destructor to destroy the window, so return 0 instead of break
		case WM_CLOSE:
			::PostQuitMessage(0);
			return 0;
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
		case WM_SIZE:
		{
			RECT clientRect = {};
			::GetClientRect(hWnd, &clientRect);
			int width = clientRect.right - clientRect.left;
			int height = clientRect.bottom - clientRect.top;

			if (windowWidth != width || windowHeight != height)
			{
				windowWidth = width;
				windowHeight = height;
				Graphics::GetInstance().ReSizeMainRT(windowWidth, windowHeight);
			}
			break;
		}

		// clear keystate when window loses focus to prevent input getting "stuck"
		case WM_KILLFOCUS:
			kbd->ClearState();
			break;
		case WM_ACTIVATE:
			// confine/free cursor on window to foreground/background if cursor disabled
			if (!cursorEnabled)
			{
				if (wParam & WA_ACTIVE)
				{
					ConfineCursor();
					HideCursor();
				}
				else
				{
					FreeCursor();
					ShowCursor();
				}
			}
			break;

			/*********** KEYBOARD MESSAGES ***********/
		case WM_KEYDOWN:
			// syskey commands need to be handled to track ALT key (VK_MENU) and F10
		case WM_SYSKEYDOWN:
			// stifle this keyboard message if imgui wants to capture
			if (EditorMode && Utility::g_EnableEditorUI && imio.WantCaptureKeyboard)
			{
				break;
			}
			if (!(lParam & 0x40000000) || kbd->AutorepeatIsEnabled()) // filter autorepeat
			{
				kbd->OnKeyDown(static_cast<unsigned char>(wParam));
			}
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			// stifle this keyboard message if imgui wants to capture
			if (EditorMode && Utility::g_EnableEditorUI && imio.WantCaptureKeyboard)
			{
				break;
			}
			kbd->OnKeyUp(static_cast<unsigned char>(wParam));
			break;
		case WM_CHAR:
			// stifle this keyboard message if imgui wants to capture
			if (EditorMode && Utility::g_EnableEditorUI && imio.WantCaptureKeyboard)
			{
				break;
			}
			kbd->OnChar(static_cast<unsigned char>(wParam));
			break;
			/*********** END KEYBOARD MESSAGES ***********/

		//	/************* MOUSE MESSAGES ****************/
		case WM_MOUSEMOVE:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			// cursorless exclusive gets first dibs
			if (!cursorEnabled)
			{
				if (!mouse->IsInWindow())
				{
					SetCapture(hWnd);
					mouse->OnMouseEnter();
					HideCursor();
				}
				break;
			}
			// stifle this mouse message if imgui wants to capture
			if (EditorMode && Utility::g_EnableEditorUI && imio.WantCaptureMouse)
			{
				break;
			}
			// in client region -> log move, and log enter + capture mouse (if not previously in window)
			if (pt.x >= 0 && pt.x < windowWidth && pt.y >= 0 && pt.y < windowHeight)
			{
				mouse->OnMouseMove(pt.x, pt.y);
				if (!mouse->IsInWindow())
				{
					SetCapture(hWnd);
					mouse->OnMouseEnter();
				}
			}
			// not in client -> log move / maintain capture if button down
			else
			{
				if (wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON))
				{
					mouse->OnMouseMove(pt.x, pt.y);
				}
				// button up -> release capture / log event for leaving
				else
				{
					ReleaseCapture();
					mouse->OnMouseLeave();
				}
			}
			break;
		}
		case WM_LBUTTONDOWN:
		{
			SetForegroundWindow(hWnd);
			if (!cursorEnabled)
			{
				ConfineCursor();
				HideCursor();
			}
			// stifle this mouse message if imgui wants to capture
			if (EditorMode && Utility::g_EnableEditorUI && imio.WantCaptureMouse)
			{
				break;
			}
			mouse->OnLeftDown();
			break;
		}
		case WM_RBUTTONDOWN:
		{
			// stifle this mouse message if imgui wants to capture
			if (EditorMode && Utility::g_EnableEditorUI && imio.WantCaptureMouse)
			{
				break;
			}
			mouse->OnRightDown();
			break;
		}
		case WM_MBUTTONDOWN:
		{
			// stifle this mouse message if imgui wants to capture
			if (EditorMode && Utility::g_EnableEditorUI && imio.WantCaptureMouse)
			{
				break;
			}
			mouse->OnWheelDown();
			break;
		}
		case WM_LBUTTONUP:
		{
			// stifle this mouse message if imgui wants to capture
			if (EditorMode && Utility::g_EnableEditorUI && imio.WantCaptureMouse)
			{
				break;
			}
			const POINTS pt = MAKEPOINTS(lParam);
			mouse->OnLeftUp();
			// release mouse if outside of window
			if (pt.x < 0 || pt.x >= windowWidth || pt.y < 0 || pt.y >= windowHeight)
			{
				ReleaseCapture();
				mouse->OnMouseLeave();
			}
			break;
		}
		case WM_RBUTTONUP:
		{
			// stifle this mouse message if imgui wants to capture
			if (EditorMode && Utility::g_EnableEditorUI && imio.WantCaptureMouse)
			{
				break;
			}
			const POINTS pt = MAKEPOINTS(lParam);
			mouse->OnRightUp();
			// release mouse if outside of window
			if (pt.x < 0 || pt.x >= windowWidth || pt.y < 0 || pt.y >= windowHeight)
			{
				ReleaseCapture();
				mouse->OnMouseLeave();
			}
			break;
		}
		case WM_MBUTTONUP:
		{
			// stifle this mouse message if imgui wants to capture
			if (EditorMode && Utility::g_EnableEditorUI && imio.WantCaptureMouse)
			{
				break;
			}
			const POINTS pt = MAKEPOINTS(lParam);
			mouse->OnWheelUp();
			// release mouse if outside of window
			if (pt.x < 0 || pt.x >= windowWidth || pt.y < 0 || pt.y >= windowHeight)
			{
				ReleaseCapture();
				mouse->OnMouseLeave();
			}
			break;
		}
		case WM_MOUSEWHEEL:
		{
			// stifle this mouse message if imgui wants to capture
			if (EditorMode && Utility::g_EnableEditorUI && imio.WantCaptureMouse)
			{
				break;
			}
			const POINTS pt = MAKEPOINTS(lParam);
			const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
			mouse->OnWheelDelta(delta);
			break;
		}
		/************** END MOUSE MESSAGES **************/

		/************** RAW MOUSE MESSAGES **************/
		case WM_INPUT:
		{
			if (!mouse->RawEnabled())
			{
				break;
			}
			UINT size = 0;
			// first get the size of the input data
			if (GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lParam),
				RID_INPUT,
				nullptr,
				&size,
				sizeof(RAWINPUTHEADER)) == -1)
			{
				// bail msg processing if error
				break;
			}
			rawBuffer.resize(size);
			// read in the input data
			if (GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lParam),
				RID_INPUT,
				rawBuffer.data(),
				&size,
				sizeof(RAWINPUTHEADER)) != size)
			{
				// bail msg processing if error
				break;
			}
			// process the raw input data
			auto& ri = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
			if (ri.header.dwType == RIM_TYPEMOUSE &&
				(ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
			{
				mouse->OnRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
			}
			break;
		}
		/************** END RAW MOUSE MESSAGES **************/
		}

		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	Window::~Window()
	{
		if (hWnd)
		{
			DestroyWindow(hWnd);
		}
		if (EditorMode)
		{
			ImGui_ImplWin32_Shutdown();
		}
	}

	void Window::EnableCursor() noexcept
	{
		cursorEnabled = true;
		ShowCursor();
		if (EditorMode)
		{
			EnableImGuiMouse();
		}
		FreeCursor();
	}

	void Window::DisableCursor() noexcept
	{
		cursorEnabled = false;
		HideCursor();
		if (EditorMode)
		{
			DisableImGuiMouse();
		}
		ConfineCursor();
	}

	bool Window::CursorEnabled() const noexcept
	{
		return cursorEnabled;
	}

	void Window::ConfineCursor() noexcept
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		MapWindowPoints(hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
		ClipCursor(&rect);
	}

	void Window::FreeCursor() noexcept
	{
		ClipCursor(nullptr);
	}

	void Window::HideCursor() noexcept
	{
		while (::ShowCursor(FALSE) >= 0);
	}

	void Window::ShowCursor() noexcept
	{
		while (::ShowCursor(TRUE) < 0);
	}

	void Window::EnableImGuiMouse() noexcept
	{
		ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
	}

	void Window::DisableImGuiMouse() noexcept
	{
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
	}

	void Window::SetTitle(const std::wstring& title)
	{
		if (SetWindowText(hWnd, title.c_str()) == 0)
		{
			throw WND_LAST_EXCEPT();
		}
	}

	std::optional<int> Window::ProcessMessages() noexcept
	{
		MSG msg;
		// while queue has messages, remove and dispatch them (but do not block on empty queue)
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			// check for quit because peekmessage does not signal this via return val
			if (msg.message == WM_QUIT)
			{
				// return optional wrapping int (arg to PostQuitMessage is in wparam) signals quit
				return static_cast<int>(msg.wParam);
			}

			// TranslateMessage will post auxilliary WM_CHAR messages from key msgs
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// return empty optional when not quitting app
		return {};
	}

	void Window::SetFullScreen(bool fullScreen) noexcept
	{
		if (isFullScreen != fullScreen)
		{
			isFullScreen = fullScreen;

			if (isFullScreen) // Switching to fullscreen.
			{
				// Store the current window dimensions so they can be restored 
				// when switching out of fullscreen state.
				::GetWindowRect(hWnd, &windowRect);

				// Set the window style to a borderless window so the client area fills
				// the entire screen.
				UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

				::SetWindowLongW(hWnd, GWL_STYLE, windowStyle);

				// Query the name of the nearest display device for the window.
				// This is required to set the fullscreen dimensions of the window
				// when using a multi-monitor setup.
				HMONITOR hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
				MONITORINFOEX monitorInfo = {};
				monitorInfo.cbSize = sizeof(MONITORINFOEX);
				::GetMonitorInfo(hMonitor, &monitorInfo);

				::SetWindowPos(hWnd, HWND_TOPMOST,
					monitorInfo.rcMonitor.left,
					monitorInfo.rcMonitor.top,
					monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
					monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
					SWP_FRAMECHANGED | SWP_NOACTIVATE);

				::ShowWindow(hWnd, SW_MAXIMIZE);
			}
			else
			{
				// Restore all the window decorators.
				::SetWindowLong(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

				::SetWindowPos(hWnd, HWND_NOTOPMOST,
					windowRect.left,
					windowRect.top,
					windowRect.right - windowRect.left,
					windowRect.bottom - windowRect.top,
					SWP_FRAMECHANGED | SWP_NOACTIVATE);

				::ShowWindow(hWnd, SW_NORMAL);
			}
		}
	}

	Window& Window::GetInstance()
	{
		if (instance == nullptr)
		{
			instance = std::make_unique<Window>();
		}
		return *instance;
	}

	void Window::Destructor() noexcept
	{
		if (instance != nullptr)
		{
			instance.reset();
		}
	}

	HWND Window::GetHandle() const noexcept
	{
		return hWnd;
	}
}
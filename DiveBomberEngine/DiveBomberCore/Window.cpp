#include "Window.h"

// Window Class Stuff
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
{
	hInst = HINSTANCE(GetModuleHandle(NULL));

	WNDCLASSEX wClass = { 0 };
	wClass.cbSize = sizeof(wClass);
	wClass.style = CS_HREDRAW | CS_VREDRAW;
	wClass.lpfnWndProc = HandleMsgSetup;
	wClass.cbClsExtra = 0;
	wClass.cbWndExtra = 0;
	wClass.hInstance = GetInstance();
	wClass.hIcon = /*static_cast<HICON>(LoadImage(
		GetInstance(), MAKEINTRESOURCE(IDI_ICON1),
		IMAGE_ICON, 32, 32, 0
	));*/
	::LoadIcon(hInst, NULL);
	wClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wClass.lpszMenuName = NULL;
	wClass.lpszClassName = GetName();
	wClass.hIconSm = ::LoadIcon(hInst, NULL); 
	/*static_cast<HICON>(LoadImage(
		GetInstance(), MAKEINTRESOURCE(IDI_ICON1),
		IMAGE_ICON, 16, 16, 0
	));*/

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

Window::Window(int width, int height, const wchar_t* name)
{
	screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	windowWidth = width;
	windowHeight = height;
	title = name;

	// calculate window size based on desired client region size
	RECT wRct;
	wRct.left = 0;
	wRct.right = width + wRct.left;
	wRct.top = 0;
	wRct.bottom = height + wRct.top;

	if (::AdjustWindowRect(&wRct, WS_OVERLAPPEDWINDOW, FALSE) == 0)
	{
		throw GetLastError();
	}

	// Center the window within the screen. Clamp to 0, 0 for the top-left corner.
	int windowX = std::max<int>(0, (screenWidth - width) / 2);
	int windowY = std::max<int>(0, (screenHeight - height) / 2);

	hWnd = ::CreateWindowEx(
		NULL,
		WindowClass::GetName(),
		name,
		WS_OVERLAPPEDWINDOW,
		windowX,
		windowY,
		wRct.right - wRct.left,
		wRct.bottom - wRct.top,
		NULL,
		NULL,
		WindowClass::GetInstance(),
		NULL
	);

	assert(hWnd && "Failed to create window");

	// newly created windows start off as hidden
	::ShowWindow(hWnd, SW_SHOW);
}

Window::~Window()
{
	DestroyWindow(hWnd);
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
	//if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	//{
	//	return true;
	//}
	//const auto& imio = ImGui::GetIO();

	switch (msg)
	{
		// we don't want the DefProc to handle this message because
		// we want our destructor to destroy the window, so return 0 instead of break
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		// clear keystate when window loses focus to prevent input getting "stuck"
	//case WM_KILLFOCUS:
	//	kbd.ClearState();
	//	break;
	//case WM_ACTIVATE:
	//	// confine/free cursor on window to foreground/background if cursor disabled
	//	if (!cursorEnabled)
	//	{
	//		if (wParam & WA_ACTIVE)
	//		{
	//			ConfineCursor();
	//			HideCursor();
	//		}
	//		else
	//		{
	//			FreeCursor();
	//			ShowCursor();
	//		}
	//	}
	//	break;

	//	/*********** KEYBOARD MESSAGES ***********/
	//case WM_KEYDOWN:
	//	// syskey commands need to be handled to track ALT key (VK_MENU) and F10
	//case WM_SYSKEYDOWN:
	//	// stifle this keyboard message if imgui wants to capture
	//	if (imio.WantCaptureKeyboard)
	//	{
	//		break;
	//	}
	//	if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled()) // filter autorepeat
	//	{
	//		kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
	//	}
	//	break;
	//case WM_KEYUP:
	//case WM_SYSKEYUP:
	//	// stifle this keyboard message if imgui wants to capture
	//	if (imio.WantCaptureKeyboard)
	//	{
	//		break;
	//	}
	//	kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
	//	break;
	//case WM_CHAR:
	//	// stifle this keyboard message if imgui wants to capture
	//	if (imio.WantCaptureKeyboard)
	//	{
	//		break;
	//	}
	//	kbd.OnChar(static_cast<unsigned char>(wParam));
	//	break;
	//	/*********** END KEYBOARD MESSAGES ***********/

	//	/************* MOUSE MESSAGES ****************/
	//case WM_MOUSEMOVE:
	//{
	//	const POINTS pt = MAKEPOINTS(lParam);
	//	// cursorless exclusive gets first dibs
	//	if (!cursorEnabled)
	//	{
	//		if (!mouse.IsInWindow())
	//		{
	//			SetCapture(hWnd);
	//			mouse.OnMouseEnter();
	//			HideCursor();
	//		}
	//		break;
	//	}
	//	// stifle this mouse message if imgui wants to capture
	//	if (imio.WantCaptureMouse)
	//	{
	//		break;
	//	}
	//	// in client region -> log move, and log enter + capture mouse (if not previously in window)
	//	if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
	//	{
	//		mouse.OnMouseMove(pt.x, pt.y);
	//		if (!mouse.IsInWindow())
	//		{
	//			SetCapture(hWnd);
	//			mouse.OnMouseEnter();
	//		}
	//	}
	//	// not in client -> log move / maintain capture if button down
	//	else
	//	{
	//		if (wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON))
	//		{
	//			mouse.OnMouseMove(pt.x, pt.y);
	//		}
	//		// button up -> release capture / log event for leaving
	//		else
	//		{
	//			ReleaseCapture();
	//			mouse.OnMouseLeave();
	//		}
	//	}
	//	break;
	//}
	//case WM_LBUTTONDOWN:
	//{
	//	SetForegroundWindow(hWnd);
	//	if (!cursorEnabled)
	//	{
	//		ConfineCursor();
	//		HideCursor();
	//	}
	//	// stifle this mouse message if imgui wants to capture
	//	if (imio.WantCaptureMouse)
	//	{
	//		break;
	//	}
	//	const POINTS pt = MAKEPOINTS(lParam);
	//	mouse.OnLeftPressed(pt.x, pt.y);
	//	break;
	//}
	//case WM_RBUTTONDOWN:
	//{
	//	// stifle this mouse message if imgui wants to capture
	//	if (imio.WantCaptureMouse)
	//	{
	//		break;
	//	}
	//	const POINTS pt = MAKEPOINTS(lParam);
	//	mouse.OnRightPressed(pt.x, pt.y);
	//	break;
	//}
	//case WM_MBUTTONDOWN:
	//{
	//	// stifle this mouse message if imgui wants to capture
	//	if (imio.WantCaptureMouse)
	//	{
	//		break;
	//	}
	//	const POINTS pt = MAKEPOINTS(lParam);
	//	mouse.OnWheelPressed(pt.x, pt.y);
	//	break;
	//}
	//case WM_LBUTTONUP:
	//{
	//	// stifle this mouse message if imgui wants to capture
	//	if (imio.WantCaptureMouse)
	//	{
	//		break;
	//	}
	//	const POINTS pt = MAKEPOINTS(lParam);
	//	mouse.OnLeftReleased(pt.x, pt.y);
	//	// release mouse if outside of window
	//	if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height)
	//	{
	//		ReleaseCapture();
	//		mouse.OnMouseLeave();
	//	}
	//	break;
	//}
	//case WM_RBUTTONUP:
	//{
	//	// stifle this mouse message if imgui wants to capture
	//	if (imio.WantCaptureMouse)
	//	{
	//		break;
	//	}
	//	const POINTS pt = MAKEPOINTS(lParam);
	//	mouse.OnRightReleased(pt.x, pt.y);
	//	// release mouse if outside of window
	//	if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height)
	//	{
	//		ReleaseCapture();
	//		mouse.OnMouseLeave();
	//	}
	//	break;
	//}
	//case WM_MBUTTONUP:
	//{
	//	// stifle this mouse message if imgui wants to capture
	//	if (imio.WantCaptureMouse)
	//	{
	//		break;
	//	}
	//	const POINTS pt = MAKEPOINTS(lParam);
	//	mouse.OnWheelReleased(pt.x, pt.y);
	//	// release mouse if outside of window
	//	if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height)
	//	{
	//		ReleaseCapture();
	//		mouse.OnMouseLeave();
	//	}
	//	break;
	//}
	//case WM_MOUSEWHEEL:
	//{
	//	// stifle this mouse message if imgui wants to capture
	//	if (imio.WantCaptureMouse)
	//	{
	//		break;
	//	}
	//	const POINTS pt = MAKEPOINTS(lParam);
	//	const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
	//	mouse.OnWheelDelta(pt.x, pt.y, delta);
	//	break;
	//}
	///************** END MOUSE MESSAGES **************/

	///************** RAW MOUSE MESSAGES **************/
	//case WM_INPUT:
	//{
	//	if (!mouse.RawEnabled())
	//	{
	//		break;
	//	}
	//	UINT size;
	//	// first get the size of the input data
	//	if (GetRawInputData(
	//		reinterpret_cast<HRAWINPUT>(lParam),
	//		RID_INPUT,
	//		nullptr,
	//		&size,
	//		sizeof(RAWINPUTHEADER)) == -1)
	//	{
	//		// bail msg processing if error
	//		break;
	//	}
	//	rawBuffer.resize(size);
	//	// read in the input data
	//	if (GetRawInputData(
	//		reinterpret_cast<HRAWINPUT>(lParam),
	//		RID_INPUT,
	//		rawBuffer.data(),
	//		&size,
	//		sizeof(RAWINPUTHEADER)) != size)
	//	{
	//		// bail msg processing if error
	//		break;
	//	}
	//	// process the raw input data
	//	auto& ri = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
	//	if (ri.header.dwType == RIM_TYPEMOUSE &&
	//		(ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
	//	{
	//		mouse.OnRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
	//	}
	//	break;
	//}
	/************** END RAW MOUSE MESSAGES **************/
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
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
			return (int)msg.wParam;
		}

		// TranslateMessage will post auxilliary WM_CHAR messages from key msgs
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// return empty optional when not quitting app
	return {};
}
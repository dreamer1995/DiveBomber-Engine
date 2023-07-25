/******************************************************************************************
*	1.Window Class																		  *
*	2.Window 																			  *
*	3.KeyBoard																			  *
*	4.Exception																			  *
*	5.Mouse																				  *
* 	6.LinktoCore																		  *
*	7.Winmain to Core																   	  *
*	8.Grapics																		  	  *
******************************************************************************************/
#pragma once

#include "..\Window\DEWindows.h"

#include "..\..\Config\SystemConfig.h"
#include <string>
#include <optional>
#include <memory>
#include <vector>

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::Hardware
{
	class Keyboard;
	class Mouse;
}

namespace DiveBomber::DEWindow
{
	class Window final
	{
		// Window Class
		class WindowClass final
		{
		public:
			static const wchar_t* GetName() noexcept;
			static HINSTANCE GetInstance() noexcept;
		private:
			WindowClass() noexcept;
			~WindowClass();
			WindowClass(const WindowClass&) = delete;
			WindowClass& operator =(const WindowClass&) = delete;
			static constexpr const wchar_t* wndClassName = L"Fallshill Engine Window";
			static WindowClass wndClass;
			HINSTANCE hInst;
		};
	public:
		Window(const wchar_t* name);
		~Window();
		Window(const Window&) = delete;
		Window& operator = (const Window&) = delete;
		void SetTitle(const std::wstring& title);
		void EnableCursor() noexcept;
		void DisableCursor() noexcept;
		bool CursorEnabled() const noexcept;
		static std::optional<int> ProcessMessages() noexcept;
		DEGraphics::Graphics& Gfx();
		void SetFullScreen(bool fullScreen) noexcept;

		const wchar_t* title;
		std::unique_ptr<Hardware::Keyboard> kbd;
		std::unique_ptr<Hardware::Mouse> mouse;
		bool isFullScreen = false;

	private:
		LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		[[nodiscard]] static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		[[nodiscard]] static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		void ConfineCursor() noexcept;
		void FreeCursor() noexcept;
		void ShowCursor() noexcept;
		void HideCursor() noexcept;
		void EnableImGuiMouse() noexcept;
		void DisableImGuiMouse() noexcept;

		bool cursorEnabled = true;
		int windowWidth = MainWindowWidth;
		int windowHeight = MainWindowHeight;
		int screenWidth;
		int screenHeight;
		HWND hWnd;
		std::unique_ptr<DEGraphics::Graphics> pGfx;
		std::vector<BYTE> rawBuffer;
		std::string commandLine;
		RECT windowRect;
	};
}
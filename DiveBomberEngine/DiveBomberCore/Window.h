#pragma once

#define NOMINMAX

#include <Windows.h>
#include <shellapi.h> // For CommandLineToArgvW
#include <string>
#include <optional>
#include <cassert>
#include <memory>
#include <vector>

#include "Common.h"

class Window
{
	// Window Class
	class WindowClass
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
	Window(int width, int height, const wchar_t* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator = (const Window&) = delete;
	void SetTitle(const std::string& title);
	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	bool CursorEnabled() const noexcept;
	static std::optional<int> ProcessMessages() noexcept;
	//Graphics& Gfx();

	const wchar_t* title;
private:
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	bool cursorEnabled = true;
	int windowWidth;
	int windowHeight;
	int screenWidth;
	int screenHeight;
	HWND hWnd;
	//std::unique_ptr<Graphics> pGfx;
	std::vector<BYTE> rawBuffer;
	std::string commandLine;
};
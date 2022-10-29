#include "WindowException.h"

// Window Exception Stuff
WindowHrException::WindowHrException(int line, const char* file, HRESULT hr) noexcept
	:
	Exception(line, file),
	hr(hr)
{}

const wchar_t* WindowHrException::whatW() const noexcept
{
	std::wostringstream oss;
	oss << GetType() << std::endl
		<< "L[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << L" (" << (unsigned long)GetErrorCode() << L")" << std::endl
		<< L"[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const wchar_t* WindowHrException::GetType() const noexcept
{
	return L"Window Exception";
}

HRESULT WindowHrException::GetErrorCode() const noexcept
{
	return hr;
}

std::wstring WindowHrException::GetErrorDescription() const noexcept
{
	return TranslateErrorCode(hr);
}

const wchar_t* NoGfxException::GetType() const noexcept
{
	return L"Window Exception [No Graphics]";
}
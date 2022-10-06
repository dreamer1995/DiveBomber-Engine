#include "WindowException.h"

// Window Exception Stuff
std::string WindowException::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = NULL;
	// windows will allocate memory for err string and make our pointer point to it
	const DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&pMsgBuf), 0, NULL
	);
	// 0 string length returned indicates a failure
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	// copy error string from windows-allocated buffer to std::string
	std::string errorString = pMsgBuf;
	// free windows buffer
	LocalFree(pMsgBuf);
	return errorString;
}


HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	:
	WindowException(line, file),
	hr(hr)
{}

const char* HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* HrException::GetType() const noexcept
{
	return "Window Exception";
}

HRESULT HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string HrException::GetErrorDescription() const noexcept
{
	return WindowException::TranslateErrorCode(hr);
}


const char* NoGfxException::GetType() const noexcept
{
	return "Window Exception [No Graphics]";
}
#include "GraphicsException.h"

// Graphics exception stuff
GraphicsHrException::GraphicsHrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file),
	hr(hr)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += ToWide(m);
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

const wchar_t* GraphicsHrException::whatW() const noexcept
{
	std::wostringstream oss;
	oss << GetType() << std::endl
		<< L"[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << L" (" << (unsigned long)GetErrorCode() << L")" << std::endl
		<< L"[Description] " << GetErrorDescription() << std::endl;
	if (!info.empty())
	{
		oss << L"\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const wchar_t* GraphicsHrException::GetType() const noexcept
{
	return L"Graphics Exception";
}

HRESULT GraphicsHrException::GetErrorCode() const noexcept
{
	return hr;
}

std::wstring GraphicsHrException::GetErrorInfo() const noexcept
{
	return info;
}

std::wstring GraphicsHrException::GetErrorDescription() const noexcept
{
	return TranslateErrorCode(hr);
}

const wchar_t* DeviceRemovedException::GetType() const noexcept
{
	return L"Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += ToWide(m);
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

const wchar_t* InfoException::whatW() const noexcept
{
	std::wostringstream oss;
	oss << GetType() << std::endl
		<< L"\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const wchar_t* InfoException::GetType() const noexcept
{
	return L"Graphics Info Exception";
}

std::wstring InfoException::GetErrorInfo() const noexcept
{
	return info;
}

#include "Exception.h"

#include "..\..\Config\SystemConfig.h"
#include "..\Utility\Common.h"

#include <exception>

namespace DiveBomber::DEException
{
	Exception::Exception(int inputLine, const char* inputFile) noexcept
	{
		line = inputLine;
		file = Utility::ToWide(inputFile);
	}

	const wchar_t* Exception::whatW() const noexcept
	{
		std::wostringstream oss;
		oss << GetType() << std::endl
			<< GetOriginString();
		whatBuffer = oss.str();
		return whatBuffer.c_str();
	}

	const wchar_t* Exception::GetType() const noexcept
	{
		return L"Standard Exception";
	}

	int Exception::GetLine() const noexcept
	{
		return line;
	}

	const std::wstring& Exception::GetFile() const noexcept
	{
		return file;
	}

	std::wstring Exception::GetOriginString() const noexcept
	{
		std::wostringstream oss;
		oss << "[File] " << file << std::endl
			<< "[Line] " << line;
		return oss.str();
	}

	std::wstring Exception::TranslateErrorCode(HRESULT hr) noexcept
	{
		wchar_t* pMsgBuf = nullptr;
		// windows will allocate memory for err string and make our pointer point to it
		const DWORD nMsgLen = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, hr, MAKELANGID(ThrowLanguage, SUBLANG_DEFAULT),
			reinterpret_cast<LPWSTR>(&pMsgBuf), 0, nullptr
		);
		// 0 string length returned indicates a failure
		if (nMsgLen == 0)
		{
			return L"Unidentified error code";
		}
		// copy error string from windows-allocated buffer to std::string
		std::wstring errorString = pMsgBuf;
		// free windows buffer
		LocalFree(pMsgBuf);
		return errorString;
	}
}
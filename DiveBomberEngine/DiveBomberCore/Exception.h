#pragma once
#include "Common.h"
#include "SystemConfig.h"
#include <exception>
#include <string>
#include <sstream>

class Exception : public std::exception
{
public:
	Exception(int inputLine, const char* inputFile) noexcept;
	virtual const wchar_t* whatW() const noexcept;
	virtual const wchar_t* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::wstring& GetFile() const noexcept;
	std::wstring GetOriginString() const noexcept;
	static std::wstring TranslateErrorCode(HRESULT hr) noexcept;
private:
	int line;
	std::wstring file;
protected:
	mutable std::wstring whatBuffer;
};


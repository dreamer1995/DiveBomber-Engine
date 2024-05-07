#pragma once
#include <string>
#include "..\Utility\DEGUID.h"

namespace DiveBomber
{
//Common define, should has specefic macro name
#ifdef _DEBUG
	#define noxnd noexcept(false)
#else
	#define noxnd noexcept(true)
#endif
}

namespace DiveBomber::Utility
{
	[[nodiscard]] std::wstring ToWide(const std::string& narrow);
	[[nodiscard]] std::string ToNarrow(const std::wstring& wide);
	[[nodiscard]] const wchar_t* ToWide(const char* narrow);

	//LPCSTR* ToNarrow1(const LPCWSTR* wide);

	// https://stackoverflow.com/questions/215963/how-do-you-properly-use-widechartomultibyte
	[[nodiscard]] std::string utf8_encode(const std::wstring& wstr);
	[[nodiscard]] std::wstring utf8_decode(const std::string& str);

	[[nodiscard]] std::wstring GUIDToWString(const GUID& guid);
	[[nodiscard]] GUID WStringToGUID(const std::wstring& wString);
}
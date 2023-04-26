#pragma once
#include <string>

namespace DiveBomber
{
//Common define, should has specefic macro name
#ifdef _DEBUG
	#define noxnd noexcept(false)
#else
	#define noxnd noexcept(true)
#endif

#define PI 3.1415926535897932384626433832795f
}

namespace DiveBomber::Utility
{
	std::wstring ToWide(const std::string& narrow);

	std::string ToNarrow(const std::wstring& wide);

	const wchar_t* ToWide(const char* narrow);

	//LPCSTR* ToNarrow1(const LPCWSTR* wide);
}
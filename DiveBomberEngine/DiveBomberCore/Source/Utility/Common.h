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

#define ProjectDirectory "..\\..\\DiveBomberCore\\"
#define WProjectDirectory L"..\\..\\DiveBomberCore\\"

#ifdef _DEBUG
	#define OutputDirectory "..\\..\\x64\\Debug\\"
	#define WOutputDirectory L"..\\..\\x64\\Debug\\"
#endif // _DEBUG

#ifdef NDEBUG
	#define OutputDirectory "..\\..\\x64\\Release\\"
	#define WOutputDirectory L"..\\..\\x64\\Release\\"
#endif // NDEBUG
}

namespace DiveBomber::Utility
{
	[[nodiscard]] std::wstring ToWide(const std::string& narrow);

	[[nodiscard]] std::string ToNarrow(const std::wstring& wide);

	[[nodiscard]] const wchar_t* ToWide(const char* narrow);

	//LPCSTR* ToNarrow1(const LPCWSTR* wide);
}
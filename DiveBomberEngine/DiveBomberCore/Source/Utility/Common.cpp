#include "Common.h"

#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <sstream>
#include <iomanip>
//#include <atlbase.h>

namespace DiveBomber::Utility
{
	std::wstring ToWide(const std::string& narrow)
	{
		wchar_t wide[512];
		mbstowcs_s(nullptr, wide, narrow.c_str(), _TRUNCATE);
		return wide;
	}

	std::string ToNarrow(const std::wstring& wide)
	{
		char narrow[512] = "";
		wcstombs_s(nullptr, narrow, wide.c_str(), _TRUNCATE);
		return narrow;
	}

	const wchar_t* ToWide(const char* narrow)
	{
		const size_t size = strlen(narrow) + 1;
		wchar_t* wide = new wchar_t[size];
		size_t outSize;
		mbstowcs_s(&outSize, wide, size, narrow, size - 1);

		return wide;
	}

	//LPCSTR* ToNarrow1(const LPCWSTR* wide)
	//{
	//	USES_CONVERSION;
	//	LPCSTR* narrow = W2A(*wide);
	//	return &narrow;
	//}
}
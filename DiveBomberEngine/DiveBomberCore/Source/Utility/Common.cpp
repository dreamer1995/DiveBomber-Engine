#include "Common.h"


#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <sstream>
#include <iomanip>
//#include <atlbase.h>
#include <windows.h>

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

	// Convert a wide Unicode string to an UTF8 string
	std::string utf8_encode(const std::wstring& wstr)
	{
		if (wstr.empty()) return std::string();
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}

	// Convert an UTF8 string to a wide Unicode String
	std::wstring utf8_decode(const std::string& str)
	{
		if (str.empty()) return std::wstring();
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

	std::wstring GUIDToWString(const GUID& guid)
	{
		wchar_t guidStringArray[40];
		if (StringFromGUID2(guid, guidStringArray, 40) == 0)
		{
			throw std::exception("Failed to convert GUID to WString");
		}

		return std::move(std::wstring(guidStringArray));
	}

	GUID WStringToGUID(const std::wstring& wString)
	{
		GUID guid;
		if (CLSIDFromString(wString.c_str(), (LPCLSID)&guid) != NOERROR)
		{
			throw std::exception("Failed to convert WString to GUID");
		}
		
		return guid;
	}
}
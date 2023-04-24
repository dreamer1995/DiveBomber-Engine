#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#include "Windows.h"

#ifdef _DEBUG
	#define noxnd noexcept(false)
#else
	#define noxnd noexcept(true)
#endif

std::wstring ToWide(const std::string& narrow);

std::string ToNarrow(const std::wstring& wide);

const wchar_t* ToWide(const char* narrow);

//LPCSTR* ToNarrow1(const LPCWSTR* wide);
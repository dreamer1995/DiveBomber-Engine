#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <Shlwapi.h>

#include "Windows.h"

#if defined(DEBUG) || defined(_DEBUG)
#define DEBUGMODE
#endif

#define noxnd noexcept(!IS_DEBUG)

std::wstring ToWide(const std::string& narrow);

std::string ToNarrow(const std::wstring& wide);

const wchar_t* ToWide(const char* narrow);

//LPCSTR* ToNarrow1(const LPCWSTR* wide);
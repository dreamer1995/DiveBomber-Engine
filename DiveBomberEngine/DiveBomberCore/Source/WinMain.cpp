#include "DiveBomberCore.h"

#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

//Should we need a main loop in core instead of in winmain?
using namespace DiveBomber;
using namespace DEException;

void ReportLiveObjects()
{
	IDXGIDebug1* dxgiDebug;
	DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug));

	dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_IGNORE_INTERNAL);
	dxgiDebug->Release();
}

int CALLBACK wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	// Set the working directory to the path of the executable.
	WCHAR path[MAX_PATH];
	HMODULE hModule = GetModuleHandleW(nullptr);
	if (GetModuleFileNameW(hModule, path, MAX_PATH) > 0)
	{
		PathRemoveFileSpecW(path);
		SetCurrentDirectoryW(path);
	}

	try
	{
		return DiveBomberCore{}.GameLoop();
	}
	catch (const Exception& e)
	{
		MessageBox(nullptr, e.whatW(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, Utility::ToWide(e.what()), L"Buildin Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, L"No details available", L"Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}

	atexit(&ReportLiveObjects);

	return -1;
}
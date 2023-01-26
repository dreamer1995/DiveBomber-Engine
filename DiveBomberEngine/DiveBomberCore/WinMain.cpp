#include "DiveBomberCore.h"
#include "Common.h"

void ReportLiveObjects()
{
	IDXGIDebug1* dxgiDebug;
	DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug));

	dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_IGNORE_INTERNAL);
	dxgiDebug->Release();
}

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
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
		MessageBox(nullptr, ToWide(e.what()), L"Buildin Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, L"No details available", L"Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}

	atexit(&ReportLiveObjects);

	return -1;
}
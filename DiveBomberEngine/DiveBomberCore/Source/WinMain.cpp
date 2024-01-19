#include "DiveBomberCore.h"

#include "Graphics/DEWrl.h"
#include "Exception/Exception.h"
#include "Utility/Common.h"

#include <dxgi1_6.h>
#include <dxgidebug.h>

#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

//Should we need a main loop in core instead of in winmain?
using namespace DiveBomber;
using namespace DEException;

void ReportLiveObjects()
{
	Microsoft::WRL::ComPtr<IDXGIDebug1> dxgiDebug;
	DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug));

	dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
}

void ClearMessageQueue()
{
	MSG msg = { 0 };
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
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
		int returnValue = DiveBomberCore::GetInstance().GameLoop();
		DiveBomberCore::Destructor();
		atexit(&ReportLiveObjects);
		return returnValue;
	}
	catch (const Exception& e)
	{
		ClearMessageQueue();
		MessageBox(nullptr, e.whatW(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		ClearMessageQueue();
		MessageBox(nullptr, Utility::ToWide(e.what()), L"Buildin Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		ClearMessageQueue();
		MessageBox(nullptr, L"No details available", L"Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}

	DiveBomberCore::Destructor();

	atexit(&ReportLiveObjects);

	return -1;
}
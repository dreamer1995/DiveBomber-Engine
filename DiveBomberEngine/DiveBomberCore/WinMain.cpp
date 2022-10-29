#include "DiveBomberCore.h"
#include "Common.h"

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{	
	try
	{
		return DiveBomberCore{}.GameLoop();
	}
	catch (const Exception& e)
	{
		MessageBox(NULL, e.whatW(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(NULL, ToWide(e.what()), L"Buildin Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(NULL, L"No details available", L"Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}
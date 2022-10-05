#include "DiveBomberCore.h"

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
	return DiveBomberCore{}.GameLoop();
	//try
	//{
	//	return ;
	//}
	//catch (...)
	//{
	//	MessageBox(nullptr, L"No details available", L"Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	//}
	//return -1;
}
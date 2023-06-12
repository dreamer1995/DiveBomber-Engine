#include "Console.h"

#include "..\Window\WindowResource.h"

#include <iostream>

namespace DiveBomber::DEConsole
{
	using namespace DEException;

	//BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
	//{
	//	switch (fdwCtrlType)
	//	{
	//		// Handle the CTRL-C signal.
	//	case CTRL_C_EVENT:
	//		printf("Ctrl-C event\n\n");
	//		Beep(750, 300);
	//		return TRUE;

	//		// CTRL-CLOSE: confirm that the user wants to exit.
	//	case CTRL_CLOSE_EVENT:
	//		Beep(600, 200);
	//		printf("Ctrl-Close event\n\n");
	//		return TRUE;

	//		// Pass other signals to the next handler.
	//	case CTRL_BREAK_EVENT:
	//		Beep(900, 200);
	//		printf("Ctrl-Break event\n\n");
	//		return FALSE;

	//	case CTRL_LOGOFF_EVENT:
	//		Beep(1000, 200);
	//		printf("Ctrl-Logoff event\n\n");
	//		return FALSE;

	//	case CTRL_SHUTDOWN_EVENT:
	//		Beep(750, 500);
	//		printf("Ctrl-Shutdown event\n\n");
	//		return FALSE;

	//	default:
	//		return FALSE;
	//	}
	//}

	Console::Console()
	{
		// Console
		if (AllocConsole() == 0)
		{
			const auto excetion = WND_LAST_EXCEPT();
		}

		FILE* fpstdin = stdin, * fpstdout = stdout, * fpstderr = stderr;

		freopen_s(&fpstdin, "CONIN$", "r", stdin);
		freopen_s(&fpstdout, "CONOUT$", "w", stdout);
		freopen_s(&fpstderr, "CONOUT$", "w", stderr);

		SetConsoleTitle(L"Console");

		//maybe we can do this later, but it's not really important
		//if(SetConsoleCtrlHandler(CtrlHandler, TRUE));
		//	while (1) {}
	}

	Console::~Console()
	{

	}

	void Console::GetInput(std::wstring& command)
	{
		std::getline(std::wcin, command);
	}
}
#include "Console.h"
#include <iostream>

Console::Console()
{
	// Console
	if (AllocConsole() == 0)
	{
		WND_LAST_EXCEPT();
	}

	FILE* fpstdin = stdin, * fpstdout = stdout, * fpstderr = stderr;

	freopen_s(&fpstdin, "CONIN$", "r", stdin);
	freopen_s(&fpstdout, "CONOUT$", "w", stdout);
	freopen_s(&fpstderr, "CONOUT$", "w", stderr);

	SetConsoleTitle(L"Console");
}

Console::~Console()
{
	FreeConsole();
}

void Console::GetInput(std::wstring& command)
{
	std::getline(std::wcin, command);
	if (waitForInput)
	{
		GetInput(command);
	}
}
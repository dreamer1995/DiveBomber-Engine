#pragma once
#include <string>

namespace DiveBomber::DEConsole
{
	class Console final
	{
	public:
		Console();
		~Console();
		Console(const Console&) = delete;
		Console& operator=(const Console&) = delete;
		void GetInput(std::wstring& command);

		bool waitForInput = true;
	};
}
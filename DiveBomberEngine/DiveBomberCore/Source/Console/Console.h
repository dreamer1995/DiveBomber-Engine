#pragma once
#include <string>
#include <atomic>

namespace DiveBomber::DEConsole
{
	class Console final
	{
	public:
		Console();
		~Console();
		Console(const Console&) = delete;
		Console& operator=(const Console&) = delete;
		void GetInput(std::wstring command);
		[[nodiscard]] bool GetWaitForInput() const noexcept;
		void SetWaitForInput(bool inputWaitForInput) noexcept;

	private:
		std::atomic_bool waitForInput = true;
	};
}
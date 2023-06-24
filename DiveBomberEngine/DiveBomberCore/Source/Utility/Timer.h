#pragma once
#include <chrono>

namespace DiveBomber::Utility
{
	class Timer final
	{
	public:
		Timer() noexcept;
		[[nodiscard]] double Mark() noexcept;
		[[nodiscard]] float Peek() const noexcept;
	private:
		std::chrono::steady_clock::time_point last;
	};
}
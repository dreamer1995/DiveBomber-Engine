#pragma once
#include <chrono>

namespace DiveBomber::Utility
{
	class Timer final
	{
	public:
		Timer() noexcept;
		double Mark() noexcept;
		float Peek() const noexcept;
	private:
		std::chrono::steady_clock::time_point last;
	};
}
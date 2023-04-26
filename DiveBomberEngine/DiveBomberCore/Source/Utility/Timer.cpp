#include "Timer.h"

using namespace std::chrono;

namespace DiveBomber::Utility
{
	Timer::Timer() noexcept
	{
		last = steady_clock::now();
	}

	double Timer::Mark() noexcept
	{
		const std::chrono::steady_clock::time_point old = last;
		last = steady_clock::now();
		const duration<double> frameTime = last - old;
		return frameTime.count();
	}

	float Timer::Peek() const noexcept
	{
		return duration<float>(steady_clock::now() - last).count();
	}
}
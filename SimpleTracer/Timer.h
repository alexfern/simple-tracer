#pragma once

#include <chrono>

struct Timer {
	using Clock = std::chrono::high_resolution_clock;
private:
	std::chrono::time_point<Clock> last;
public:
	Timer() :
		last(Clock::now())
	{}

	std::chrono::duration<double> mark();
};

#pragma once

#include <chrono>

struct Timer {
	using Clock = std::chrono::high_resolution_clock;
private:
	std::chrono::time_point<Clock> last;
public:
	Timer();

	std::chrono::duration<double> mark();
};

#include "Timer.h"

std::chrono::duration<double> Timer::mark() {
	auto now = Clock::now();
	auto delta = now - last;
	last = now;
	return delta;
}

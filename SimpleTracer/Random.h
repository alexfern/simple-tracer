#pragma once

#include <random>
#include <functional>

template<typename Type>
inline Type random() {
	static std::uniform_real_distribution<Type> distribution(0.0, 1.0);
	static std::mt19937 generator;
	static std::function<Type()> rand_generator = std::bind(distribution, generator);
	return rand_generator();
}

inline double randomD() {
	return random<double>();
}

inline float randomF() {
	return random<float>();
}
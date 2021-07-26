#pragma once
#include <random>

#define RANDOM_HEADER_WORKING 0

class Random
{
public:
	static void Init();
	static float Float();

private:
#if RANDOM_HEADER_WORKING
	static std::mt19937 s_RandomEngine;
	static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
#endif
};
#pragma once

template<typename T> bool SignBool(T val) { return (val >= (T)0); }

template<typename T> T SignScalar(T val) { return ((val >= (T)0) ? ((T)1) : ((T)(-1))); }

template<typename T> T Abs(T val) { return ((val < 0) ? (-val) : (val)); }

int Round(float val);
int Round(double val);

template<typename T> T Min(T a, T b) { if (a < b) return a; else return b; }
template<typename T> T Max(T a, T b) { if (a > b) return a; else return b; }

#include <vector>
#include <limits>

template<typename T>
T MinOf(std::vector<T> arr) {
	register T min = std::numeric_limits<T>::max();
	for (T elem : arr) { min = Min<T>(elem, min); }
	return min;
}
template<typename T>
T MaxOf(std::vector<T> arr) {
	register T max = std::numeric_limits<T>::min();
	for (T elem : arr) { max = Max<T>(elem, max); }
	return max;
}

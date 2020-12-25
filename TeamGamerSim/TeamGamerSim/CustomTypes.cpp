#include "CustomTypes.h"

template<typename T>
T Clamp(T value, T min, T max)
{
	if (value > max) return max;
	else if (value < min) return min;
	else return value;
}

double Dot(Vec2, Vec2)
{
	return 0.0; // TODO
}

double Dot(Vec3, Vec3)
{
	return 0.0; // TODO
}

POINT operator+(const POINT a, const POINT b) { return { a.x + b.x, a.y + b.y }; }

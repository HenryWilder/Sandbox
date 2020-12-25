#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <thread>

template<typename T>
T Clamp(T value, T min, T max);

struct Vec2
{
	double x, y;

	double Length() const { return sqrt(((x * x) + (y * y))); }
	Vec2 Normal() const { return { x / Length(), y / Length() }; }
};
double Dot(Vec2, Vec2); // TODO

struct Vec3
{
	double x, y, z;

	double Length() const { return sqrt(((x * x) + (y * y) + (z * z))); }
	Vec3 Normal() const { return {x / Length(), y / Length(), z / Length()}; }
};
double Dot(Vec3, Vec3); // TODO

struct Color
{
	unsigned char r, g, b;

	operator COLORREF() const { return RGB(r, g, b); }
	operator Vec3() const { return { (double)r / 255.0, (double)g / 255.0, (double)b / 255.0 }; }
};

POINT operator+(const POINT a, const POINT b);

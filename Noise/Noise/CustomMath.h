#pragma once
#include <cmath>

typedef unsigned int Whole;

// A 2D screenspace position
struct PixelPos
{
	PixelPos() : x{ 0U }, y{ 0U } {};

	PixelPos(double _x, double _y) : x{ (Whole)_x }, y{ (Whole)_y } {};

	PixelPos(Whole _x, Whole _y) : x{ _x }, y{ _y } {};

	Whole x, y;
};

// A 2D abstract position with special math
struct AbstrPos
{
	AbstrPos() : x{ 0.0 }, y{ 0.0 } {};

	AbstrPos(double _x, double _y) : x{ _x }, y{ _y } {};

	AbstrPos(Whole _x, Whole _y) : x{ double(_x) }, y{ double(_y) } {};

	AbstrPos(PixelPos _pos) : x{ double(_pos.x) }, y{ double(_pos.y) } {};

	double x, y;

	AbstrPos operator+(const AbstrPos& p2);

	AbstrPos operator-(const AbstrPos& p2);

	AbstrPos operator*(const AbstrPos& p2);

	AbstrPos operator/(const AbstrPos& p2);

	AbstrPos operator*(const double scale);

	AbstrPos operator/(const double scale);

	AbstrPos Normal();

	double Sum();
};

double Distance(AbstrPos& a, AbstrPos& b);

double Dot(AbstrPos a, AbstrPos b);

// A 2D rectangle
struct Rect
{
	Rect() : offset{}, width { 0U }, height{ 0U } {};

	Rect(double _width, double _height) : offset{}, width { (Whole)_width }, height{ (Whole)_height } {};
	Rect(AbstrPos _offset, double _width, double _height) : offset{ PixelPos((Whole)_offset.x, (Whole)_offset.y) }, width{ (Whole)_width }, height{ (Whole)_height } {};

	Rect(Whole _width, Whole _height) : width{ _width }, height{ _height } {};
	Rect(PixelPos _offset, Whole _width, Whole _height) : offset{ _offset }, width{ _width }, height{ _height } {};

	PixelPos offset; // The top-left corner of the rectangle

	Whole width, height;
};
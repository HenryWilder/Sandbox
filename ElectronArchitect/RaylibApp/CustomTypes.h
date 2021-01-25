#pragma once
#include "Globals.h"

struct Int2
{
	int x, y;

	Int2 operator+(Int2 b)
	{
		return { x + b.x, y + b.y };
	}
	Int2 operator-(Int2 b)
	{
		return { x - b.x, y - b.y };
	}
	Int2 operator*(Int2 b)
	{
		return { x * b.x, y * b.y };
	}
	Int2 operator/(Int2 b)
	{
		return { x / b.x, y / b.y };
	}

	operator Vector2()
	{
		return Vector2{ (float)x, (float)y };
	}
};
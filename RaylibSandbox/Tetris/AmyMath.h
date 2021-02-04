#pragma once
#include <raylib.h>
Vector2 operator+(Vector2 a, Vector2 b)
{
	return { a.x + b.x, a.y + b.y };
}
Vector2 operator-(Vector2 a, Vector2 b)
{
	return { a.x - b.x, a.y - b.y };
}
Vector2 operator*(Vector2 a, Vector2 b)
{
	return { a.x * b.x, a.y * b.y };
}
Vector2 operator*(Vector2 a, float scale)
{
	return { a.x * scale, a.y * scale };
}
Vector2 operator/(Vector2 a, Vector2 b)
{
	return { a.x / b.x, a.y / b.y };
}
Vector2 operator/(Vector2 a, float scale)
{
	return { a.x / scale, a.y / scale };
}
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
	Int2 operator*(int scale)
	{
		return { x * scale, y * scale };
	}
	Int2 operator/(Int2 b)
	{
		return { x / b.x, y / b.y };
	}
	Int2 operator/(int scale)
	{
		return { x / scale, y / scale };
	}
};
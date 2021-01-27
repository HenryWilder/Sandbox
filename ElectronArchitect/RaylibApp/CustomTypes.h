#pragma once
#include "BasicMath.h"

struct Vector2;
extern const int g_gridSize;

struct Int2
{
	Int2() : x(), y() {};
	Int2(int scale) : x(scale), y(scale) {};
	Int2(int _x, int _y) : x(_x), y(_y) {};
	Int2(Vector2 vec);
	Int2(const Int2& i2) : x(i2.x), y(i2.y) {};

	int x, y;

	Int2& operator=(const Int2& i2) {
		x = i2.x;
		y = i2.y;
		return *this;
	}
	
	Int2 operator+(const Int2 b) const {
		return { x + b.x, y + b.y };
	}
	Int2& operator++(void) {
		x++;
		y++;
		return *this;
	}
	Int2& operator+=(const Int2 b) {
		x += b.x;
		y += b.y;
		return *this;
	}
	Int2 operator-(const Int2 b) const {
		return { x - b.x, y - b.y };
	}
	Int2& operator--(void) {
		x--;
		y--;
		return *this;
	}
	Int2& operator-=(const Int2 b) {
		x -= b.x;
		y -= b.y;
		return *this;
	}
	Int2 operator*(const Int2 b) const {
		return { x * b.x, y * b.y };
	}
	Int2& operator*=(const Int2 b) {
		x *= b.x;
		y *= b.y;
		return *this;
	}
	Int2 operator/(const Int2 b) const {
		return { x / b.x, y / b.y };
	}
	Int2& operator/=(const Int2 b) {
		x /= b.x;
		y /= b.y;
		return *this;
	}
	Int2 operator*(const int scale) const {
		return { x * scale, y * scale };
	}
	Int2& operator*=(const int scale) {
		x *= scale;
		y *= scale;
		return *this;
	}
	Int2 operator/(const int scale) const {
		return { x / scale, y / scale };
	}
	Int2& operator/=(const int scale) {
		x /= scale;
		y /= scale;
		return *this;
	}
	Int2 operator*(const float scale) const {
		return { Round((float)x * scale), Round((float)y * scale) };
	}
	Int2& operator*=(const float scale) {
		Int2 scaled = (*this * scale);
		x = scaled.x;
		y = scaled.y;
		return *this;
	}
	Int2 operator/(const float scale) const {
		return { Round((float)x / scale), Round((float)y / scale) };
	}
	Int2& operator/=(const float scale) {
		Int2 scaled = (*this / scale);
		x = scaled.x;
		y = scaled.y;
		return *this;
	}
	Int2 operator%(const Int2 grid) const {
		return { x % grid.x, y % grid.y };
	}
	Int2& operator%=(const Int2 grid) {
		x %= grid.x;
		y %= grid.y;
		return *this;
	}
	Int2 operator%(const int grid) const {
		return { x % grid, y % grid };
	}
	Int2& operator%=(const int grid) {
		x %= grid;
		y %= grid;
		return *this;
	}

	bool operator==(const Int2 b) const {
		return (x == b.x && y == b.y);
	}
	bool operator!=(const Int2 b) const {
		return (x != b.x || y != b.y);
	}
	bool operator<(const Int2 b) const {
		return (x < b.x && y < b.y);
	}
	bool operator<=(const Int2 b) const {
		return (x <= b.x && y <= b.y);
	}
	bool operator>(const Int2 b) const {
		return (x > b.x && y > b.y);
	}
	bool operator>=(const Int2 b) const {
		return (x >= b.x && y >= b.y);
	}

	explicit operator Vector2() const;
};

Int2 operator-(const Int2 i2);

template<> Int2 Min<Int2>(Int2 a, Int2 b);
template<> Int2 Max<Int2>(Int2 a, Int2 b);

Int2 VecSpace(Int2 i2);

Vector2 IntSpace(Vector2 vec);

struct Rectangle;

struct IntRect
{
	IntRect() : m_cornerMin(), m_cornerMax() {};
	IntRect(Int2 a, Int2 b) : m_cornerMin(Min(a,b)), m_cornerMax(Max(a,b)) {};
	IntRect(const IntRect& ir2) : m_cornerMin(ir2.m_cornerMin), m_cornerMax(ir2.m_cornerMax) {};

	Int2 m_cornerMin;
	Int2 m_cornerMax;

	Int2 Range() const;
	bool Contains(const Int2 pt) const;
	void GrowBorder(const int amount); // Grows the border in all directions by the amount
	void Move(const Int2 offset);

	explicit operator Rectangle() const;
};

#include <raylib.h>
#include "CustomTypes.h"

Int2::Int2(Vector2 vec) : x(Round(vec.x)), y(Round(vec.y)) {}

Int2::operator Vector2() const {
	return { (float)x, (float)y };
}

Int2 VecSpace(Int2 i2)
{
	return i2 * g_gridSize;
}

Vector2 IntSpace(Vector2 vec)
{
	Int2 i2 = (Int2)(vec);
	return (Vector2)(i2 * g_gridSize);
}

Int2 IntRect::Range() const
{
	return (m_cornerMax - m_cornerMin);
}

bool IntRect::Contains(const Int2 pt) const
{
	return (m_cornerMin <= pt && pt <= m_cornerMax);
}

void IntRect::GrowBorder(const int amount)
{ 
	m_cornerMin -= amount;
	m_cornerMax += amount;
}

void IntRect::Move(const Int2 offset)
{
	m_cornerMin += offset;
	m_cornerMax += offset;
}

IntRect::operator Rectangle() const {
	Int2 range = Range();
	return Rectangle{ (float)m_cornerMin.x, (float)m_cornerMin.y, (float)range.x, (float)range.y };
}

Int2 operator-(const Int2 i2)
{
	return Int2(-(i2.x), -(i2.y));
}

template<> Int2 Min<Int2>(Int2 a, Int2 b) { return Int2(Min(a.x, b.x), Min(a.y, b.y)); }
template<> Int2 Max<Int2>(Int2 a, Int2 b) { return Int2(Max(a.x, b.x), Max(a.y, b.y)); }

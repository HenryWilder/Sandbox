#include "ivec.h"

IVec2 IVec2Zero()
{
	static const IVec2 null(0,0);
	return null;
}

IVec2 operator+(IVec2 a, IVec2 b)
{
	return IVec2(a.x + b.x, a.y + b.y);
}
IVec2 operator-(IVec2 a, IVec2 b)
{
	return IVec2(a.x - b.x, a.y - b.y);
}
IVec2 operator*(IVec2 a, IVec2 b)
{
	return IVec2(a.x * b.x, a.y * b.y);
}
IVec2 operator/(IVec2 a, IVec2 b)
{
	return IVec2(a.x / b.x, a.y / b.y);
}

IVec2 IVec2Scale_i(IVec2 a, int b)
{
	return IVec2(a.x * b, a.y * b);
}
IVec2 IVec2Scale_i(IVec2 a, int b)
{
	return IVec2(a.x * b, a.y * b);
}

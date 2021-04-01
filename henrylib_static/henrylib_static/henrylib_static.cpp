// henrylib_static.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"

// TODO: This is an example of a library function
void fnhenrylibstatic()
{
}

Int2 operator+(Int2 pt1, Int2 pt2)
{
	return { pt1.x + pt2.x, pt1.y + pt2.y };
}
Int2 operator-(Int2 pt1, Int2 pt2)
{
	return { pt1.x - pt2.x, pt1.y - pt2.y };
}
Int2 operator*(Int2 pt1, Int2 pt2)
{
	return Int2{ pt1.x * pt2.x, pt1.y * pt2.y };
}
Int2 operator/(Int2 pt1, Int2 pt2)
{
	return Int2{ pt1.x / pt2.x, pt1.y / pt2.y };
}

Int2 operator*(Int2 pt1, int scale)
{
	return {};
}

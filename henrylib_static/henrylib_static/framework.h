#pragma once

//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

struct Int2
{
	int x, y;
};
Int2 operator+(Int2 pt1, Int2 pt2);
Int2 operator-(Int2 pt1, Int2 pt2);
Int2 operator*(Int2 pt1, Int2 pt2);
Int2 operator/(Int2 pt1, Int2 pt2);

Int2 operator*(Int2 pt1, int scale);
Int2 operator/(Int2 pt1, int scale);

Int2 operator*(Int2 pt1, float scale);
Int2 operator/(Int2 pt1, float scale);

bool Between(Int2 pt, Int2 a, Int2 b);
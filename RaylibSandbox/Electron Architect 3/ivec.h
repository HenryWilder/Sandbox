#pragma once

using IVecInt_t = int;

struct IVec2
{
	IVec2(IVecInt_t x, IVecInt_t y) : x(x), y(y) {}

	IVecInt_t x;
	IVecInt_t y;
};

IVec2 IVec2Zero();
IVec2 operator+(IVec2 a, IVec2 b);
IVec2 operator-(IVec2 a, IVec2 b);
IVec2 operator*(IVec2 a, IVec2 b);
IVec2 operator/(IVec2 a, IVec2 b);

IVec2 IVec2Scale_i(IVec2 a, int b);
IVec2 IVec2Scale_i(int a, IVec2 b);
IVec2 IVec2Scale_f(IVec2 a, float b);
IVec2 IVec2Scale_f(float a, IVec2 b);

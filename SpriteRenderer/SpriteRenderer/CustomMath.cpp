#include "CustomMath.h"

AbstrPos AbstrPos::operator+(const AbstrPos& p2)
{
	AbstrPos out;
	out.x = x + p2.x;
	out.y = y + p2.y;
	return out;
}

AbstrPos AbstrPos::operator-(const AbstrPos& p2)
{
	AbstrPos out;
	out.x = x - p2.x;
	out.y = y - p2.y;
	return out;
}

AbstrPos AbstrPos::operator*(const AbstrPos& p2)
{
	AbstrPos out;
	out.x = x * p2.x;
	out.y = y * p2.y;
	return out;
}

AbstrPos AbstrPos::operator/(const AbstrPos& p2)
{
	AbstrPos out;
	out.x = x / p2.x;
	out.y = y / p2.y;
	return out;
}

AbstrPos AbstrPos::operator*(const double scale)
{
	AbstrPos out;
	out.x = x * scale;
	out.y = y * scale;
	return out;
}

AbstrPos AbstrPos::operator/(const double scale)
{
	AbstrPos out;
	out.x = x / scale;
	out.y = y / scale;
	return out;
}

AbstrPos AbstrPos::Normal()
{
	AbstrPos out = *this;
	out / sqrt((out.x * out.x) + (out.y * out.y));
	return out;
}

double AbstrPos::Sum()
{
	return (x + y);
}

double Distance(AbstrPos& a, AbstrPos& b)
{
	AbstrPos offset;

	offset = b - a;

	offset = offset * offset;

	double d = offset.Sum();

	d = sqrt(d);

	return d;
}

double Dot(AbstrPos a, AbstrPos b)
{
	AbstrPos ab = (a * b);

	double sum = ab.x + ab.y;

	return sum;
}

Whole Rect::Area()
{
	return width * height;
}

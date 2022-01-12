#include <iostream>
#include <math.h>

constexpr double pi = 3.141592653589793238462614;

struct Value
{
	Value() = default;

	double n;
	bool b_radical;

	bool operator==(double n1) const
	{
		return (b_radical ? sqrt(n) : n) == n1;
	}
	bool operator!=(double n1) const
	{
		return (b_radical ? sqrt(n) : n) != n1;
	}
	Value operator*(double n1) const
	{
		if (b_radical && sqrt(n) == n1)
			return { n, false };
		else
			return { n * n1, false };
	}


	Value(double _n)
	{
		n = _n;
		b_radical = false;
	}
	Value(double _n, bool _radical)
	{
		n = _n;
		b_radical = _radical;
	}
	Value(double val)
	{
		n = val;
		b_radical = false;
	}
};
std::ostream& operator<<(std::ostream& stream, Value value)
{
	if (value.b_radical)
		return stream << "sqrt(" << value.n << ")";
	else
		return stream << value.n;
}

struct RadianMeasure
{
	RadianMeasure() = default;

	Value top, bottom;
	bool b_xPi;

	RadianMeasure(const char* equ)
	{
		top = bottom = Value(0.0);
		b_xPi = false;
		bool side = false;
		for (const char* d = equ; *d; ++d)
		{
			if (side)
			{
				bottom.n = (bottom.n * 10.0) + (double)(*d - '0');
			}
			else
			{
				if (*d == '/')
				{
					side = true;
				}
				else if (*d == 'p' && *(++d) == 'i')
				{
					b_xPi = true;
				}
				else
				{
					top.n = (top.n * 10.0) + (double)(*d - '0');
				}
			}
		}
	}
};
std::ostream& operator<<(std::ostream& stream, RadianMeasure measure)
{
	if (measure.top != 1.0)
		stream << measure.top;

	return stream << measure.top << (measure.b_xPi ? "pi/" : "/") << measure.bottom;
}


int main()
{
	RadianMeasure r = "2pi/4";
	std::cout << r;
	return 0;
}
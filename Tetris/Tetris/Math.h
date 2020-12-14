#pragma once

typedef unsigned int Whole; // A positive integer including 0
typedef unsigned char Byte; // A single byte (0-255)

namespace ext
{
	struct Coord
	{
		// Creates a coordinate from the input x and y values
		Coord(int inX, int inY);
		// Creates a coordinate at the origin
		Coord();

		int x, y;

		// Coord math
		Coord operator+(const Coord& b)
		{
			Coord out;
			out.x = x + b.x;
			out.y = y + b.y;
			return out;
		}
		Coord operator-(const Coord& b)
		{
			Coord out;
			out.x = x - b.x;
			out.y = y - b.y;
			return out;
		}
		Coord operator*(const Coord& b)
		{
			Coord out;
			out.x = x * b.x;
			out.y = y * b.y;
			return out;
		}
		Coord operator/(const Coord& b)
		{
			Coord out;
			out.x = x / b.x;
			out.y = y / b.y;
			return out;
		}

		// Coord comparing
		bool operator<(const Coord& b)
		{
			return (x < b.x) && (y < b.y);
		}
		bool operator>(const Coord& b)
		{
			return (x > b.x) && (y > b.y);
		}
		bool operator<=(const Coord& b)
		{
			return (x <= b.x) && (y <= b.y);
		}
		bool operator>=(const Coord& b)
		{
			return (x >= b.x) && (y >= b.y);
		}

		// Returns the index of a position in an array
		int Index(Whole width);
		// Returns the index of a position in an array with offset
		int Index(Whole width, Coord offset);
	};

	struct Range
	{
		Range(int minIn, int maxIn);
		// If only one input is supplied, it will be assumed that input is the max and the min is 0
		Range(Whole sizeIn);
		// The default range is a Whole number 0 to a Whole number 1
		Range();

		int min;
		int max;

		// Returns the distance between the min and max values
		Whole Size();

		// Returns the midpoint/average of the two values
		int Mean();

		// Linearly interpolate between the min and max values (higher values are closer to max)
		int Lerp(float weight = .5);

		// Returns a bool for whether the input is inside of the range (inclusive)
		bool Has(int val);
	};

	struct Rect
	{
		// Creates a rectangle from a pair of coordinates (min and max corners)
		Rect(Coord coordMin, Coord coordMax);
		// Creates a rectangle from a pair of ranges
		Rect(Range xRangeIn, Range yRangeIn);
		// Creates a rectangle from the width and height
		// (Optionally add a coord for offset)
		Rect(Whole xSizeIn, Whole ySizeIn, Coord offset = Coord(0,0));
		// Creates a rectangle from the center point & extents
		// @TODO not sure if this is useful but it's still helpful to know how to make it
		Rect(Coord position, Whole halfWidth, Whole halfHeight);
		// Creates a rectangle from a pair of ranges, with Range() implied
		Rect(Whole xMinIn, Whole xMaxIn, Whole yMinIn, Whole yMaxIn);
		// Creates a square with area 1 with no offset
		Rect();
		
		Range xRange;
		Range yRange;

		bool Has(Coord pos);

		Whole Index(Coord pos);

		Coord Corner(bool x, bool y);
		// Returns the quadrant from the signs inputted
		// 0 = negative; 1 = positive
		Rect GetQuadrant(bool xSign, bool ySign);
	};
}

signed int Sign(signed int val);
signed int Sign(signed short val);
signed int Sign(signed long val);
signed int Sign(signed long long val);
signed int Sign(float val);
signed int Sign(double val);
signed int Sign(signed char val);

bool BoolSign(signed int input);
bool BoolSign(signed short input);
bool BoolSign(signed long input);
bool BoolSign(signed long long input);
bool BoolSign(float input);
bool BoolSign(double input);
bool BoolSign(signed char input);
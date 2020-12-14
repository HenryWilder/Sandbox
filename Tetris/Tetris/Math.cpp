#include "Math.h"

namespace ext
{
	// Creates a coordinate from the input x and y values

	Coord::Coord(int inX, int inY)
	{
		x = inX;
		y = inY;
	}

	// Creates a coordinate at the origin

	Coord::Coord()
	{
		x = 0;
		y = 0;
	}

	// Returns the index of a position in an array

	int Coord::Index(Whole width)
	{
		return (y * width) + x;
	}

	int Coord::Index(Whole width, Coord offset)
	{
		Coord pos;
		pos = Coord(x, y) - offset;
		Whole out = pos.Index(width);
		return out;
	}


	Range::Range(int minIn, int maxIn)
	{
		// Validate that the min is in fact smaller than the max
		min = (
			minIn < maxIn ?
			minIn : maxIn);
		// Validate that the max is in fact larger than the min
		max = (
			maxIn > minIn ?
			maxIn : minIn);
	}

	// If only one input is supplied, it will be assumed that input is the max and the min is 0

	Range::Range(Whole sizeIn)
	{
		min = 0;
		max = sizeIn;
	}

	// The default range is a Whole number 0 to a Whole number 1

	Range::Range()
	{
		min = 0;
		max = 1;
	}

	// Returns the distance between the min and max values

	Whole Range::Size()
	{
		return max - min;
	}

	// Returns the midpoint/average of the two values

	int Range::Mean()
	{
		return (min + max) / 2;
	}

	// Linearly interpolate between the min and max values (higher values are closer to max)

	int Range::Lerp(float weight)
	{
		return int(((1 - weight) * float(min)) + (weight * float(max)));
	}

	// Returns a bool for whether the input is inside of the range (inclusive)

	bool Range::Has(int val)
	{
		return
			(val >= min) &&
			(val <= max);
	}

	Rect::Rect(Coord coordMin, Coord coordMax)
	{
		xRange = Range(
			coordMin.x,
			coordMax.x);
		yRange = Range(
			coordMin.y,
			coordMax.y);
	}

	Rect::Rect(Range xRangeIn, Range yRangeIn)
	{
		xRange = xRangeIn;
		yRange = yRangeIn;
	}

	Rect::Rect(Whole xSizeIn, Whole ySizeIn, Coord offset)
	{
		xRange = xSizeIn + offset.x;
		yRange = ySizeIn + offset.y;
	}

	Rect::Rect(Coord position, Whole halfWidth, Whole halfHeight)
	{
		xRange = Range(position.x - halfWidth, position.x + halfWidth);
		yRange = Range(position.y - halfHeight, position.y + halfHeight);
	}

	Rect::Rect(Whole xMinIn, Whole xMaxIn, Whole yMinIn, Whole yMaxIn)
	{
		xRange = Range(xMinIn, xMaxIn);
		yRange = Range(yMinIn, yMaxIn);
	}

	Rect::Rect()
	{
		xRange = Range(0, 1);
		yRange = Range(0, 1);
	}

	bool Rect::Has(Coord pos)
	{
		return (
			xRange.Has(pos.x) &&
			yRange.Has(pos.y));
	}

	Whole Rect::Index(Coord pos)
	{
		return Whole((pos.y * xRange.Size()) + pos.x);
	}

	Coord Rect::Corner(bool x, bool y)
	{
		return Coord(
			(x ? xRange.max
				: xRange.min),
			(y ? yRange.max
				: yRange.min));
	}

	// Returns the quadrant from the signs inputted
	// 0 = negative; 1 = positive

	Rect Rect::GetQuadrant(bool xSign, bool ySign)
	{
		return Rect(
			// x Range
			Range(
				(xSign ? xRange.Mean() : xRange.min), // Min
				(xSign ? xRange.max : xRange.Mean()) // Max
			),
			// y Range
			Range(
				(ySign ? yRange.Mean() : yRange.min), // Min
				(ySign ? yRange.max : yRange.Mean()) // Max
			));
	}
}

signed int Sign(signed int val)
{
	return (val < 0 ? -1 : 1);
}

signed int Sign(signed short val)
{
	return (val < 0 ? -1 : 1);
}

signed int Sign(signed long val)
{
	return (val < 0 ? -1 : 1);
}

signed int Sign(signed long long val)
{
	return (val < 0 ? -1 : 1);
}

signed int Sign(float val)
{
	return (val < 0 ? -1 : 1);
}

signed int Sign(double val)
{
	return (val < 0 ? -1 : 1);
}

signed int Sign(signed char val)
{
	return (val < 0 ? -1 : 1);
}

bool BoolSign(signed int input)
{
	return (input < 0 ? false : true);
}

bool BoolSign(signed short input)
{
	return BoolSign(Sign(input));
}

bool BoolSign(signed long input)
{
	return BoolSign(Sign(input));
}

bool BoolSign(signed long long input)
{
	return BoolSign(Sign(input));
}

bool BoolSign(float input)
{
	return BoolSign(Sign(input));
}

bool BoolSign(double input)
{
	return BoolSign(Sign(input));
}

bool BoolSign(signed char input)
{
	return BoolSign(Sign(input));
}
#pragma once
#include <cmath>

struct ColorRGB
{
	unsigned char r, g, b;

	ColorRGB operator+(const ColorRGB& col);
	ColorRGB operator-(const ColorRGB& col);
	ColorRGB operator*(const ColorRGB& col);
	ColorRGB operator/(const ColorRGB& col);

	ColorRGB operator*(const double scale);
	ColorRGB operator/(const double scale);
};

struct ColorRGBA
{
	unsigned char r, g, b, alpha;
};

struct Vec2
{
	double x, y;

	// Vector math
	Vec2 operator+(const Vec2& p1);
	Vec2 operator-(const Vec2& p1);
	Vec2 operator*(const Vec2& p1);
	Vec2 operator/(const Vec2& p1);

	// Scalar math
	Vec2 operator*(const double in);
	Vec2 operator/(const double in);

	// Special math
	double Sum(void); // Used in Length & Dot
	double Length(void);
};

double Distance(Vec2& p1, Vec2& p2);

struct Vec3
{
	double x, y, z;

	// Vector math
	Vec3 operator+(const Vec3& p1);
	Vec3 operator-(const Vec3& p1);
	Vec3 operator*(const Vec3& p1);
	Vec3 operator/(const Vec3& p1);

	// Scalar math
	Vec3 operator*(const double scale);
	Vec3 operator/(const double scale);
	
	// Special math
	double Sum(void); // Used in Length & Dot
	double Length(void);
	Vec3 Normal(void);
};

double Distance(Vec3& p1, Vec3& p2);

double Dot(Vec3& p1, Vec3& p2);

template<typename T = double>
struct Range
{
	T min;
	T max;

	bool ContainsIn(T val); // Inclusive
	bool ContainsEx(T val); // Exclusive
	bool ContainsExMin(T val); // Exclusive min, Inclusive max
	bool ContainsExMax(T val); // Inclusive min, Exclusive max
	T Length();
};

/*
template<typename T>
// Created because <vector> doesn't have the functionalities I need for this particular usage
struct DynamicList
{
	DynamicList()
	{
		size = 0;
		elem = nullptr;
	}
	DynamicList(int usableSize, T* inputs)
	{
		size = usableSize;
		delete[] elem;
		elem = inputs;
	}
	~DynamicList()
	{
		delete[] elem;
	}

	int size; // Usable elements
	T* elem;

	T Get(int index);
	void Set(int index, T value); // Overwrites the specified index with the new value
	int Find(T search); // Returns the index of the specified element
	void Append(T value);
	void InsertAt(int index, T value);
	void RemoveIndex(int index);
	void RemoveElem(T search);
	void Clear(); // Sets all elements to 0 or equivalent
	void RemoveElemRange(Range<int> range);
	void RemoveElemSeries(DynamicList<T>& elements); // Finds & removes all of the matching elements
};
*/

// alpha should be between 0 and 1 inclusive (0 <= alpha <= 1)
double Lerp(double a, double b, double alpha = .5);
ColorRGB Lerp(ColorRGB a, ColorRGB b, double alpha = .5);

// alpha should be a vector of the weights of a, b, and c. The x,y,z values should align with the a,b,c counterparts.
double TriLerp(double a, double b, double c, Vec3 distances = {1,1,1});

struct Rect
{
	Range<double> xRange, yRange;

	bool ContainsIn(Vec2 pos); // Inclusive
	bool ContainsEx(Vec2 pos); // Exclusive
	double Width();
	double Height();
	double Area();
	Vec2 CornerTL(); // Top-Left corner
	Vec2 CornerBR(); // Bottom-Right corner
};

Vec2 MyFunction(const Vec2 a, const Vec2 b)
{
	return a + b;
}
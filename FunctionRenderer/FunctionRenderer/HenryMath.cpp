#include "HenryMath.h"

ColorRGB ColorRGB::operator+(const ColorRGB& col)
{
	ColorRGB out;
	out.r = this->r + col.r;
	out.g = this->g + col.g;
	out.b = this->b + col.b;
	return out;
}

ColorRGB ColorRGB::operator-(const ColorRGB& col)
{
	ColorRGB out;
	out.r = this->r - col.r;
	out.g = this->g - col.g;
	out.b = this->b - col.b;
	return out;
}

ColorRGB ColorRGB::operator*(const ColorRGB& col)
{
	ColorRGB out;
	out.r = this->r * col.r;
	out.g = this->g * col.g;
	out.b = this->b * col.b;
	return out;
}

ColorRGB ColorRGB::operator/(const ColorRGB& col)
{
	ColorRGB out;
	out.r = this->r / col.r;
	out.g = this->g / col.g;
	out.b = this->b / col.b;
	return out;
}

ColorRGB ColorRGB::operator*(const double scale)
{
	ColorRGB out;
	out.r = (unsigned char)(double(this->r) * scale);
	out.g = (unsigned char)(double(this->g) * scale);
	out.b = (unsigned char)(double(this->b) * scale);
	return out;
}

ColorRGB ColorRGB::operator/(const double scale)
{
	ColorRGB out;
	out.r = (unsigned char)(double(this->r) / scale);
	out.g = (unsigned char)(double(this->g) / scale);
	out.b = (unsigned char)(double(this->b) / scale);
	return out;
}

Vec2 Vec2::operator+(const Vec2& p1) {
	Vec2 out;
	out.x = this->x + p1.x;
	out.y = this->y + p1.y;
	return out;
}
Vec2 Vec2::operator-(const Vec2& p1) {
	Vec2 out;
	out.x = this->x - p1.x;
	out.y = this->y - p1.y;
	return out;
}
Vec2 Vec2::operator*(const Vec2& p1) {
	Vec2 out;
	out.x = this->x * p1.x;
	out.y = this->y * p1.y;
	return out;
}
Vec2 Vec2::operator/(const Vec2& p1) {
	Vec2 out;
	out.x = this->x / p1.x;
	out.y = this->y / p1.y;
	return out;
}

double Vec2::Sum(void)
{
	return this->x + this->y;
}

double Vec2::Length(void)
{
	// This function uses the pythagorean theorem
	// a² + b² = c²
	Vec2 v = *this; // a, b
	Vec2 square = v * v; // a², b²
	double hypotenuseSquared = square.Sum(); // d² = a² + b² + c²
	return sqrt(hypotenuseSquared); // d = sqrt(d²)
}

Vec2 Vec2::operator*(const double in) {
	Vec2 out;
	out.x = this->x * in;
	out.y = this->y * in;
	return out;
}
Vec2 Vec2::operator/(const double in) {
	Vec2 out;
	out.x = this->x / in;
	out.y = this->y / in;
	return out;
}

Vec3 Vec3::operator+(const Vec3& p1) {
	Vec3 out;
	out.x = this->x + p1.x;
	out.y = this->y + p1.y;
	out.z = this->z + p1.z;
	return out;
}
Vec3 Vec3::operator-(const Vec3& p1) {
	Vec3 out;
	out.x = this->x - p1.x;
	out.y = this->y - p1.y;
	out.z = this->z - p1.z;
	return out;
}
Vec3 Vec3::operator*(const Vec3& p1) {
	Vec3 out;
	out.x = this->x * p1.x;
	out.y = this->y * p1.y;
	out.z = this->z * p1.z;
	return out;
}
Vec3 Vec3::operator/(const Vec3& p1) {
	Vec3 out;
	out.x = this->x / p1.x;
	out.y = this->y / p1.y;
	out.z = this->z / p1.z;
	return out;
}

Vec3 Vec3::operator*(const double scale) {
	Vec3 out;
	out.x = this->x * scale;
	out.y = this->y * scale;
	out.z = this->z * scale;
	return out;
}
Vec3 Vec3::operator/(const double scale) {
	Vec3 out;
	out.x = this->x / scale;
	out.y = this->y / scale;
	out.z = this->z / scale;
	return out;
}

double Vec3::Sum(void)
{
	return (this->x + this->y + this->z); // This is surprisingly useful. Trust me.
}

double Vec3::Length(void)
{
	// This function uses the pythagorean theorem for 3D
	// a² + b² + c² = d²
	Vec3 v = *this; // a, b, c
	Vec3 square = v * v; // a², b², c²
	double hypotenuseSquared = square.Sum(); // d² = (a² + b² + c²)
	return sqrt(hypotenuseSquared); // d = sqrt(d²)
}

Vec3 Vec3::Normal(void)
{
	Vec3 v = *this;
	Vec3 normal = (v / this->Length());
	return normal;
}

double Distance(Vec2& p1, Vec2& p2)
{
	Vec2 difference = p2 - p1;
	double dist = difference.Length();
	return dist;
}

double Distance(Vec3& p1, Vec3& p2)
{
	Vec3 difference = p2 - p1;
	double dist = difference.Length();
	return dist;
}

double Dot(Vec3& p1, Vec3& p2)
{
	Vec3 product = p1 * p2;
	double dot = product.Sum();
	return dot;
}

template<typename T>
bool Range<T>::ContainsIn(T val)
{
	return min <= val && val <= max;
}

template<typename T>
bool Range<T>::ContainsEx(T val)
{
	return min < val && val < max;
}

template<typename T>
bool Range<T>::ContainsExMin(T val)
{
	return min < val && val <= max;
}

template<typename T>
bool Range<T>::ContainsExMax(T val)
{
	return min <= val && val < max;
}

template<typename T>
T Range<T>::Length()
{
	return max - min; // The distance between the min and the max
}


double Lerp(double a, double b, double alpha)
{
	double out;
	out = (a * (1.0 - alpha)) + (b * alpha);
	return out;
}
ColorRGB Lerp(ColorRGB a, ColorRGB b, double alpha)
{
	ColorRGB out;
	out = (a * (1.0 - alpha)) + (b * alpha);
	return out;
}

double TriLerp(double a, double b, double c, Vec3 distances)
{
	Vec3 alpha = distances.Normal();
	double out;
	// Because we have normalized the distances, the weights will sum to 1
	out = (a * alpha.x) + (b * alpha.y) + (c * alpha.z);
	return out;
}

bool Rect::ContainsIn(Vec2 pos)
{
	bool xContainment = xRange.ContainsIn(pos.x);

	bool yContainment = yRange.ContainsIn(pos.y);

	bool out = xContainment && yContainment;

	return out;
}

bool Rect::ContainsEx(Vec2 pos)
{
	bool xContainment = xRange.ContainsEx(pos.x);

	bool yContainment = yRange.ContainsEx(pos.y);

	bool out = xContainment && yContainment;

	return out;
}

double Rect::Width()
{
	return xRange.Length();
}

double Rect::Height()
{
	return yRange.Length();
}

double Rect::Area()
{
	return Width() * Height();
}

Vec2 Rect::CornerTL()
{
	return { xRange.min, yRange.min };
}

Vec2 Rect::CornerBR()
{
	return { xRange.max, yRange.max };
}

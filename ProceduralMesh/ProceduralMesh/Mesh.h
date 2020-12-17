#pragma once
#include <math.h>

template<typename T>
class Vec3Base
{
public:
	constexpr Vec3Base() : a{ 0 }, b{ 0 }, c{ 0 } {};
	constexpr Vec3Base(T _a, T _b, T _c) : a{ _a }, b{ _b }, c{ _c } {};
	constexpr Vec3Base(const Vec3Base&) = default;

	constexpr T const& GetElem0() const { return a; };
	constexpr T const& GetElem1() const { return b; };
	constexpr T const& GetElem2() const { return c; };
	void SetElem0(T const& value) { a = value; };
	void SetElem1(T const& value) { b = value; };
	void SetElem2(T const& value) { c = value; };

protected:
	T a, b, c;
};

class Vector : Vec3Base<double>
{
public:
	Vector() : Vec3Base<double>({ 0.0,0.0,0.0 }) {};
	Vector(const Vec3Base<double>& in) : Vec3Base<double>(in) {};
	Vector(double _x, double _y, double _z) : Vec3Base<double>({ _x,_y,_z }) {};
	Vector(const Vector&) = default;

	constexpr double const& GetX() const { return a; }
	constexpr double const& GetY() const { return b; }
	constexpr double const& GetZ() const { return c; }
	void SetX(double const& value) { a = value; }
	void SetY(double const& value) { b = value; }
	void SetZ(double const& value) { c = value; }

	Vector operator+(const Vector& v2) const;
	Vector operator-(const Vector& v2) const;
	Vector operator*(const Vector& vec) const;
	Vector operator/(const Vector& vec) const;
	Vector operator*(const double& scale) const;
	Vector operator/(const double& scale) const;

	double const& GetLength() const { return sqrt(GetX() * GetX() + GetY() * GetY() + GetZ() * GetZ()); }

	double NrmX() const { return GetX() / GetLength(); }
	double NrmY() const { return GetY() / GetLength(); }
	double NrmZ() const { return GetZ() / GetLength(); }
	Vector const& Normal() { return { (*this) }; }
};

typedef unsigned char Byte;

class Color : Vec3Base<Byte>
{
public:
	constexpr Color() : Vec3Base<Byte>({ 0,0,0 }) {};
	constexpr Color(const Vec3Base<Byte>& in) : Vec3Base<Byte>(in) {};
	constexpr Color(Byte _r, Byte _g, Byte _b) : Vec3Base<Byte>({ _r,_g,_b }) {};
	constexpr Color(const Color&) = default;

	constexpr Byte const& GetR() const { return a; };
	constexpr Byte const& GetG() const { return b; };
	constexpr Byte const& GetB() const { return c; };
	void SetR(Byte const& value) { a = value; };
	void SetG(Byte const& value) { b = value; };
	void SetB(Byte const& value) { c = value; };

	constexpr Color operator+(const Color& col2) const;
	constexpr Color operator-(const Color& col2) const;
	constexpr Color operator*(const double& scale) const;
	constexpr Color operator/(const double& scale) const;
	constexpr Color operator*(const Vector& vec) const;
	constexpr Color operator/(const Vector& vec) const;
};

class Rotation : Vec3Base<double>
{
public:
	Rotation() : Vec3Base<double>({ 0,0,0 }) {};
	Rotation(const Vec3Base<double>& in) : Vec3Base<double>(in) {};
	Rotation(double _roll, double _pitch, double _yaw) : Vec3Base<double>({ _roll ,_pitch,_yaw }) {};
	Rotation(const Rotation&) = default;

	constexpr double const& GetRoll() const { return a; }
	constexpr double const& GetPitch() const { return b; }
	constexpr double const& GetYaw() const { return c; }
	void SetRoll(double const& value) { a = value; }
	void SetPitch(double const& value) { b = value; }
	void SetYaw(double const& value) { c = value; }
};

class Scale : Vec3Base<double>
{
public:
	Scale() : Vec3Base<double>({ 0,0,0 }) {};
	Scale(const Vec3Base<double>& in) : Vec3Base<double>(in) {};
	Scale(double _width, double _height, double _depth) : Vec3Base<double>({ _width,_height,_depth }) {};
	Scale(const Scale&) = default;

	constexpr double const& GetWidth() const { return a; }
	constexpr double const& GetHeight() const { return b; }
	constexpr double const& GetDepth() const { return c; }
	void SetWidth(double const& value) { a = value; }
	void SetHeight(double const& value) { b = value; }
	void SetDepth(double const& value) { c = value; }
};

class Transform
{
private:
	Vector pos;
	Rotation rot;
	Scale scale;

public:
	Transform() : pos({ 0.0,0.0,0.0 }), rot({ 0.0,0.0,0.0 }), scale({ 1.0,1.0,1.0 }) {};
	Transform(const Vector& _pos, const Rotation& _rot, const Scale& _scale) : pos(_pos), rot(_rot), scale(_scale) {};
	Transform(const Transform&) = default;

	constexpr Vector const& GetPosition() const { return pos; }
	constexpr Rotation const& GetRotation() const { return rot; }
	constexpr Scale const& GetScale() const { return scale; }
	void SetPosition(Vector const& _pos) { pos = _pos; }
	void SetRotation(Rotation const& _rot) { rot = _rot; }
	void SetScale(Scale const& _scale) { scale = _scale; }
};

//
//
// START OF MESH TYPES
//
//

// A polygon with 3 verticies
class Triangle
{
public:
	constexpr Triangle(unsigned int _v1, unsigned int _v2, unsigned int _v3) : v{ _v1, _v2, _v3 } {};

	constexpr unsigned int const& GetVert(unsigned int vertex) const
	{
		return v[vertex % 3];
	}

private:
	unsigned int v[3]; // v[i] is an index in an array of verts
};

/*
struct Poly
{
	Poly(size_t _sides, unsigned int* _v, Color _color) : m_sides(_sides), v(_v), color(_color) {};

	size_t m_sides;
	Color color; // The color value
	unsigned int* v; // v is an index in an array of verts

	constexpr size_t const& GetSides() const { return m_sides; }
};

// A polygon with 3 verticies
struct Tri : public Poly
{
	Tri() : Poly(3, v_mem, { 255,255,255 }) {};
	unsigned int v_mem[3];
};

// A polygon with 4 verticies
struct Quad : public Poly
{
	Quad() : Poly(4, v_mem, { 255,255,255 }) {};
	unsigned int v_mem[4];
};

// A polygon with n verticies
// Syntax is "NGon<n> ngonName"
template<size_t n>
struct NGon : public Poly
{
	NGon() : Poly(n, v_mem, { 255,255,255 }) {};
	unsigned int v_mem[n];
};
*/

struct Mesh
{
	Transform m_transform;
	Vector m_vert[8];
	Triangle m_tri[6];

	void DrawTri(int triangleIndex)
	{
		Triangle& tri = m_tri[triangleIndex];
		tri.GetVert(0);
		tri.GetVert(1);
		tri.GetVert(2);
	}
};
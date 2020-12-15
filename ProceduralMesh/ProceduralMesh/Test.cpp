#include <math.h>
x = fmod();

template<typename T>
class Vector3Template
{
protected:
	T a, b, c;
};

typedef unsigned char Byte;

class Color : Vector3Template<unsigned char>
{
public:
	const unsigned char GetR() { return a; }
	const unsigned char GetG() { return b; }
	const unsigned char GetB() { return c; }
	void SetR(unsigned char value) { a = value; }
	void SetG(unsigned char value) { b = value; }
	void SetB(unsigned char value) { c = value; }

	Color operator+(Color col2)
	{
		Color out;
		out.SetR(GetR() + col2.GetR());
		out.SetG(GetG() + col2.GetG());
		out.SetB(GetB() + col2.GetB());
		return out;
	}
	Color operator-(Color col2)
	{
		Color out;
		out.SetR(GetR() - col2.GetR());
		out.SetG(GetG() - col2.GetG());
		out.SetB(GetB() - col2.GetB());
		return out;
	}
};

class Position : Vector3Template<double>
{
public:
	const double GetX() { return a; }
	const double GetY() { return b; }
	const double GetZ() { return c; }
	void SetX(double value) { a = value; }
	void SetY(double value) { b = value; }
	void SetZ(double value) { c = value; }
};

class Rotation : Vector3Template<double>
{
public:
	const double GetRoll()  { return a; }
	const double GetPitch() { return b; }
	const double GetYaw()   { return c; }
	void SetRoll (double value) { a = value; }
	void SetPitch(double value) { b = value; }
	void SetYaw  (double value) { c = value; }
};

class Scale : Vector3Template<double>
{
public:
	const double GetWidth()  { return a; }
	const double GetHeight() { return b; }
	const double GetDepth()  { return c; }
	void SetWidth (double value) { a = value; }
	void SetHeight(double value) { b = value; }
	void SetDepth (double value) { c = value; }
};

class Transform
{
private:
	Position pos;
	Rotation rot;
	Scale scale;

public:
	const Position GetPosition() { return pos;   }
	const Rotation GetRotation() { return rot;   }
	const Scale GetScale()       { return scale; }
	void SetPosition(Position value) {   pos = value; }
	void SetRotation(Rotation value) {   rot = value; }
	void SetScale   (Scale value)    { scale = value; }
};
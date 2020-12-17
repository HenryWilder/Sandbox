#include "Mesh.h"

constexpr Color Color::operator+(const Color& col2)const 
{
	return { (Byte)(this->GetR() + col2.GetR()),
			 (Byte)(this->GetG() + col2.GetG()),
			 (Byte)(this->GetB() + col2.GetB()), };
}

constexpr Color Color::operator-(const Color& col2)const 
{
	return { (Byte)(this->GetR() - col2.GetR()),
			 (Byte)(this->GetG() - col2.GetG()),
			 (Byte)(this->GetB() - col2.GetB()), };
}

constexpr Color Color::operator*(const double& scale) const
{
	return { (Byte)((double)this->GetR() * scale),
			 (Byte)((double)this->GetG() * scale),
			 (Byte)((double)this->GetB() * scale), };
}

constexpr Color Color::operator/(const double& scale) const
{
	return { (Byte)((double)this->GetR() / scale),
			 (Byte)((double)this->GetG() / scale),
			 (Byte)((double)this->GetB() / scale), };
}

constexpr Color Color::operator*(const Vector& vec) const
{
	return { (Byte)((double)this->GetR() * vec.NrmX()),
			 (Byte)((double)this->GetG() * vec.NrmY()),
			 (Byte)((double)this->GetB() * vec.NrmZ()), };
}

constexpr Color Color::operator/(const Vector& vec) const
{
	return { (Byte)((double)this->GetR() / vec.NrmX()),
			 (Byte)((double)this->GetG() / vec.NrmY()),
			 (Byte)((double)this->GetB() / vec.NrmZ()), };
}

Vector Vector::operator+(const Vector& v2) const
{
	return { (double)(this->GetX()) + (v2.GetX()),
			 (double)(this->GetY()) + (v2.GetY()),
			 (double)(this->GetZ()) + (v2.GetZ()), };
}

Vector Vector::operator-(const Vector& v2) const
{
	return { (double)(this->GetX() - v2.GetX()),
			 (double)(this->GetY() - v2.GetY()),
			 (double)(this->GetZ() - v2.GetZ()), };
}

Vector Vector::operator*(const Vector& vec) const
{
	return { (double)(this->GetX() * vec.GetX()),
			 (double)(this->GetY() * vec.GetY()),
			 (double)(this->GetZ() * vec.GetZ()), };
}

Vector Vector::operator/(const Vector& vec) const
{
	return { (double)(this->GetX() / vec.GetX()),
			 (double)(this->GetY() / vec.GetY()),
			 (double)(this->GetZ() / vec.GetZ()), };
}

Vector Vector::operator*(const double& scale) const
{
	return { (double)(this->GetX() * scale),
			 (double)(this->GetY() * scale),
			 (double)(this->GetZ() * scale), };
}

Vector Vector::operator/(const double& scale) const
{
	return { (double)(this->GetX() / scale),
			 (double)(this->GetY() / scale),
			 (double)(this->GetZ() / scale), };
}

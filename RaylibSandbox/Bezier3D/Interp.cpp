#include <raymath.h>
#include "Interp.h"

#if 0
float Square(float f)
{
	return f * f;
}
float Cube(float f)
{
	return f * f * f;
}

float Qerp(float a, float b, float c, float t)
{
	return Square(1.0f - t) * a + 2.0f * (1.0f - t) * t * b + t * t * c;
}
float Cerp(float a, float b, float c, float d, float t)
{
	return Cube(1.0f - t) * a + 3.0f * t * Square(1.0f - t) * b + 3.0f * Square(t) * (1.0f - t) * c + Cube(t) * d;
}

Vector2 Vector2Qerp(Vector2 a, Vector2 b, Vector2 c, float t)
{
	float tp2 = t * t;
	float tm2 = 2.0f * t;
	float o0 = 1.0f - tm2 + tp2; // Square(1.0f - t)
	float o1 = tm2 - (2.0f * tp2); // 2.0f * (1.0f - t) * t
	float o2 = tp2; // t * t
	return {
		(o0 * a.x) + (o1 * b.x) + (o2 * c.x),
		(o0 * a.y) + (o1 * b.y) + (o2 * c.y)
	};
}
Vector2 Vector2Cerp(Vector2 a, Vector2 b, Vector2 c, Vector2 d, float t)
{
	float tp2 = t * t;
	float tp2m3 = 3.0f * tp2;
	float tp3 = tp2 * t;
	float tm3 = 3.0f * t;
	float tp3m3 = 3.0f * tp3;
	float o0 = 1.0f - tm3 + tp2m3 - tp3; // Cube(1.0f - t)
	float o1 = tm3 - (6.0f * tp2) + tp3m3; // 3.0f * t * Square(1.0f - t)
	float o2 = tp2m3 - tp3m3; // 3.0f * (1.0f - t) * Square(t)
	float o3 = tp3;
	return {
		(o0 * a.x) + (o1 * b.x) + (o2 * c.x) + (o3 * d.x),
		(o0 * a.y) + (o1 * b.y) + (o2 * c.y) + (o3 * d.y)
	};
}

Vector3 Vector3Qerp(Vector3 a, Vector3 b, Vector3 c, float t)
{
	float tp2 = t * t;
	float tm2 = 2.0f * t;
	float o0 = 1.0f - tm2 + tp2; // Square(1.0f - t)
	float o1 = tm2 - (2.0f * tp2); // 2.0f * (1.0f - t) * t
	float o2 = tp2; // t * t
	return {
		(o0 * a.x) + (o1 * b.x) + (o2 * c.x),
		(o0 * a.y) + (o1 * b.y) + (o2 * c.y),
		(o0 * a.z) + (o1 * b.z) + (o2 * c.z)
	};
}
Vector3 Vector3Cerp(Vector3 a, Vector3 b, Vector3 c, Vector3 d, float t)
{
	float tp2 = t * t;
	float tp2m3 = 3.0f * tp2;
	float tp3 = tp2 * t;
	float tm3 = 3.0f * t;
	float tp3m3 = 3.0f * tp3;
	float o0 = 1.0f - tm3 + tp2m3 - tp3; // Cube(1.0f - t)
	float o1 = tm3 - (6.0f * tp2) + tp3m3; // 3.0f * t * Square(1.0f - t)
	float o2 = tp2m3 - tp3m3; // 3.0f * (1.0f - t) * Square(t)
	float o3 = tp3;
	return {
		(o0 * a.x) + (o1 * b.x) + (o2 * c.x) + (o3 * d.x),
		(o0 * a.y) + (o1 * b.y) + (o2 * c.y) + (o3 * d.y),
		(o0 * a.z) + (o1 * b.z) + (o2 * c.z) + (o3 * d.z)
	};
}

#else

float Qerp(float a, float b, float c, float t)
{
	return Lerp(Lerp(a, b, t), Lerp(b, c, t), t);
}
float Cerp(float a, float b, float c, float d, float t)
{
	return Qerp(Lerp(a, b, t), Lerp(b, c, t), Lerp(c, d, t), t);
}

Vector2 Vector2Qerp(Vector2 a, Vector2 b, Vector2 c, float t)
{
	return {
		Qerp(a.x, b.x, c.x, t),
		Qerp(a.y, b.y, c.y, t)
	};
}
Vector2 Vector2Cerp(Vector2 a, Vector2 b, Vector2 c, Vector2 d, float t)
{
	return {
		Cerp(a.x, b.x, c.x, d.x, t),
		Cerp(a.y, b.y, c.y, d.y, t)
	};
}

Vector3 Vector3Qerp(Vector3 a, Vector3 b, Vector3 c, float t)
{
	return {
		Qerp(a.x, b.x, c.x, t),
		Qerp(a.y, b.y, c.y, t),
		Qerp(a.z, b.z, c.z, t)
	};
}
Vector3 Vector3Cerp(Vector3 a, Vector3 b, Vector3 c, Vector3 d, float t)
{
	return {
		Cerp(a.x, b.x, c.x, d.x, t),
		Cerp(a.y, b.y, c.y, d.y, t),
		Cerp(a.z, b.z, c.z, d.z, t)
	};
}

#endif
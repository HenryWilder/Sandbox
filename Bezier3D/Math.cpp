#include "Math.h"

float Lerp(float start, float end, float amount)
{
	return start + amount * (end - start);
}


Vector2 Vector2Zero()
{
	return Vector2{ 0.0f,0.0f };
}
Vector2 Vector2Add(Vector2 v1, Vector2 v2)
{
	return Vector2{ v1.x + v2.x, v1.y + v2.y };
}
Vector2 Vector2Subtract(Vector2 v1, Vector2 v2)
{
	return Vector2{ v1.x - v2.x, v1.y - v2.y };
}
Vector2 Vector2Multiply(Vector2 v1, Vector2 v2)
{
	return Vector2{ v1.x * v2.x, v1.y * v2.y };
}
Vector2 Vector2Divide(Vector2 v1, Vector2 v2)
{
	return Vector2{ v1.x / v2.x, v1.y / v2.y };
}
Vector2 Vector2Scale(Vector2 v, float scale)
{
	return Vector2{ v.x * scale, v.y * scale };
}
Vector2 Vector2Distance(Vector2 v1, Vector2 v2);

Vector2 Vector2Lerp(Vector2 start, Vector2 end, float amount)
{
	return {
		Lerp(start.x, end.x, amount),
		Lerp(start.y, end.y, amount)
	};
}


Vector3 Vector3Zero()
{
	return Vector3{ 0.0f,0.0f,0.0f };
}
Vector3 Vector3Add(Vector3 v1, Vector3 v2)
{
	return Vector3{ v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}
Vector3 Vector3Subtract(Vector3 v1, Vector3 v2)
{
	return Vector3{ v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}
Vector3 Vector3Multiply(Vector3 v1, Vector3 v2)
{
	return Vector3{ v1.x * v2.x, v1.y * v2.y, v1.z * v2.z };
}
Vector3 Vector3Divide(Vector3 v1, Vector3 v2)
{
	return Vector3{ v1.x / v2.x, v1.y / v2.y, v1.z / v2.z };
}
Vector3 Vector3Scale(Vector3 v, float scale)
{
	return Vector3{ v.x * scale, v.y * scale, v.z * scale };
}

Vector3 Vector3Lerp(Vector3 start, Vector3 end, float amount)
{
	return {
		Lerp(start.x, end.x, amount),
		Lerp(start.y, end.y, amount),
		Lerp(start.z, end.z, amount)
	};
}

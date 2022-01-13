#pragma once

float Lerp(float start, float end, float amount);


struct Vector2
{
	float x, y;
};

Vector2 Vector2Zero();
Vector2 Vector2Add(Vector2 v1, Vector2 v2);
Vector2 Vector2Subtract(Vector2 v1, Vector2 v2);
Vector2 Vector2Multiply(Vector2 v1, Vector2 v2);
Vector2 Vector2Divide(Vector2 v1, Vector2 v2);
Vector2 Vector2Scale(Vector2 v, float scale);
Vector2 Vector2Distance(Vector2 v1, Vector2 v2);

Vector2 Vector2Lerp(Vector2 start, Vector2 end, float amount);


struct Vector3
{
	float x, y, z;
};

Vector3 Vector3Zero();
Vector3 Vector3Add(Vector3 v1, Vector3 v2);
Vector3 Vector3Subtract(Vector3 v1, Vector3 v2);
Vector3 Vector3Multiply(Vector3 v1, Vector3 v2);
Vector3 Vector3Divide(Vector3 v1, Vector3 v2);
Vector3 Vector3Scale(Vector3 v, float scale);
Vector3 Vector3Distance(Vector3 v1, Vector3 v2);

Vector3 Vector3Lerp(Vector3 start, Vector3 end, float amount);

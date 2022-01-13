#pragma once

float Qerp(float a, float b, float c, float t);
float Cerp(float a, float b, float c, float d, float t);

Vector2 Vector2Qerp(Vector2 a, Vector2 b, Vector2 c, float t);
Vector2 Vector2Cerp(Vector2 a, Vector2 b, Vector2 c, Vector2 d, float t);

Vector3 Vector3Qerp(Vector3 a, Vector3 b, Vector3 c, float t);
Vector3 Vector3Cerp(Vector3 a, Vector3 b, Vector3 c, Vector3 d, float t);

float Vector2QerpLengthFast(Vector2 a, Vector2 b, Vector2 c);
float Vector2QerpLength(Vector2 a, Vector2 b, Vector2 c, float dt);
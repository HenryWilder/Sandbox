#include <raylib.h>
#include <raymath.h>
#include "Math.h"

#pragma region RangeAddress math

float ScaleFromRange(float min, float max) {
	return max - min;
}
float RangeToScalar(float value, float from_min, float from_max) {
	return ((value - from_min) / (from_max - from_min));
}
float RangeFromScalar(float value, float to_min, float to_max) {
	return (value * (to_max - to_min)) + to_min;
}
float RangeRemap(float value, float from_min, float from_max, float to_min, float to_max) {
	return (((value - from_min) / (from_max - from_min)) * (to_max - to_min)) + to_min;
}

#ifdef RAYMATH_H

Vector2 ScaleFromRangeVector2(Vector2 min, Vector2 max) {
	return max - min;
}
Vector2 RangeToScalarVector2(Vector2 value, Vector2 from_min, Vector2 from_max) {
	return ((value - from_min) / ScaleFromRangeVector2(from_min, from_max));
}
Vector2 RangeFromScalarVector2(Vector2 value, Vector2 to_min, Vector2 to_max) {
	return (value * ScaleFromRangeVector2(to_min, to_max)) + to_min;
}
Vector2 RangeRemapVector2(Vector2 value, Vector2 from_min, Vector2 from_max, Vector2 to_min, Vector2 to_max) {
	return (((value - from_min) / ScaleFromRangeVector2(from_min, from_max)) * ScaleFromRangeVector2(to_min, to_max)) + to_min;
}

Vector3 ScaleFromRangeVector3(Vector3 min, Vector3 max) {
	return max - min;
}
Vector3 RangeToScalarVector3(Vector3 value, Vector3 from_min, Vector3 from_max) {
	return ((value - from_min) / ScaleFromRangeVector3(from_min, from_max));
}
Vector3 RangeFromScalarVector3(Vector3 value, Vector3 to_min, Vector3 to_max) {
	return (value * ScaleFromRangeVector3(to_min, to_max)) + to_min;
}
Vector3 RangeRemapVector3(Vector3 value, Vector3 from_min, Vector3 from_max, Vector3 to_min, Vector3 to_max) {
	return (((value - from_min) / ScaleFromRangeVector3(from_min, from_max)) * ScaleFromRangeVector3(to_min, to_max)) + to_min;
}

#endif // RAYMATH_H

#pragma endregion
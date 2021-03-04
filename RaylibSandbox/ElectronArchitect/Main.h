#pragma once

enum class WireShape : char;

struct Int3;

int Log2(int val);
float Round(float f);
Vector2 Vector2Round(Vector2 vec);
Vector2 Vector2Snap(Vector2 vec, float to);
Color DepthColor(Color color, float z, float target, float zoom);
void DrawWire(Int3 start, Int3 end, WireShape shape, bool b_state, float targetDepth, float zoom);
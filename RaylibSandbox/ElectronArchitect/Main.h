#pragma once

int Log2(int val);
float Round(float f);
Vector2 Vector2Round(Vector2 vec);
Vector2 Vector2Snap(Vector2 vec, float to);
Color DepthColor(Color color, float z, float target, float zoom);
void DrawWire(Vector2 start, Vector2 end, WireShape shape, bool b_state, float depth, float targetDepth, float zoom);
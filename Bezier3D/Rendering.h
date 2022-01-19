#pragma once

struct Color
{
	unsigned char r, g, b;
};

void DrawTriangle(Vector2 p0, Vector2 p1, Vector2 p2, Color color);
void DrawTriangleFan(Vector2* points, int ptcount, Color color);

#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "Interp.h"
#include "Triangle.h"

void DrawWTri2Q(QuadTriangle2 tri, float increment, Color color)
{
	int count = (int)ceilf(3.0f / increment) + 2;
	Vector2* points = new Vector2[count];

	size_t i = 0;
	points[i++] = tri.Center();

	points[i++] = tri.p0;

	for (float t = increment; t < 1.0f; t += increment)
	{
		points[i++] = Vector2Qerp(tri.p0, tri.c01, tri.p1, t);
	}
	points[i++] = tri.p1;

	for (float t = increment; t < 1.0f; t += increment)
	{
		points[i++] = Vector2Qerp(tri.p1, tri.c12, tri.p2, t);
	}
	points[i++] = tri.p2;

	for (float t = increment; t < 1.0f; t += increment)
	{
		points[i++] = Vector2Qerp(tri.p2, tri.c20, tri.p0, t);
	}
	points[i++] = tri.p0;

	DrawTriangleFan(points, count, color);

	delete[] points;
}

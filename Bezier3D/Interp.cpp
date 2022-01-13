#include "Math.h"
#include "Interp.h"

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


#if 0

struct WTri2Q
{
	/*
	*	    p0
	*	  20  01
	*	p2  12  p1
	*/

	Vector2 p0;
	Vector2 c01;

	Vector2 p1;
	Vector2 c12;

	Vector2 p2;
	Vector2 c20;

	Vector2 Center()
	{
		return Vector2{
			(p0.x + c01.x + p1.x + c12.x + p2.x + c20.x) / 6.0f,
			(p0.y + c01.y + p1.y + c12.y + p2.y + c20.y) / 6.0f
		};
	}
};
void DrawWTri2Q(WTri2Q tri, float increment)
{
	Vector2 center = tri.Center();

	Vector2 last = tri.p0;
	for (float t = increment; t < 1.0f; t += increment)
	{
		Vector2 current = Qerp2(tri.p0, tri.c01, tri.p1, t);
		DrawTriangle2D(last, current, center);
		last = current;
	}
	DrawTriangle2D(last, tri.p1, center);

	last = tri.p1;
	for (float t = increment; t < 1.0f; t += increment)
	{
		Vector2 current = Qerp2(tri.p1, tri.c12, tri.p2, t);
		DrawTriangle2D(last, current, center);
		last = current;
	}
	DrawTriangle2D(last, tri.p2, center);

	last = tri.p2;
	for (float t = increment; t < 1.0f; t += increment)
	{
		Vector2 current = Qerp2(tri.p2, tri.c20, tri.p0, t);
		DrawTriangle2D(last, current, center);
		last = current;
	}
	DrawTriangle2D(last, tri.p0, center);
}

struct WTri2C
{
	/*
	*	      p0
	*	    02  01
	*	  20      10
	*	p2  21  12  p1
	*/

	Vector2 p0;
	Vector2 c01;

	Vector2 c10;
	Vector2 p1;
	Vector2 c12;

	Vector2 c21;
	Vector2 p2;
	Vector2 c20;

	Vector2 c02;

	Vector2 Center()
	{
		return Vector2{
			(p0.x + c01.x + c02.x + p1.x + c10.x + c12.x + p2.x + c20.x + c21.x) / 9.0f,
			(p0.y + c01.y + c02.y + p1.y + c10.y + c12.y + p2.y + c20.y + c21.y) / 9.0f
		};
	}
};
void DrawWTri2C(WTri2C tri, float increment)
{
	Vector2 center = tri.Center();

	Vector2 last = tri.p0;
	for (float t = increment; t < 1.0f; t += increment)
	{
		Vector2 current = Cerp2(tri.p0, tri.c01, tri.c10, tri.p1, t);
		DrawTriangle2D(last, current, center);
		last = current;
	}
	DrawTriangle2D(last, tri.p1, center);

	last = tri.p1;
	for (float t = increment; t < 1.0f; t += increment)
	{
		Vector2 current = Cerp2(tri.p1, tri.c12, tri.c21, tri.p2, t);
		DrawTriangle2D(last, current, center);
		last = current;
	}
	DrawTriangle2D(last, tri.p2, center);

	last = tri.p2;
	for (float t = increment; t < 1.0f; t += increment)
	{
		Vector2 current = Cerp2(tri.p2, tri.c20, tri.c02, tri.p0, t);
		DrawTriangle2D(last, current, center);
		last = current;
	}
	DrawTriangle2D(last, tri.p0, center);
}

#endif

float Vector2QerpLengthFast(Vector2 a, Vector2 b, Vector2 c)
{

}
float Vector2QerpLength(Vector2 a, Vector2 b, Vector2 c, float dt)
{

}
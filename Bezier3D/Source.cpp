#include <windows.h>
#include <windowsx.h>

#if 0
float Square(float f)
{
	return f * f;
}
float Cube(float f)
{
	return f * f * f;
}

float Lerp(float a, float b, float t)
{
	return a + t * (b - a);
}
float Qerp(float a, float b, float c, float t)
{
	// return Lerp(Lerp(a, b, t), Lerp(b, c, t), t);
	return Square(1.0f - t) * a + 2.0f * (1.0f - t) * t * b + t * t * c;
}
float Cerp(float a, float b, float c, float d, float t)
{
	// return Qerp(Lerp(a, b, t), Lerp(b, c, t), Lerp(c, d, t), t);
	return Cube(1.0f - t) * a + 3.0f * t * Square(1.0f - t) * b + 3.0f * Square(t) * (1.0f - t) * c + Cube(t) * d;
}

struct Vector2
{
	float x, y;
};

Vector2 Lerp2(Vector2 a, Vector2 b, float t)
{
	return {
		a.x + t * (b.x - a.x),
		a.y + t * (b.y - a.y)
	};
}
Vector2 Qerp2(Vector2 a, Vector2 b, Vector2 c, float t)
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
Vector2 Cerp2(Vector2 a, Vector2 b, Vector2 c, Vector2 d, float t)
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

struct Vector3
{
	float x, y, z;
};

Vector3 Lerp3(Vector3 a, Vector3 b, float t)
{
	return {
		a.x + t * (b.x - a.x),
		a.y + t * (b.y - a.y),
		a.z + t * (b.z - a.z)
	};
}
Vector3 Qerp3(Vector3 a, Vector3 b, Vector3 c, float t)
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
Vector3 Cerp3(Vector3 a, Vector3 b, Vector3 c, Vector3 d, float t)
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

float Lerp(float a, float b, float t)
{
	return a + t * (b - a);
}
float Qerp(float a, float b, float c, float t)
{
	return Lerp(Lerp(a, b, t), Lerp(b, c, t), t);
}
float Cerp(float a, float b, float c, float d, float t)
{
	return Qerp(Lerp(a, b, t), Lerp(b, c, t), Lerp(c, d, t), t);
}

struct Vector2
{
	float x, y;
};

Vector2 Lerp2(Vector2 a, Vector2 b, float t)
{
	return {
		Lerp(a.x, b.x, t),
		Lerp(a.y, b.y, t)
	};
}
Vector2 Qerp2(Vector2 a, Vector2 b, Vector2 c, float t)
{
	return {
		Qerp(a.x, b.x, c.x, t),
		Qerp(a.y, b.y, c.y, t)
	};
}
Vector2 Cerp2(Vector2 a, Vector2 b, Vector2 c, Vector2 d, float t)
{
	return {
		Cerp(a.x, b.x, c.x, d.x, t),
		Cerp(a.y, b.y, c.y, d.y, t)
	};
}

struct Vector3
{
	float x, y, z;
};

Vector3 Lerp3(Vector3 a, Vector3 b, float t)
{
	return {
		Lerp(a.x, b.x, t),
		Lerp(a.y, b.y, t),
		Lerp(a.z, b.z, t)
	};
}
Vector3 Qerp3(Vector3 a, Vector3 b, Vector3 c, float t)
{
	return {
		Qerp(a.x, b.x, c.x, t),
		Qerp(a.y, b.y, c.y, t),
		Qerp(a.z, b.z, c.z, t)
	};
}
Vector3 Cerp3(Vector3 a, Vector3 b, Vector3 c, Vector3 d, float t)
{
	return {
		Cerp(a.x, b.x, c.x, d.x, t),
		Cerp(a.y, b.y, c.y, d.y, t),
		Cerp(a.z, b.z, c.z, d.z, t)
	};
}

#endif

//Get a console handle
HWND g_window = GetConsoleWindow();
//Get a handle to device context
HDC g_hdc = GetDC(g_window);

void DrawTriangle2D(Vector2 p0, Vector2 p1, Vector2 p2)
{
	POINT vertices[] = {
		{ (LONG)(p0.x + 0.5f), (LONG)(p0.y + 0.5f) },
		{ (LONG)(p1.x + 0.5f), (LONG)(p1.y + 0.5f) },
		{ (LONG)(p2.x + 0.5f),(LONG)(p2.y + 0.5f) }
	};
	Polygon(g_hdc, vertices, 3);
}

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


int main()
{
	HPEN hPen = CreatePen(PS_SOLID, 0, RGB(0, 0, 255));
	HPEN hOldPen = SelectPen(g_hdc, hPen);

	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 255));
	HBRUSH hOldBrush = SelectBrush(g_hdc, hBrush);

	WTri2Q qTri0 = WTri2Q{
		Vector2{ 200, 200 }, // p0
		Vector2{ 250, 250 },
		Vector2{ 300, 300 }, // p1
		Vector2{ 200, 350 },
		Vector2{ 100, 300 }, // p2
		Vector2{ 200, 300 }
	};
	DrawWTri2Q(qTri0, 0.025f);
	
	WTri2Q qTri1 = WTri2Q{
		Vector2{ 200, 200 }, // p0
		Vector2{ 300, 150 },
		Vector2{ 400, 200 }, // p1
		Vector2{ 300, 200 },
		Vector2{ 300, 300 }, // p2
		Vector2{ 250, 250 },
	};
	DrawWTri2Q(qTri1, 0.025f);

	//WTri2C cTri = WTri2C{
	//	Vector2{  60, 200 - 70 }, // p0
	//	Vector2{ 130, 200 - 70 }, // c01
	//	Vector2{ 180, 200 -  0 }, // c10
	//	Vector2{ 130, 200 -  0 }, // p1
	//	Vector2{ 130, 200 - 70 }, // c12
	//	Vector2{  60, 200 -  0 }, // c21
	//	Vector2{  20, 200 -  0 }, // p2
	//	Vector2{  20, 200 - 70 }, // c20
	//	Vector2{  60, 200 - 30 } // c02
	//};
	//DrawWTri2C(cTri, 0.025f);

	SelectBrush(g_hdc, hOldBrush);
	DeleteObject(hBrush);

	SelectPen(g_hdc, hOldPen);
	DeleteObject(hPen);

	return 0;
}

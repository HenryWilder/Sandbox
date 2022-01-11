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
};
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
	Vector2 c02;

	Vector2 p1;
	Vector2 c10;
	Vector2 c12;

	Vector2 p2;
	Vector2 c20;
	Vector2 c21;
};

struct WTri3Q
{
	/*
	*	    p0
	*	  20  01
	*	p2  12  p1
	*/

	Vector3 p0;
	Vector3 c01;

	Vector3 p1;
	Vector3 c12;

	Vector3 p2;
	Vector3 c20;
};
struct WTri3C
{
	/*
	*	      p0
	*	    02  01
	*	  20      10
	*	p2  21  12  p1
	*/

	Vector3 p0;
	Vector3 c01;
	Vector3 c02;

	Vector3 p1;
	Vector3 c10;
	Vector3 c12;

	Vector3 p2;
	Vector3 c20;
	Vector3 c21;
};

int main()
{
	return 0;
}

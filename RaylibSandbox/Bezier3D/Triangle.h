#pragma once

struct QuadTriangle2
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

void DrawWTri2Q(QuadTriangle2 tri, float increment, Color color);

struct CubicTriangle2
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

void DrawWTri2C(CubicTriangle2 tri, float increment, Color color);

struct QuadTriangle3
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

	Vector3 Center()
	{
		return Vector3{
			(p0.x + c01.x + p1.x + c12.x + p2.x + c20.x) / 6.0f,
			(p0.y + c01.y + p1.y + c12.y + p2.y + c20.y) / 6.0f,
			(p0.z + c01.z + p1.z + c12.z + p2.z + c20.z) / 6.0f
		};
	}
};

struct CubicTriangle3
{
	/*
	*	      p0
	*	    02  01
	*	  20      10
	*	p2  21  12  p1
	*/

	Vector3 p0;
	Vector3 c01;

	Vector3 c10;
	Vector3 p1;
	Vector3 c12;

	Vector3 c21;
	Vector3 p2;
	Vector3 c20;

	Vector3 c02;

	Vector3 Center()
	{
		return Vector3{
			(p0.x + c01.x + c02.x + p1.x + c10.x + c12.x + p2.x + c20.x + c21.x) / 9.0f,
			(p0.y + c01.y + c02.y + p1.y + c10.y + c12.y + p2.y + c20.y + c21.y) / 9.0f,
			(p0.z + c01.z + c02.z + p1.z + c10.z + c12.z + p2.z + c20.z + c21.z) / 9.0f
		};
	}
};
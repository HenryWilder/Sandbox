#pragma once
#include "Math.h"

using namespace ext;

namespace pce
{
	/* -Rotator type-
	Negative rotations don't play nice with the form of rotation in this project.
	Specifically, selecting the rotation from an array of pre-rotated shapes.
	As such, this type was made so that it could have specially designed operators for modulo addition,
	while remaining only 1 byte large. */
	struct Rot
	{
		// Creates a rotator from the input number
		Rot(unsigned char rotation);
		Rot(char rotation);
		Rot(unsigned int rotation);
		Rot(int rotation);
		Rot(unsigned short rotation);
		Rot(short rotation);
		// Defaults 0
		Rot();

		/* A rot is a 0-3 value for a number of quarter-turns
		0 =   0 or -360 degrees
		1 =  90 or -270 degrees
		2 = 180 or -180 degrees
		3 = 270 or  -90 degrees */
		unsigned char rot;

		Rot operator+(const signed char delta)
		{
			// Changes in rotation over 360 degrees are identical to the 360 degree modulo of the rotation
			// Additionally, a rotation of four quarter-turns is identical to no turns at all.
			signed char change = (delta % 4);
			// We will no longer use delta from this point forward. It is replaced with "change"

			signed char out = signed(rot); // The variable "out" is a signed stand-in for the temporary rotation

			out += change;

			/* It is possible at this point that the sum of the initial rotation and the delta rotation is greater than 3
			For example:
			1 + 3 > 3
			2 + 2 > 3
			3 + 1 > 3
			Which is why we once again modulo the result. */

			out = out % 4;

			/* However, there is also the possibility that the sum, should the delta rotation be negative, is less than 0.
			For example:
			0 - 1 < 0
			1 - 2 < 0
			2 - 3 < 0
			A negative rotation, however, is not the same as the absolute value of the rotation.
			So we add 360 degrees to the rotation to find the positive equivalent. */
			if (out < 0)
				out += 4;

			return Rot(unsigned(out));

			/*
			3 + 1 = 0
			3 + 2 = 1
			3 + 3 = 2
			3 + 4 = 3

			0 - 1 = 3
			0 - 2 = 2
			0 - 3 = 1
			0 - 4 = 0
			*/
		}
		Rot& operator+=(const signed char delta)
		{
			this->rot = this->rot + delta;
			return *this;
		}
	};

	struct Offset
	{
		Offset(short xIn, short yIn);
		Offset();

		short x;
		short y;
	};

	struct AbstractShape
	{
		// Effectively just creates a shape from the definition of a shape
		AbstractShape(Offset offset[4]);
		// Creates a shape from the coordinates of each point without being inside an array
		AbstractShape(Offset offset1, Offset offset2, Offset offset3, Offset offset4);
		// Constructs a shape from a series of coordinate parts, for defining a brand new shape.
		AbstractShape(short x1, short y1, short x2, short y2, short x3, short y3, short x4, short y4);
		// Creates an "O" piece by default
		AbstractShape();

		AbstractShape(const AbstractShape&) = default;

		Offset m_vert[4];
	};

	struct AbstRotShape
	{
		// Creates a rotatible shape with four distinct rotations (like the T or L pieces)
		AbstRotShape(AbstractShape orient0, AbstractShape orient1, AbstractShape orient2, AbstractShape orient3);
		// Creates a rotatible shape with two distinct rotations and two rotational symmetries (like the I or S pieces)
		AbstRotShape(AbstractShape orient0And2, AbstractShape orient1And3);
		// Creates a rotatible shape with one distinct rotation and full rotational symmetry (like the O piece)
		AbstRotShape(AbstractShape orient0Through3);
		// Creates an O piece
		AbstRotShape();

		AbstRotShape(const AbstRotShape&) = default;

		AbstractShape m_orientation[4];

		AbstractShape Rotation(Rot rot);
	};

	struct Shape
	{
		// Create an unrotated shape at the coordinates
		Shape(Coord pos, AbstRotShape* refShape);
		// Create an unrotated shape at the coordinates (implies "Coord(x,y)")
		Shape(Whole xPos, Whole yPos, AbstRotShape* refShape);
		// Create the input shape at the origin
		Shape(AbstRotShape* refShape);
		// Create an O-piece at (0,0)
		//TODO: FIX NULLPTR
		Shape();

		Coord m_position;
		AbstRotShape* m_template;
		AbstractShape m_outputShape;
		Rot m_rot; // (0,3)

		/// <summary>
		/// <para>Turn 90 degrees in the provided direction.</para>
		/// <para>Positive values for CW rotation, negative for CC.
		/// Values beyond (-1,1) will affect the amount the shape rotates.</para>
		/// </summary>
		/// <param name="turns">Number of quarter-turns by which the shape should rotate.</param>
		void Rotate(signed char turns);

		/// <summary>
		/// Gives the rotation of the shape with the input number of quarter-turns.
		/// </summary>
		/// <param name="rot">Number of quarter-turns</param>
		/// <returns>The abstract shape at this orientation of type "AbstractShape"</returns>
		AbstractShape GetRotation(Rot rot);
	};

	static AbstRotShape g_Ipiece;
	static AbstRotShape g_Jpiece;
	static AbstRotShape g_Lpiece;
	static AbstRotShape g_Opiece;
	static AbstRotShape g_Spiece;
	static AbstRotShape g_Tpiece;
	static AbstRotShape g_Zpiece;
}

// int size = sizeof Shape;
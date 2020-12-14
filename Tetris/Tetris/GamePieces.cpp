#include "GamePieces.h"
#include "TetrisBlocks.h"

using namespace pce;

namespace Block {

	Rot::Rot(unsigned char rotation)
	{
		rot = rotation % 4;
	}

	Rot::Rot(char rotation)
	{
		if (rotation < 0)
			rot = (rotation % 4) + 4;
		else
			rot = rotation;
	}

	Rot::Rot(unsigned int rotation)
	{
		rot = rotation;
	}

	Rot::Rot(int rotation)
	{
		if (rotation < 0)
			rot = (rotation % 4) + 4;
		else
			rot = rotation;
	}

	Rot::Rot(unsigned short rotation)
	{
		rot = char(rotation);
	}

	Rot::Rot(short rotation)
	{
		if (rotation < 0)
			rot = (rotation % 4) + 4;
		else
			rot = char(rotation);
	}

	Rot::Rot()
	{
		rot = 0;
	}

	Shape::Shape(Coord pos, AbstRotShape* refShape)
	{
		m_position = pos;
		m_template = refShape;
		m_rot = 0;
		m_outputShape = GetRotation(m_rot);
	}

	Shape::Shape(Whole xPos, Whole yPos, AbstRotShape* refShape)
	{
		Shape(Coord(xPos, yPos), refShape);
	}

	Shape::Shape(AbstRotShape* refShape)
	{
		m_position = Coord(0, 0);
		m_template = refShape;
		m_rot = 0;
		m_outputShape = GetRotation(m_rot);
	}

	Block::Shape::Shape(Coord pos0, Coord pos1, Coord pos2, Coord pos3)
	{
	}

	Shape::Shape()
	{
		m_position = Coord(0, 0);
		m_template = nullptr;
		m_rot = 0;
		m_outputShape = GetRotation(m_rot);
	}

	void Shape::Rotate(signed char turns)
	{
		m_rot += turns;
		m_outputShape = GetRotation(m_rot);
	}

	AbstractShape Shape::GetRotation(Rot rot)
	{
		return m_template->m_orientation[rot.rot];
	}

	AbstractShape::AbstractShape(Offset offset[4])
	{
		for (int i = 0; i < 4; ++i)
		{
			m_vert[i] = offset[i];
		}
	}

	AbstractShape::AbstractShape(Offset offset1, Offset offset2, Offset offset3, Offset offset4)
	{
		m_vert[0] = offset1;
		m_vert[1] = offset2;
		m_vert[2] = offset3;
		m_vert[3] = offset4;
	}

	AbstractShape::AbstractShape(short x1, short y1, short x2, short y2, short x3, short y3, short x4, short y4)
	{
		AbstractShape(Offset(x1, y1), Offset(x2, y2), Offset(x3, y3), Offset(x4, y4));
	}

	AbstractShape::AbstractShape()
	{
		AbstractShape(0, 0, 0, 1, 1, 0, 1, 1);
	}

	pce::AbstRotShape::AbstRotShape(AbstractShape orient0, AbstractShape orient1, AbstractShape orient2, AbstractShape orient3)
	{
		m_orientation[0] = orient0;
		m_orientation[1] = orient1;
		m_orientation[2] = orient2;
		m_orientation[3] = orient3;
	}

	pce::AbstRotShape::AbstRotShape(AbstractShape orient0And2, AbstractShape orient1And3)
	{
		m_orientation[0] = orient0And2;
		m_orientation[1] = orient1And3;
		m_orientation[2] = orient0And2;
		m_orientation[3] = orient1And3;
	}

	pce::AbstRotShape::AbstRotShape(AbstractShape orient0Through3)
	{
		for (int i = 0; i < 4; ++i)
		{
			m_orientation[i] = orient0Through3;
		}
	}

	AbstRotShape::AbstRotShape()
	{
		m_orientation[0] = AbstractShape();
		m_orientation[1] = AbstractShape();
		m_orientation[2] = AbstractShape();
		m_orientation[3] = AbstractShape();
	}

	AbstractShape AbstRotShape::Rotation(Rot rot)
	{
		return m_orientation[rot.rot];
	}


	AbstRotShape g_Ipiece = AbstRotShape(
		AbstractShape(Offset(), Offset(0, 1), Offset(0, 2), Offset(0, 3)),
		AbstractShape(Offset(), Offset(1, 0), Offset(2, 0), Offset(3, 0)));

	AbstRotShape g_Jpiece = AbstRotShape(
		AbstractShape(Offset(), Offset(), Offset(), Offset()),
		AbstractShape(Offset(), Offset(), Offset(), Offset()),
		AbstractShape(Offset(), Offset(), Offset(), Offset()),
		AbstractShape(Offset(), Offset(), Offset(), Offset()));

	AbstRotShape g_Lpiece = AbstRotShape(
		AbstractShape(Offset(), Offset(), Offset(), Offset()),
		AbstractShape(Offset(), Offset(), Offset(), Offset()),
		AbstractShape(Offset(), Offset(), Offset(), Offset()),
		AbstractShape(Offset(), Offset(), Offset(), Offset()));

	AbstRotShape g_Opiece = AbstRotShape();

	AbstRotShape g_Spiece = AbstRotShape(
		AbstractShape(Offset(), Offset(), Offset(), Offset()),
		AbstractShape(Offset(), Offset(), Offset(), Offset()));

	AbstRotShape g_Tpiece = AbstRotShape(
		AbstractShape(Offset(), Offset(), Offset(), Offset()),
		AbstractShape(Offset(), Offset(), Offset(), Offset()),
		AbstractShape(Offset(), Offset(), Offset(), Offset()),
		AbstractShape(Offset(), Offset(), Offset(), Offset()));

	AbstRotShape g_Zpiece = AbstRotShape(
		AbstractShape(Offset(), Offset(), Offset(), Offset()),
		AbstractShape(Offset(), Offset(), Offset(), Offset()));

	pce::Offset::Offset(short xIn, short yIn)
	{
		x = xIn;
		y = yIn;
	}

	Offset::Offset()
	{
		x = 0;
		y = 0;
	}

}
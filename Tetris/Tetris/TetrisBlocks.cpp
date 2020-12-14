#include "TetrisBlocks.h"

namespace Block {

	void GamePiece::SetPosition(Coord newOrigin)
	{
		Coord deltaPos = origin - newOrigin;
		ChangePosition(deltaPos);
	}

	void GamePiece::ChangePosition(Coord deltaPos)
	{
		origin = origin + deltaPos;
		for (int i = 0; i < 4; ++i)
		{
			shape.pos[i] = shape.pos[i] + deltaPos;
		}
	}

	void GamePiece::MoveRight()
	{
		ChangePosition(Coord(1, 0));
	}

	void GamePiece::MoveLeft()
	{
		ChangePosition(Coord(-1, 0));
	}

	void GamePiece::MoveDown()
	{
		ChangePosition(Coord(0, 1));
	}

	void GamePiece::Init(AbstractPiece* abstract)
	{
		
	}

	void GamePiece::Rotate(int qTurns)
	{
		// Positive values are clockwise, negative values are counterclockwise.
		bool direction = BoolSign(qTurns);

		// We are about to use this at least four times in short succession. We might as well declair it out here.
		Coord temp;

		for (int i = 0; i < abs(qTurns); ++i)
		{
			for (int b = 0; b < 4; ++b)
			{
				temp = shape.pos[b] - origin;

				if (direction) // A 90 degree clockwise rotation is the same as a 270 degree rotation clockwise
				{
					shape.pos[b].x = origin.x + temp.y;
					shape.pos[b].y = origin.y - temp.x;
				}
				else // If negative, rotate counterclockwise
				{
					shape.pos[b].x = origin.x - temp.y;
					shape.pos[b].y = origin.y + temp.x;
				}
			}
		}
	}

	AbstractPiece::AbstractPiece(Shape shapeIn, Coord origIn, Color col)
	{
		for (size_t i = 0; i < 4; i++)
		{
			shape.pos[i] = shapeIn.pos[i];
		}
		origin = origIn;
		color = col;
	}

	AbstractPiece::AbstractPiece()
	{
		for (size_t i = 0; i < 4; i++)
		{
			shape.pos[i] = Coord();
		}
		origin = Coord();
		color = Color();
	}

	Shape::Shape(Coord pos0, Coord pos1, Coord pos2, Coord pos3)
	{
		pos[0] = pos0;
		pos[1] = pos1;
		pos[2] = pos2;
		pos[3] = pos3;
	}

	Shape::Shape()
	{
		pos[0] = Coord();
		pos[1] = Coord();
		pos[2] = Coord();
		pos[3] = Coord();
	}

}
#pragma once
#include "Math.h"
#include "Buffer.h"

using namespace ext;

namespace Block
{
	struct Shape
	{
		Shape(Coord pos0, Coord pos1, Coord pos2, Coord pos3);
		Shape();

		Shape(const Shape& shapeIn)
		{
			for (Byte i = 0; i < 4; i++)
			{
				pos[i] = shapeIn.pos[i];
			}
		}

		Coord pos[4];
	};

	struct AbstractPiece
	{
		AbstractPiece(Shape shapeIn, Coord origIn, Color col);
		AbstractPiece();

		Shape shape;
		Coord origin;
		Color color;
	};

	const static AbstractPiece g_PieceI;
	const static AbstractPiece g_PieceJ;
	const static AbstractPiece g_PieceL;
	const static AbstractPiece g_PieceO;
	const static AbstractPiece g_PieceS;
	const static AbstractPiece g_PieceT;
	const static AbstractPiece g_PieceZ;

	class GamePiece
	{
	private: // Only the parent class can use these
		void SetPosition(Coord newOrigin); // Sets the position directly
		void ChangePosition(Coord deltaPos); // Adds an offset to the position

		bool CanMoveRight(); // Checks if the piece can move to the right one space
		bool CanMoveLeft(); // Checks if the piece can move to the left one space
		bool CanMoveDown(); // Checks if the piece can move down one space
		bool CheckPosition(); // Checks if a position is already taken (used in "CanMove"s)

		void Rotate(int qTurns); // Adds rotation


		Shape shape; // The on-screen positions of each pixel
		Coord origin; // The position about which the shape will rotate
		Color color; // The color of the game piece

	public: // Anyone can use these
		void MoveRight(); // Move the piece right 1 space
		void MoveLeft(); // Move the piece left 1 space
		void MoveDown(); // Move the piece down 1 space

		void RotateCW(); // Rotate clockwise 90 degrees
		void RotateCC(); // Rotate counter clockwise 90 degrees

		void Init(AbstractPiece* abstract); // Initialize the piece with an abstracted version
	};
}
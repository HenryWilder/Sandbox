#pragma once

struct Position
{
	int x, y;
};

class Unit
{
	enum class UnitColor : bool
	{
		Black = false,
		White,
	};
	enum class Type : char
	{
		Pawn = 0,
		Rook,
		Knight,
		Bishop,
		Queen,
		King,
	};

	UnitColor color;
	Type type;
	Position pos;
};

class Pawn : Unit
{

};

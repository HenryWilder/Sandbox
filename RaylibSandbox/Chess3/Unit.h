#pragma once

struct Vector2;

class Board;

class Unit
{
public:
	enum class UnitColor : bool	{
		Black,
		White
	};
	Unit() : pos(), color(UnitColor::Black) {};
	Unit(Vector2 _pos) : pos(_pos), color(UnitColor::Black) {};
	Unit(Vector2 _pos, UnitColor _color) : pos(_pos), color(_color) {};

	void Draw();
	void Move(Vector2 newPos);
	Vector2 GetPos() const;
	bool IsBlack() const;
	bool IsWhite() const;

protected:
	Vector2 pos;
	UnitColor color; // false = black, true = white
};
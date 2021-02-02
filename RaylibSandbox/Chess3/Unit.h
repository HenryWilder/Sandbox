#pragma once

struct Vector2;

class Board;

class Unit
{
public:
	Unit() : pos() {};
	Unit(Vector2 _pos) : pos(_pos) {};

	void Draw();
	void Move(Vector2 newPos);

protected:
	Vector2 pos;
};
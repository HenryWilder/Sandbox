#pragma once
#include <raylib.h>

class CBButton
{
public:
	CBButton(Vector2 position) : position(position) {}

	static constexpr Vector2 size = {30,30};
	Vector2 position;

	void Draw() const;
	bool IsHovered(Vector2 point) const;
};

class CardColorButton : private CBButton
{
public:
	CardColorButton(Vector2 position) : CBButton(position) {}
};

class ThreadColorButton : private CBButton
{
public:
	ThreadColorButton(Vector2 position) : CBButton(position) {}
};

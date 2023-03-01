#pragma once
#include <raylib.h>

class CBButton
{
public:
	CBButton(int x, int y, Color color, bool isThreadColor) : position{(float)x,(float)y}, color(color), isThreadColor(isThreadColor) {}

	static constexpr Vector2 size = {30,30};
	Vector2 position;
	Color color;
	bool isThreadColor; // Card color otherwise

	void Draw() const;
	bool IsHovered(Vector2 point) const;

	Rectangle GetRectangle() const;
};

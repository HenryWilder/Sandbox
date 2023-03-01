#include "CBButton.h"

void CBButton::Draw() const
{
	DrawRectangleV(position, size, GRAY);

	int left = position.x + 5;
	int width = size.x - 10;

	if (isThreadColor)
	{
		DrawRectangle(left, position.y + size.y / 2 - 1, width, 2, color);
	}
	else
	{
		DrawRectangle(left, position.y + 8, width, size.y - 16, color);
	}
	DrawRectangleLines(position.x, position.y, size.x, size.y, DARKGRAY);
}

bool CBButton::IsHovered(Vector2 point) const
{
	bool hovered =
		point.x >= position.x &&
		point.x <= position.x + size.x &&
		point.y >= position.y &&
		point.y <= position.y + size.y;

	return hovered;
}

Rectangle CBButton::GetRectangle() const
{
	return { position.x, position.y, size.x, size.y };
}

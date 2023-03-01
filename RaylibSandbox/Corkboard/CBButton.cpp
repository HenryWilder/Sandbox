#include "CBButton.h"

void CBButton::Draw() const
{
	DrawRectangleV(position, size, GRAY);
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

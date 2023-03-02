#include "CBButton.h"

Color cardColor;
Color threadColor;

void CBButton::Draw() const
{
	DrawRectangleV(position, size, GRAY);

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

void ThreadColorButton::Draw() const
{
	CBButton::Draw();

	int left = position.x + 5;
	int width = size.x - 10;

	DrawRectangle(left, position.y + size.y / 2 - 1, width, 2, color);
}

void ThreadColorButton::OnClick() const
{
	threadColor = color;
}

void CardColorButton::Draw() const
{
	CBButton::Draw();

	int left = position.x + 5;
	int width = size.x - 10;

	DrawRectangle(left, position.y + 8, width, size.y - 16, color);
}

void CardColorButton::OnClick() const
{
	cardColor = color;
}

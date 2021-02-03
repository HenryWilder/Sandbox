#include <raylib.h>
#include "Global.h"
#include "Unit.h"

void Unit::Draw()
{
	if (this) DrawTextureRec(
		sprite::pawn,
		{ 0,0, 16,-16 },
		BoardToScreen(pos),
		WHITE);
}

void Unit::Move(Vector2 newPos)
{
	pos.x = newPos.x;
	pos.y = newPos.y;
}

Vector2 Unit::GetPos() const
{
	return pos;
}

bool Unit::IsBlack() const
{
	return color == UnitColor::Black;
}

bool Unit::IsWhite() const
{
	return color == UnitColor::White;
}

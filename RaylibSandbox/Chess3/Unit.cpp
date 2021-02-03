#include <raylib.h>
#include "Global.h"
#include "Unit.h"

void Unit::Draw()
{
	if (this) DrawTextureRec(
		*sprite::pawn,
		{ 0,0, 16,-16 },
		Vector2{
			(pos.x * spce::scrn::g_tileWidth) - (spce::scrn::g_tileWidth * 0.5f),
			(spce::scrn::g_boardWidth - spce::scrn::g_tileWidth - (pos.y * spce::scrn::g_tileWidth)) + (spce::scrn::g_tileWidth * 0.5f)
		},
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

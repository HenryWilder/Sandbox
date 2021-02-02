#include <raylib.h>
#include "Global.h"
#include "Unit.h"

void Unit::Draw()
{
	DrawTextureRec(*sprite::pawn, { 0,0, 16,-16 }, { (pos.x * spce::scrn::g_tileWidth), spce::scrn::g_boardWidth - spce::scrn::g_tileWidth - (pos.y * spce::scrn::g_tileWidth) }, WHITE);
}

void Unit::Move(Vector2 newPos)
{
	pos.x = newPos.x;
	pos.y = newPos.y;
}

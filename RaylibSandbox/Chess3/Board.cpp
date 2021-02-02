#include <raylib.h>
#include "Board.h"

void Board::Draw()
{
	ClearBackground(BLACK);

	BeginShaderMode(boardShader); {

		DrawTextureEx(missingTexture, { 0,0 }, 0.0f, (float)spce::scrn::outp::g_oboardWidth * 0.5f/*missing.png is 2 pixels wide*/, WHITE);

	} EndShaderMode();

	BeginTextureMode(unitsBuffer); {

		BeginShaderMode(blackUnitShdr); {



		} EndShaderMode();

	} EndTextureMode();

	DrawTextureEx(unitsBuffer.texture, { 0,0 }, 0.0f, spce::scrn::g_gameScale, WHITE);
}
Board g_board;
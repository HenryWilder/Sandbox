#include <raylib.h>
#include "Global.h"
#include "Unit.h"
#include "Board.h"

int main()
{
	int windowWidth = 1280;
	int windowHeight = 720;
	InitWindow(windowWidth, windowHeight, "Experimental");

	// Init vars
	// -------------------------
	Shader boardShader = LoadShader(NULL, "board.frag");
	Shader blackUnitShdr = LoadShader(NULL, "unit_black.frag");
	Shader whiteUnitShdr = LoadShader(NULL, "unit_white.frag");

	Texture2D missingTexture = LoadTexture("missing.png");


	RenderTexture2D unitsBuffer = LoadRenderTexture(spce::scrn::g_boardWidth, spce::scrn::g_boardWidth);

	Board::s_boardShader = &boardShader;
	Board::s_blackUnitShdr = &blackUnitShdr;
	Board::s_whiteUnitShdr = &whiteUnitShdr;
	Board::s_unitsBuffer = &unitsBuffer;
	sprite::missing = &missingTexture;

	Texture2D spriteSet[] = {
		LoadTexture("pawn.png"),
		LoadTexture("rook.png"),
		LoadTexture("knight.png"),
		LoadTexture("bishop.png"),
		LoadTexture("queen.png"),
		LoadTexture("king.png"),
	};
	{
		int i = 0;
		sprite::pawn	= (spriteSet + i++);
		sprite::rook	= (spriteSet + i++);
		sprite::knight	= (spriteSet + i++);
		sprite::bishop	= (spriteSet + i++);
		sprite::queen	= (spriteSet + i++);
		sprite::king	= (spriteSet + i);
	}

	Vector2 cursorPos = {};

	Unit* selected = nullptr;

	// Gameloop
	// -------------------------
	SetTargetFPS(60);
	while (!WindowShouldClose())
	{
		// Update simulation
		// ---------------------
		cursorPos = GetMousePosition();

		// Begin drawing
		// ---------------------
		BeginDrawing(); {

			ClearBackground(BLACK);
			g_board.Draw();
			if (selected)
			{
				selected->Draw();
			}

			QuadTreeNode* hoveredNode = g_board.m_qTree->TraceChild({ cursorPos.x / spce::scrn::outp::g_otileWidth, cursorPos.y / spce::scrn::outp::g_otileWidth });
			if (hoveredNode->Weight() == 1) DrawCircle(spce::scrn::outp::g_oboardWidth, spce::scrn::outp::g_oboardWidth, spce::scrn::outp::g_otileWidth * 0.5f, GREEN);

		} EndDrawing();

		// Prepare for next frame
		// ---------------------
	}

	// Unload and free memory
	// -------------------------
	UnloadShader(boardShader);
	UnloadShader(blackUnitShdr);
	UnloadShader(whiteUnitShdr);

	UnloadTexture(*sprite::missing);
	UnloadRenderTexture(unitsBuffer);

	for (int i = 0; i < 6; ++i) { UnloadTexture(spriteSet[i]); }

	CloseWindow();
	return 0;
}
#include <raylib.h>
#include "Global.h"
#include "Unit.h"

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

	// Gameloop
	// -------------------------
	SetTargetFPS(60);
	while (!WindowShouldClose())
	{
		// Update simulation
		// ---------------------

		// Begin drawing
		// ---------------------
		BeginDrawing(); {

			

		} EndDrawing();

		// Prepare for next frame
		// ---------------------
	}

	// Unload and free memory
	// -------------------------
	UnloadShader(boardShader);
	UnloadShader(blackUnitShdr);
	UnloadShader(whiteUnitShdr);

	UnloadTexture(missingTexture);
	UnloadRenderTexture(unitsBuffer);

	for (int i = 0; i < 6; ++i) { UnloadTexture(spriteSet[i]); }

	CloseWindow();
	return 0;
}
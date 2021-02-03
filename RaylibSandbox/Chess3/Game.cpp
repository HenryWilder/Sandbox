#include <raylib.h>
#include <raymath.h>
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
	Shader selectedUnitShdr = LoadShader(NULL, "unit_selected.frag");

	Texture2D missingTexture = LoadTexture("missing.png");


	RenderTexture2D unitsBuffer = LoadRenderTexture(spce::scrn::g_boardWidth, spce::scrn::g_boardWidth);

	Board::s_boardShader = &boardShader;
	Board::s_blackUnitShdr = &blackUnitShdr;
	Board::s_whiteUnitShdr = &whiteUnitShdr;
	Board::s_selectedUnitShdr = &selectedUnitShdr;
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
		sprite::pawn = (spriteSet + i++);
		sprite::rook = (spriteSet + i++);
		sprite::knight = (spriteSet + i++);
		sprite::bishop = (spriteSet + i++);
		sprite::queen = (spriteSet + i++);
		sprite::king = (spriteSet + i);
	}

	Vector2 cursorPos = {};

	// Gameloop
	// -------------------------
	SetTargetFPS(60);
	while (!WindowShouldClose())
	{
		// Update simulation
		// ---------------------
		cursorPos = GetMousePosition();
		cursorPos = { cursorPos.x / spce::scrn::outp::g_otileWidth, cursorPos.y / spce::scrn::outp::g_otileWidth };

		QTNode* hoveredNode = g_board.m_qTree->TraceDeep(cursorPos);
		if (hoveredNode->Weight())
		{
			DrawCircle(spce::scrn::outp::g_oboardWidth, spce::scrn::outp::g_oboardWidth, spce::scrn::outp::g_otileWidth * 0.5f, GREEN);
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
			{
				float shortestDist = FLT_MAX;
				Unit* closest = nullptr;
				for (Unit* unit : hoveredNode->units)
				{
					float dist = Vector2Distance(cursorPos, unit->GetPos());
					if (dist < shortestDist)
					{
						shortestDist = dist;
						closest = unit;
					}
				}
				if (shortestDist < 1.0f)
				{
					g_board.s_selected = closest;
					g_board.m_qTree->Eliminate(closest);
				}
			}
		}
		if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && g_board.s_selected) // Refresh
		{
			hoveredNode->units.push_back(g_board.s_selected);
			g_board.m_qTree->Subdivide();
			g_board.m_qTree->Prune();
			g_board.s_selected = nullptr;
		}

		if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && g_board.s_selected)
		{
			if (g_board.s_selected) g_board.s_selected->Move(cursorPos);
		}

		// Begin drawing
		// ---------------------
		BeginDrawing(); {

			ClearBackground(BLACK);
			g_board.Draw();
			g_board.m_qTree->DrawDebug(g_board.m_qTree->MaxDepth() + 1);
			{
				Rectangle rec = hoveredNode->coverage;
				rec.x *= spce::scrn::outp::g_otileWidth;
				rec.y *= spce::scrn::outp::g_otileWidth;
				rec.width *= spce::scrn::outp::g_otileWidth;
				rec.height *= spce::scrn::outp::g_otileWidth;
				DrawRectangleRec(rec, ColorAlpha(GREEN, 0.5));
			}
			//g_board.m_qTree->TraceDeep(cursorPos);

		} EndDrawing();

		// Prepare for next frame
		// ---------------------
	}

	// Unload and free memory
	// -------------------------
	UnloadShader(boardShader);
	UnloadShader(blackUnitShdr);
	UnloadShader(whiteUnitShdr);
	UnloadShader(selectedUnitShdr);

	UnloadTexture(*sprite::missing);
	UnloadRenderTexture(unitsBuffer);

	for (int i = 0; i < 6; ++i) { UnloadTexture(spriteSet[i]); }

	CloseWindow();
	return 0;
}
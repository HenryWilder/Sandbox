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
	SetTargetFPS(60);

#pragma region Declare/Init Vars
	// Init vars
	// -------------------------
	Board::s_boardShader = LoadShader(NULL, "board.frag");
	Board::s_blackUnitShdr = LoadShader(NULL, "unit_black.frag");
	Board::s_whiteUnitShdr = LoadShader(NULL, "unit_white.frag");
	Board::s_selectedUnitShdr = LoadShader(NULL, "unit_selected.frag");
	Board::s_unitsBuffer = LoadRenderTexture(spce::s::g_boardWidth, spce::s::g_boardWidth);
	sprite::missing = LoadTexture("missing.png");

	sprite::pawn = LoadTexture("pawn.png");
	sprite::rook = LoadTexture("rook.png");
	sprite::knight = LoadTexture("knight.png");
	sprite::bishop = LoadTexture("bishop.png");
	sprite::queen = LoadTexture("queen.png");
	sprite::king = LoadTexture("king.png");

	Vector2 cursorPos = {};

#pragma endregion

#pragma region Gameloop
	// Gameloop
	// -------------------------
	while (!WindowShouldClose())
	{
#pragma region Simulation
		// Update simulation
		// ---------------------
		cursorPos = ScreenToBoard(GetMousePosition());

		QTNode* hoveredNode = g_board.m_qTree->TraceDeep(cursorPos);
		if (hoveredNode->Weight())
		{
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
					g_board.m_selected = closest;
					g_board.m_qTree->Eliminate(closest);
				}
			}
		}
		if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && g_board.m_selected) // Refresh
		{
			hoveredNode->units.push_back(g_board.m_selected);
			g_board.m_qTree->Subdivide();
			g_board.m_qTree->Prune();
			g_board.m_selected = nullptr;
		}

		if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && g_board.m_selected)
		{
			if (g_board.m_selected) g_board.m_selected->Move(cursorPos);
		}
#pragma endregion
		
#pragma region Drawing
		// Begin drawing
		// ---------------------
		BeginDrawing(); {

			ClearBackground(BLACK);
			g_board.Draw();
			g_board.m_qTree->DrawDebug(g_board.m_qTree->MaxDepth() + 1);
			{
				DrawRectangleRec(BoardToScreen(hoveredNode->coverage), ColorAlpha(GREEN, 0.5));
				DrawText(FormatText("%i Units", hoveredNode->units.size()), spce::s::o::g_oBoardWidth, 0, 8, WHITE);
			}
			//g_board.m_qTree->TraceDeep(cursorPos);

		} EndDrawing();
#pragma endregion
	}
#pragma endregion

#pragma region Cleanup
	// Unload and free memory
	// -------------------------

	// Shaders
	UnloadShader(Board::s_boardShader);
	UnloadShader(Board::s_blackUnitShdr);
	UnloadShader(Board::s_whiteUnitShdr);
	UnloadShader(Board::s_selectedUnitShdr);
	// Render textures
	UnloadRenderTexture(Board::s_unitsBuffer);
	// Sprites
	UnloadTexture(sprite::missing);
	UnloadTexture(sprite::pawn);
	UnloadTexture(sprite::rook);
	UnloadTexture(sprite::knight);
	UnloadTexture(sprite::bishop);
	UnloadTexture(sprite::queen);
	UnloadTexture(sprite::king);

	CloseWindow();
#pragma endregion
	return 0;
}
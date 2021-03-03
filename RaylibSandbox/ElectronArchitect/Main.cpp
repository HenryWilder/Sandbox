#include <raylib.h>
#include <raymath.h>
#include "Logic.h"
#include <bitset>
#include <iostream>
#include <string>

float Round(float f) {
	if (f >= 0.0f)
		return (float)((int)(f + 0.5f));
	else
		return (float)((int)(f - 0.5f));
}

Vector2 Vector2Round(Vector2 vec) {
	return Vector2{ Round(vec.x), Round(vec.y) };
}

Vector2 Vector2Snap(Vector2 vec, float to) {
	return Vector2Scale(Vector2Round(Vector2Scale(vec, 1.0f / to)), to);
}

int main()
{
	int windowWidth = 1280;
	int windowHeight = 720;
	InitWindow(windowWidth, windowHeight,"Electron Architect");
	SetTargetFPS(30);

	Camera2D camera;
	camera.offset = { };
	camera.target = { };
	camera.rotation = 0.0f;
	camera.zoom = 8.0f;

	Vector2 cameraPos = { (float)windowWidth * 0.5f, (float)windowHeight * 0.5f };

	float depth = 0.0f;

	auto DepthColor = [&](Color color, float z) {
		return ColorAlpha(color, 1.0f - ((abs(depth - z) * camera.zoom)) / 32);
	};

	Vector2 mousePos{};
	Vector2 worldMousePos{};
	Vector2 mousePos_last{};
	Vector2 worldMousePos_last{};

	Rectangle worldPixel;
	auto UpdateWorldPixel = [&] {
		Vector2 pos = GetScreenToWorld2D({ 0, 0 }, camera);
		Vector2 width = Vector2Subtract(GetScreenToWorld2D({ 0, 1 }, camera), GetScreenToWorld2D({ 0, 0 }, camera));

		worldPixel.x = pos.x;
		worldPixel.y = pos.y;
		worldPixel.width = width.y;
		worldPixel.height = width.y;
	};

	while (!WindowShouldClose())
	{
		#pragma region Simulate

		mousePos_last = mousePos;
		mousePos = GetMousePosition();

		worldMousePos_last = worldMousePos;
		worldMousePos = GetScreenToWorld2D(mousePos, camera);
		
		if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON))
		{
			Vector2 mouse_delta = Vector2Subtract(mousePos, mousePos_last);
			Vector2 worldMouse_delta = Vector2Subtract(worldMousePos, worldMousePos_last);

			cameraPos = Vector2Add(cameraPos, mouse_delta);
		}
		if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON))
		{
			cameraPos = Vector2Snap(cameraPos, camera.zoom);
		}

		if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
			if (GetMouseWheelMove() > 0.0f) camera.zoom *= 2.0f;
			if (GetMouseWheelMove() < 0.0f) camera.zoom *= 0.5f;
		}
		else depth += GetMouseWheelMove();

#pragma endregion

#pragma region Draw

		BeginDrawing(); {

			camera.offset = Vector2Snap(cameraPos, camera.zoom);
			UpdateWorldPixel();

			ClearBackground(BLACK);

			BeginMode2D(camera); {

				{
					float screenRight = worldPixel.x + (worldPixel.width * windowWidth);
					float screenBottom = worldPixel.y + (worldPixel.height * windowHeight);

					for (float y = worldPixel.y; y <= screenBottom; y += (worldPixel.height * camera.zoom))
					{
						DrawLineV({ worldPixel.x, y }, { screenRight, y }, DARKGRAY);
					}
					for (float x = worldPixel.x; x <= screenRight; x += (worldPixel.width * camera.zoom))
					{
						DrawLineV({ x, worldPixel.y }, { x, screenBottom }, DARKGRAY);
					}
				}

				DrawLineV({ 0, 0 }, { 10, 0 }, DepthColor(RED, 0.0f));
				DrawLineV({ 0, 0 }, { 0, -10 }, DepthColor(GREEN, 0.0f));

				srand(0);
				for (int i = 0; i < 256; ++i) {
					DrawCircleV({ (float)(rand() % 128 - 64), (float)(rand() % 128 - 64) }, 1.0f, DepthColor(WHITE, (float)(rand() % 16 - 8)));
				}

			} EndMode2D();

			//DrawLineV(mousePos_last, mousePos, WHITE);

			DrawText(TextFormat("{ %f, %f, %f }", camera.offset.x, camera.offset.y, depth), mousePos.x, mousePos.y, 8, WHITE);
			DrawText(TextFormat("{ %f, %f }", worldMousePos.x, worldMousePos.y), mousePos.x, mousePos.y + 10, 8, WHITE);
			DrawText(TextFormat("%f", camera.zoom), mousePos.x, mousePos.y + 20, 8, WHITE);

		} EndDrawing();

		#pragma endregion
	}

	CloseWindow();
}
	
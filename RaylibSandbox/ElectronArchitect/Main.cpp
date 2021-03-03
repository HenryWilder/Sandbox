#include <bitset>
#include <iostream>
#include <string>

#include <raylib.h>
#include <raymath.h>

#include "Logic.h"

#include "Containers.h"
#include "Main.h"

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

	auto DepthColorLambda = [&](Color color, float z) {
		return DepthColor(color, z, depth, camera.zoom);
	};
	auto DrawWireLambda = [&](Vector2 start, Vector2 end, WireShape shape, bool state, float z) {
		DrawWire(start,
				 end,
				 shape,
				 state,
				 z,
				 depth,
				 camera.zoom
		);
	};

	Vector2 mousePos{};
	Vector2 worldMousePos{};
	Vector2 mousePos_last{};
	Vector2 worldMousePos_last{};

	Vector2 wireStart{};
	Vector2 wireEnd{};
	WireShape wireShape = WireShape::XFirst;

	Rectangle worldPixel;
	auto UpdateWorldPixel = [&] {
		Vector2 pos = GetScreenToWorld2D({ 0, 0 }, camera);
		Vector2 width = Vector2Subtract(GetScreenToWorld2D({ 0, 1 }, camera), GetScreenToWorld2D({ 0, 0 }, camera));

		worldPixel.x = pos.x;
		worldPixel.y = pos.y;
		worldPixel.width = width.y;
		worldPixel.height = width.y;
	};

	VoxelArray world;

	while (!WindowShouldClose())
	{
		#pragma region Simulate

		mousePos_last = mousePos;
		mousePos = GetMousePosition();

		worldMousePos_last = worldMousePos;
		worldMousePos = GetScreenToWorld2D(mousePos, camera);
		
		if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
			Vector2 mouse_delta = Vector2Subtract(mousePos, mousePos_last);
			Vector2 worldMouse_delta = Vector2Subtract(worldMousePos, worldMousePos_last);

			cameraPos = Vector2Add(cameraPos, mouse_delta);
		}
		if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON)) {
			cameraPos = Vector2Snap(cameraPos, camera.zoom);
		}

		if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
			if (GetMouseWheelMove() > 0.0f)
				camera.zoom *= 2.0f;
			else if (GetMouseWheelMove() < 0.0f)
				camera.zoom *= 0.5f;
		}
		else depth += GetMouseWheelMove();

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			wireStart = Vector2Snap(worldMousePos, 1.0f);
		}
		if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
			wireEnd = Vector2Snap(worldMousePos, 1.0f);
		}
		if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
			world.push(Gate());
		}

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

					for (float y = worldPixel.y; y <= screenBottom; y += (worldPixel.height * camera.zoom)) {
						DrawLineV({ worldPixel.x, y }, { screenRight, y }, {20,20,20,255});
					}
					for (float x = worldPixel.x; x <= screenRight; x += (worldPixel.width * camera.zoom)) {
						DrawLineV({ x, worldPixel.y }, { x, screenBottom }, {20,20,20,255});
					}
				}

				if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
					DrawWireLambda(wireStart, wireEnd, wireShape, false, depth);
				}

			} EndMode2D();

			//DrawLineV(mousePos_last, mousePos, WHITE);

			DrawText(TextFormat("{ %i, %i, %i }", (int)Round(worldMousePos.x), (int)Round(worldMousePos.y), (int)Round(depth)), 0, 0, 8, WHITE);
			//DrawText(TextFormat("{ %f, %f }", worldMousePos.x, worldMousePos.y), mousePos.x, mousePos.y + 10, 8, WHITE);
			//DrawText(TextFormat("%f", camera.zoom), mousePos.x, mousePos.y + 20, 8, WHITE);

		} EndDrawing();

		#pragma endregion
	}

	CloseWindow();
}

int Log2(int val) {
	int steps = 0;
	while (val) {
		val /= 2;
		++steps;
	}
	return steps;
}

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

Color DepthColor(Color color, float z, float target, float zoom) {
	return ColorAlpha(color, 1.0f - ((abs(target - z) * zoom)) / 32);
};

void DrawWire(Vector2 start, Vector2 end, WireShape shape, bool b_state, float depth, float targetDepth, float zoom) {
	Color color = DepthColor((b_state ? RED : WHITE), depth, targetDepth, zoom);
	float thick = (float)Log2(Round(targetDepth) - Round(depth));

	Vector2 joint;

	if (shape == WireShape::DiagFirst || shape == WireShape::DiagLast)
	{
		float xLength = abs(end.x - start.x);
		float yLength = abs(end.y - start.y);

		float& shortLength = (xLength < yLength ? xLength : yLength);

		Vector2 vectorToEnd = {
			(start.x < end.x ? 1.0f : -1.0f),
			(start.y < end.y ? 1.0f : -1.0f),
		};

		if (shape == WireShape::DiagFirst)
			joint = Vector2Add(start, Vector2Scale(vectorToEnd, shortLength));
		else if (shape == WireShape::DiagLast)
			joint = Vector2Subtract(end, Vector2Scale(vectorToEnd, shortLength));
	}
	else if (shape == WireShape::XFirst || shape == WireShape::YFirst) 
	{
		if (shape == WireShape::XFirst)
			joint = { end.x, start.y };
		else if (shape == WireShape::YFirst)
			joint = { start.x, end.y };
	}
	else joint = start;

	DrawLineV(start, joint, color);
	DrawLineV(joint, end, color);

}
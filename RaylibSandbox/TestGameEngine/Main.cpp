#include <raylib.h>
#include "BaseTypes.h"

int main()
{
	int windowWidth = 1280;
	int windowHeight = 720;

	const char text[] = "Test successful!";

	Camera2D camera;
	camera.zoom = 1.0f;
	camera.offset = Vector2{ 0.0, 0.0 };
	camera.target = Vector2{ 0.0, 0.0 };
	camera.rotation = 0.0f;

	Texture2D;

	InitWindow(windowWidth, windowHeight, "Experimental");
	SetTargetFPS(60);

	while (!WindowShouldClose())
	{
		BeginDrawing();

		ClearBackground(RAYWHITE);

		DrawText(text, (windowWidth - ((48 / 2) * sizeof(text))) / 2, (windowHeight - 48) / 2, 48, BLACK);

		BeginMode2D(camera);

		DrawTextureEx();

		EndMode2D();

		EndDrawing();
	}
	CloseWindow();

	return 0;
}
#include <raylib.h>
#include "random.h"

void Tick()
{
	// TODO: Update vars
}

void Frame()
{
	// TODO: Rendering
}

void GameLoop()
{
	while (!WindowShouldClose())
	{
		Tick();

		BeginDrawing();
		{
			Frame();
		}
		EndDrawing();
	}
}

int main()
{
	int windowWidth = 1920;
	int windowHeight = 1080;
	InitWindow(windowWidth, windowHeight, "Five Nights at Freddy's");
	SetTargetFPS(60);

	GameLoop();

	CloseWindow();

	return 0;
}
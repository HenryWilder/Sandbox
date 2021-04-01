#include <raylib.h>
#include "Templates.h"
#include <tuple>
#include <stack>

struct Space {
	bool visited;
};

int main()
{
	return 0;
	int windowWidth = 1280;
	int windowHeight = 720;
	InitWindow(windowWidth, windowHeight, "Pathfinder");

	Space spaces[8 * 8];

	while (!WindowShouldClose())
	{

		BeginDrawing(); {

			for (int y = 0; y < 8; ++y)
			{
				for (int x = 0; x < 8; ++x)
				{
					spaces[(y * 8) + x];
				}
			}

		} EndDrawing();
	}

	CloseWindow();
	return 0;
}
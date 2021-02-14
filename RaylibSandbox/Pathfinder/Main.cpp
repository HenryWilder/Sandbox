#include <raylib.h>
#include "Templates.h"
#include <tuple>

int main()
{
	return 0;
	int windowWidth = 1280;
	int windowHeight = 720;
	InitWindow(windowWidth, windowHeight, "Pathfinder");

	Array2D<char, 3, 3> test({
		'a','b','c',
		'd','e','f',
		'g','h','i'}
	);

	printf("\nStarting test...\n");
	for (char& c : test)
	{
		printf("%c", c);
	}
	printf("\nTest complete.\n");

	while (!WindowShouldClose())
	{
	}

	CloseWindow();
	return 0;
}
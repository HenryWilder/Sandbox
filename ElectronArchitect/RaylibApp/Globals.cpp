#include <raylib.h>
#include "CustomTypes.h"
#include "Globals.h"

const int g_windowWidth = 1280;
const int g_windowHeight = 720;
const int g_gridSize = 1;
Int2 cursorPos = Int2(-100);

Vector2 Vector2InvertY(Vector2 vector) {
	return Vector2{ vector.x, g_windowHeight - vector.y };
}

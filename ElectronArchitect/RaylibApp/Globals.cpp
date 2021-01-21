#include "Globals.h";

const int g_windowWidth = 1280;
const int g_windowHeight = 720;
const float g_gridSize = 10.0f;
Vector2 cursorPos = { -100.0f, -100.0f };

Vector2 Vector2InvertY(Vector2 vector) {
	return Vector2{ vector.x, g_windowHeight - vector.y };
}

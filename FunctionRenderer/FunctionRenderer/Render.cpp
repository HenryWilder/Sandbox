#include "Render.h"


// Get a console handle
HWND window = GetConsoleWindow();
// Get a handle to device context
HDC hdc = GetDC(window);

bool DrawPixel(int x, int y, ColorRGB color)
{
	return SetPixelV(hdc, x, y, RGBC(color));
}

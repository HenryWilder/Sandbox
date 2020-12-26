#include "Rendering.h"

HDC hConsoleDC = GetDC(GetConsoleWindow());

void DrawPixel(POINT pixel, COLORREF color)
{
	SetPixelV(hConsoleDC, pixel.x, pixel.y, color);
}

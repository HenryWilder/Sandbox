#include "Input.h"

void UpdateScreencap()
{
	BitBlt(g_hInternal, 0, 0, g_screenWidth, g_screenHeight, g_hDesktop, 0, 0, SRCCOPY);

	BITMAPINFOHEADER bmi = { 0 };
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biPlanes = 1;
	bmi.biBitCount = 32;
	bmi.biWidth = g_screenWidth;
	bmi.biHeight = -g_screenHeight;
	bmi.biCompression = BI_RGB;
	bmi.biSizeImage = 0; // 3 * ScreenX * ScreenY; (position, not size)

	GetDIBits(g_hDesktop, g_hBitmap, 0, g_screenHeight, g_screenData, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);
}

unsigned long PixelIndex(long x, long y) { return 4u * (unsigned)((y * (long)g_screenWidth) + x); }

Color GetPixelColor(long x, long y)
{
	unsigned long index = PixelIndex(x, y);

	Color output = {
		g_screenData[index + 2u],	// Red
		g_screenData[index + 1u],	// Green
		g_screenData[index] };		// Blue

	/*
	SetPixel(g_hDesktop, x - 1, y - 1, RGB(255, 0, 255));
	SetPixel(g_hDesktop, x, y - 1, RGB(255, 0, 255));
	SetPixel(g_hDesktop, x + 1, y - 1, RGB(255, 0, 255));
	SetPixel(g_hDesktop, x - 1, y, RGB(255, 0, 255));
	SetPixel(g_hDesktop, x, y, RGB(255, 0, 255));
	SetPixel(g_hDesktop, x + 1, y, RGB(255, 0, 255));
	SetPixel(g_hDesktop, x - 1, y + 1, RGB(255, 0, 255));
	SetPixel(g_hDesktop, x, y + 1, RGB(255, 0, 255));
	SetPixel(g_hDesktop, x + 1, y + 1, RGB(255, 0, 255));
	*/

	return output;
}

Color GetPixelColor(POINT pos) { return GetPixelColor(pos.x, pos.y); }

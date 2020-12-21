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
	bmi.biSizeImage = 0; // 3 * ScreenX * ScreenY;

	if (g_screenData) free(g_screenData); // Empty screen data

	g_screenData = (BYTE*)malloc(4 * (unsigned long long)g_screenWidth * (unsigned long long)g_screenHeight);

	GetDIBits(g_hInternal, g_hBitmap, 0, g_screenHeight, g_screenData, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);
}

unsigned long PixelIndex(long x, long y)
{
	return 4u * (unsigned)((y * (long)g_screenWidth) + x);
}

Color GetPixelColor(long x, long y)
{
	unsigned long index = PixelIndex(x, y);
	return { g_screenData[index + 2u],	// Red
		g_screenData[index + 1u],	// Green
		g_screenData[index] };		// Blue
}

Color GetPixelColor(POINT pos)
{
	return GetPixelColor(pos.x, pos.y);
}

void SetPixelColor(long x, long y, Color color, BYTE* data)
{
	unsigned long index = PixelIndex(x, y);
	data[index + 2u] = color.r;	// Red
	data[index + 1u] = color.g;	// Green
	data[index] = color.b;		// Blue
}
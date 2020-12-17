#include <iostream>
#include <Windows.h>

// Get a console handle
HWND window = GetConsoleWindow();

// Get a handle to device hdc
HDC hConsole = GetDC(window);

BYTE* g_screenData = 0;

int g_screenWidth = 0;
int g_screenHeight = 0;

struct Color
{
	unsigned char r, g, b;

	unsigned char Gray()
	{
		return (unsigned char)(((int)r + (int)g + (int)b) / 3);
	}
};

Color GetPixelColor(int x, int y)
{
	unsigned long long pos = 4 * ((y * (unsigned long long)g_screenWidth) + x);
	return { g_screenData[pos + 2], g_screenData[pos + 1], g_screenData[pos] };
}

struct Pixel
{
	Pixel() : color{ 0,0,0 }, x{ 0 }, y{ 0 } {};
	Pixel(Color _color, int _x, int _y) : color{ _color }, x{ _x }, y{ _y } {};

	Color color;
	int x;
	int y;
};

void SetPixelColor(int x, int y, Color color)
{
	unsigned long long pos = 4 * ((y * (unsigned long long)g_screenWidth) + x);
	g_screenData[pos + 2] = color.r; // Red
	g_screenData[pos + 1] = color.g; // Green
	g_screenData[pos] = color.b; // Blue
}

// A section of the screen
// PixelRect<width, height>(startX, startY)
template<short w, short h>
struct PixelRect
{
	PixelRect() {
		int i = 0;
		for (int y = 0; y < h; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				m_frame[i++] = Pixel({ 0,0,0 }, x, y);
			}
		}
	}
	PixelRect(int startX, int startY) {
		int i = 0;
		for (int y = startY; y < startY + h; ++y)
		{
			for (int x = startX; x < startX + w; ++x)
			{
				m_frame[i++] = Pixel(GetPixelColor(x, y), x, y);
			}
		}
	}

	Pixel m_frame[w * h];
};

void PixelProcessing(HDC context)
{
	PixelRect<50, 35> clock = PixelRect<50, 35>((g_screenWidth / 2) - 75, 0);
	for (Pixel pixel : clock.m_frame)
	{
		//if (pixel.color.r > 127)
		SetPixel(context, pixel.x, pixel.y, RGB(pixel.color.r + 0, pixel.color.g + 50, pixel.color.b + 50)); // Draws the pixels (we only want this on a small number of pixels)
	}

	/*
	for (int y = 0; y < g_screenHeight; ++y)
	{
		for (int x = 0; x < g_screenWidth; ++x)
		{
			Color color = GetPixelColor(x, y);
			if (color.Gray() > 127) SetPixel(context, x, y, RGB(0, 255, 255)); // Draws the pixels (we only want this on a small number of pixels)
		}
	}
	*/
}

int main()
{
	HDC hDesktop = GetDC(NULL); // get the desktop device context
	HDC hDest = CreateCompatibleDC(hDesktop); // create a device context to use ourselves

	// get the height and width of the screen.
	g_screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	g_screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);

	// create a bitmap
	HBITMAP hBitmap = CreateCompatibleBitmap(hDesktop, g_screenWidth, g_screenHeight);

	// use the previously created device context with the bitmap
	SelectObject(hDest, hBitmap);

	// copy from the desktop device context to the bitmap device context
	// call this once per 'frame'
	while (true)
	{
		BitBlt(hDest, 0, 0, g_screenWidth, g_screenHeight, hDesktop, 0, 0, SRCCOPY);

		BITMAPINFOHEADER bmi = { 0 };
		bmi.biSize = sizeof(BITMAPINFOHEADER);
		bmi.biPlanes = 1;
		bmi.biBitCount = 32;
		bmi.biWidth = g_screenWidth;
		bmi.biHeight = -g_screenHeight;
		bmi.biCompression = BI_RGB;
		bmi.biSizeImage = 0;// 3 * ScreenX * ScreenY;

		if (g_screenData) free(g_screenData); // Empty screen data

		g_screenData = (BYTE*)malloc(4 * (unsigned long long)g_screenWidth * (unsigned long long)g_screenHeight);

		GetDIBits(hDest, hBitmap, 0, g_screenHeight, g_screenData, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);

		PixelProcessing(hDest);

		BitBlt(hConsole, 0, 0, g_screenWidth, g_screenHeight, hDest, 0, 0, SRCCOPY); // Render output to console window

		Sleep(1); // How long we should wait before the next snapshot
	}

	// after the recording is done, release the desktop context you got..
	ReleaseDC(NULL, hDesktop);

	// ..delete the bitmap you were using to capture frames..
	DeleteObject(hBitmap);

	// ..and delete the context you created
	DeleteDC(hDest);

	std::cin.ignore();
	return 0;
}
#include <iostream>
#include <Windows.h>

// Get a console handle
HWND window = GetConsoleWindow();

// Get a handle to device context
HDC hdc = GetDC(window);

void DrawPixel(int x, int y, unsigned char color)
{
	for (int xOff = 0; xOff < 8; ++xOff)
	{
		for (int yOff = 0; yOff < 8; ++yOff)
		{
			SetPixel(hdc, (x * 8) + xOff, (y * 8) + yOff, RGB(color * 32, color * 32, color * 32));
		}
	}
}

int main()
{
	const int width = 16;
	const unsigned char colorWidth = 8;

	for (int i = 0; i < width * colorWidth; ++i)
	{
		for (int x = 0; x < width; ++x)
		{
			for (unsigned char color = 0u; color < colorWidth; ++color)
			{
				DrawPixel(x, 0, RGB(color * (256u / colorWidth), color * (256u / colorWidth), color * (256u / colorWidth)));

				Sleep(16);
			}
		}
	}

	std::cin.ignore();
	return 0;
}
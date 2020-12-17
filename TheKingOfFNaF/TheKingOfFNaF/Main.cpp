#include <iostream>
#include <Windows.h>
#include "CustomTypes.h"

// Get a console handle
HWND window = GetConsoleWindow();

// Get a handle to device hdc
HDC hConsole = GetDC(window);

BYTE* g_screenData = 0;

int g_screenWidth = 0;
int g_screenHeight = 0;

ClockTime g_time = ClockTime();

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

void SetPixelColor(long x, long y, Color color, BYTE* data = g_screenData)
{
	unsigned long index = PixelIndex(x, y);
	data[index + 2u] = color.r;	// Red
	data[index + 1u] = color.g;	// Green
	data[index] = color.b;		// Blue
}

enum class NumberScale
{
	Small = 0,
	Norml = 1,
	Large = 2,
};

// Input should be top-left corner of the number followed by the size
char ReadNumber(int x, int y, NumberScale size
#ifdef _DEBUG
	, HDC context
#endif
)
{
	enum Sample : unsigned short
	{
		Top =		0b000000001,
		Middle =	0b000000010,
		Left =		0b000000100,
		Right =		0b000001000,
		BottomL =	0b000010000,
		BottomC =	0b000100000,
		BottomR =	0b001000000,
		TopL =		0b010000000,
		TopR =		0b100000000,
	};
	Sample sampleIDs[] = { Top, Middle, Left, Right, BottomL, BottomC, BottomR, TopL, TopR };
	POINT sampleOffsets[] = { {5,0}, {5,8}, {0,8}, {10,8}, {0,12}, {5,12}, {10,12}, {0,7}, {10,7} };
	enum SamplesToNumbers : unsigned short
	{
		_n1 = 0b000100011,
		_n2 = 0b001110011,
		_n3 = 0b000100001,
		_n4 = 0b000001110,
		_n5 = 0b000101001,
		_n6 = 0b010101101,
		_n7 = 0b000000011,
		_n8 = 0b000101101,
		_n9 = 0b100100001,
		_n0 = 0b110101101,
	};
	SamplesToNumbers values[] = { _n0,_n1,_n2,_n3,_n4,_n5,_n6,_n7,_n8,_n9 };

	int guess = 0b000000000;
	unsigned char threshold = 63;

	for (int sample = 0; sample < 9; ++sample)
	{
		if (GetPixelColor(x + sampleOffsets[sample].x, y + sampleOffsets[sample].y).Gray() > threshold) guess |= sampleIDs[sample];
#ifdef _DEBUG
		if ((guess & sampleIDs[sample]) == sampleIDs[sample]) SetPixel(context, x + sampleOffsets[sample].x, y + sampleOffsets[sample].y, RGB(255, 0, 0));
		else SetPixel(context, x + sampleOffsets[sample].x, y + sampleOffsets[sample].y, RGB(0, 0, 255));
#endif
	}

	for (char guessValue = 0; guessValue < 10; ++guessValue)
	{
		if (guess == values[guessValue]) return guessValue;
	}
	return '!'; // Error
}

// Run this about once every frame
void ReadGameClock(HDC context)
{
	std::cout << ReadNumber(1873, 85, NumberScale::Small, context); // Deciseconds
	std::cout << ReadNumber(1849, 85, NumberScale::Small, context); // Seconds (ones)
	std::cout << ReadNumber(1832, 85, NumberScale::Small, context); // Seconds (tens)
	std::cout << ReadNumber(1807, 85, NumberScale::Small, context); // Minutes
	std::cout << '\n';

	/*
	int clockWidth = 85;
	int clockHeight = 65;
	int clockOffsetX = g_screenWidth / 2 - clockWidth - 30;
	int clockOffsetY = 35;
	for (int y = clockOffsetY; y < clockOffsetY + clockHeight; ++y)
	{
		for (int x = clockOffsetX; x < clockOffsetX + clockWidth; ++x)
		{
			Color color = GetPixelColor(x, y);
			//if (color.Gray() > 63)
			{
#ifdef _DEBUG
				SetPixel(context, x, y, RGB(255, 0, 0)); // Draws the pixels (we only want this on a small number of pixels)
#endif
			}
		}
	}
	*/

	//g_time.IncrementTime();
}

void PingGameState(HDC context)
{
	ReadGameClock(context);
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
		bmi.biSizeImage = 0; // 3 * ScreenX * ScreenY;

		if (g_screenData) free(g_screenData); // Empty screen data

		g_screenData = (BYTE*)malloc(4 * (unsigned long long)g_screenWidth * (unsigned long long)g_screenHeight);

		GetDIBits(hDest, hBitmap, 0, g_screenHeight, g_screenData, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);

		//PingGameState(hDest);
		PingGameState(hDesktop);

#ifdef _DEBUG
		//BitBlt(hConsole, 0, 0, g_screenWidth, g_screenHeight, hDest, 0, 0, SRCCOPY); // Render output to console window
#endif

		//Sleep(16); // How long we should wait before the next snapshot
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
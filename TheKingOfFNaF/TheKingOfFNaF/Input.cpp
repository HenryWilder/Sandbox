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

void SetPixelColor(long x, long y, Color color, BYTE* data)
{
	unsigned long index = PixelIndex(x, y);
	data[index + 2u] = color.r;	// Red
	data[index + 1u] = color.g;	// Green
	data[index] = color.b;		// Blue
}


// Input should be top-left corner of the number followed by the size
char ReadNumber(int x, int y, NumberScale size)
{
	enum Sample : unsigned short
	{
		Top = 0b000000001,
		Middle = 0b000000010,
		Left = 0b000000100,
		Right = 0b000001000,
		BottomL = 0b000010000,
		BottomC = 0b000100000,
		BottomR = 0b001000000,
		TopL = 0b010000000,
		TopR = 0b100000000,
	};
	Sample sampleIDs[] = { Top, Middle, Left, Right, BottomL, BottomC, BottomR, TopL, TopR };
	POINT sampleOffsets[] = { { 5,0 },{ 5,8 },{ 0,8 },{ 10,8 },{ 0,12 },{ 5,12 },{ 10,12 },{ 0,7 },{ 10,7 } };
	enum SamplesToNumbers : unsigned short
	{
		_n1 = 0b000100011,
		_n2 = 0b001110011,
		_n3 = 0b000100001,
		_n4 = 0b010001110,
		_n5 = 0b100101001,
		_n6 = 0b010101101,
		_n7 = 0b000000011,
		_n8 = 0b000101101,
		_n9 = 0b100100001,
		_n0 = 0b110101101,
	};
	SamplesToNumbers values[] = { _n0,_n1,_n2,_n3,_n4,_n5,_n6,_n7,_n8,_n9 };

	int guess = 0b000000000;
	unsigned char threshold = 100;

	for (int sample = 0; sample < 9; ++sample)
	{
		if (GetPixelColor(x + sampleOffsets[sample].x, y + sampleOffsets[sample].y).Gray() > threshold) guess |= sampleIDs[sample];
	}

	for (char guessValue = 0; guessValue < 10; ++guessValue)
	{
		if (guess == values[guessValue]) return guessValue;
	}
	return 0; // Error
}

// Run this about once every frame
bool ReadGameClock(HDC context)
{
	int time = (int)ReadNumber(1873, 85, NumberScale::Small); // Deciseconds
	int seconds = (int)ReadNumber(1849, 85, NumberScale::Small); // Seconds (ones)
	int tensOfSeconds = (int)ReadNumber(1832, 85, NumberScale::Small); // Seconds (tens)
	int minute = (int)ReadNumber(1807, 85, NumberScale::Small); // Minutes

	time = time + 10 * seconds + 100 * tensOfSeconds + 600 * minute;

	if (time > g_gameState.m_data.time.GetDeciseconds())
	{
		g_gameState.m_data.time.IncrementTime();
		return true;
	}
	else
	{
		g_gameState.m_data.time.PingWithoutChange();
		return false;
	}
}

void CheckVentsReset(HDC context)
{
	if (GetPixelColor(1580, 1040).RedDev() > 35)
	{
		g_gameState.m_data.ventilationNeedsReset = true;
	}
}

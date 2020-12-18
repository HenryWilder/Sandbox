#pragma once
#include "CustomTypes.h"
#include "Globals.h"

void UpdateScreencap();

unsigned long PixelIndex(long x, long y);

Color GetPixelColor(long x, long y);

void SetPixelColor(long x, long y, Color color, BYTE* data = g_screenData);

enum class NumberScale
{
	Small = 0,
	Norml = 1,
	Large = 2,
};

// Input should be top-left corner of the number followed by the size
char ReadNumber(int x, int y, NumberScale size);

// Run this about once every frame
bool ReadGameClock(HDC context);

void CheckVentsReset(HDC context);
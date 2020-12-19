#pragma once
#include "CustomTypes.h"
#include "Globals.h"

void UpdateScreencap();

unsigned long PixelIndex(long x, long y);

Color GetPixelColor(long x, long y);
Color GetPixelColor(POINT pos);

void SetPixelColor(long x, long y, Color color, BYTE* data = g_screenData);

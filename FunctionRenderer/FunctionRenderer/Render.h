#pragma once
#include "Windows.h"
#define RGBC(c) RGB(c.r,c.g,c.b)
#include "HenryMath.h"

// Get a console handle
extern HWND window;
// Get a handle to device context
extern HDC hdc;

bool DrawPixel(int x, int y, ColorRGB color);
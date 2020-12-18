#pragma once
#include "CustomTypes.h"

// Get a console handle
extern HWND window;

// Get a handle to device hdc
extern HDC hConsole;

extern int g_screenHeight;
extern int g_screenWidth;

extern BYTE* g_screenData;

extern ClockTime g_time;

extern GameState g_gameState;

extern HDC g_hDesktop; // get the desktop device context
extern HDC g_hInternal; // create a device context to use ourselves

// create a bitmap
extern HBITMAP g_hBitmap;
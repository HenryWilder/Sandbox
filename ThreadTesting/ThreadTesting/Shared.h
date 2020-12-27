#pragma once
#include <Windows.h>
#include <thread>
#include <iostream>
#include "Tracking.h"

extern HWND g_wndConsole; // Get a console handle
extern HDC g_hConsoleDC; // Get a handle to device hdc

extern int g_screenHeight;
extern int g_screenWidth;

extern BYTE* g_screenData;

//HWND g_gameWindow = FindWindow(NULL, TEXT("Ultimate Custom Night"));
//HDC g_gameDC = GetDC(g_gameWindow);
extern HDC g_hDesktop; // get the desktop device context
extern HDC g_hInternal; // create a device context to use ourselves

extern HBITMAP g_hBitmap;

extern bool loop;

struct Color
{
	unsigned char r, g, b;

	operator COLORREF() { return RGB(r,g,b);  }
};

void DeclareTrackingVars();

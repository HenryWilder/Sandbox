#pragma once
#include "CustomTypes.h"

/////////////////////////////////////////////////////////////////////////////////////////////
// Here, all of the variables & constants used throughout the project are declared/defined //
/////////////////////////////////////////////////////////////////////////////////////////////

//
// Global variables -- used by everyone, but changeable.
//

// Get a console handle
extern HWND g_wndConsole;

// Get a handle to device hdc
extern HDC g_hConsoleDC;

extern int g_screenHeight;
extern int g_screenWidth;

extern BYTE* g_screenData;

//extern HWND g_gameWindow;
//extern HDC g_gameDC;
extern HDC g_hDesktop; // get the desktop device context
extern HDC g_hInternal; // create a device context to use ourselves

// create a bitmap 
extern HBITMAP g_hBitmap;

#include "Globals.h"

// Get a console handle
HWND window = GetConsoleWindow();

// Get a handle to device hdc
HDC hConsole = GetDC(window);

int g_screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
int g_screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);

BYTE* g_screenData;

ClockTime g_time = ClockTime();

GameState g_gameState; // All the information we have about the state of the game

HDC g_hDesktop = GetDC(NULL); // get the desktop device context
HDC g_hInternal = CreateCompatibleDC(g_hDesktop); // create a device context to use ourselves

HBITMAP g_hBitmap = CreateCompatibleBitmap(g_hDesktop, g_screenWidth, g_screenHeight);
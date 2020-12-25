#include "Globals.h"

HWND g_wndConsole = GetConsoleWindow(); // Get a console handle
HDC g_hConsoleDC = GetDC(g_wndConsole); // Get a handle to device hdc

int g_screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
int g_screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);

BYTE* g_screenData = (BYTE*)malloc(4/*number of channels, maybe?*/ * (unsigned long long)g_screenWidth * (unsigned long long)g_screenHeight);

//HWND g_gameWindow = FindWindow(NULL, TEXT("Ultimate Custom Night"));
//HDC g_gameDC = GetDC(g_gameWindow);
HDC g_hDesktop = GetDC(NULL); // get the desktop device context
HDC g_hInternal = CreateCompatibleDC(g_hDesktop); // create a device context to use ourselves

HBITMAP g_hBitmap = CreateCompatibleBitmap(g_hDesktop, g_screenWidth, g_screenHeight);

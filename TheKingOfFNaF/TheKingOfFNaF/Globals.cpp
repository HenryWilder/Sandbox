#include "Globals.h"

// Get a console handle
HWND window = GetConsoleWindow();

// Get a handle to device hdc
HDC hConsole = GetDC(window);

int g_screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
int g_screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);

BYTE* g_screenData;

GameState g_gameState; // All the information we have about the state of the game

HDC g_hDesktop = GetDC(NULL); // get the desktop device context
HDC g_hInternal = CreateCompatibleDC(g_hDesktop); // create a device context to use ourselves

HBITMAP g_hBitmap = CreateCompatibleBitmap(g_hDesktop, g_screenWidth, g_screenHeight);

const POINT buttonPositions[] = {
		pnt::ofc::mask,
		pnt::cam::resetVent,
		pnt::cam::cam01,
		pnt::cam::cam02,
		pnt::cam::cam03,
		pnt::cam::cam04,
		pnt::cam::cam05,
		pnt::cam::cam06,
		pnt::cam::cam07,
		pnt::cam::cam08,
		{ pnt::cam::sysbX, pnt::cam::cSysY },
		{ pnt::cam::sysbX, pnt::cam::vSysY },
		{ pnt::cam::sysbX, pnt::cam::dSysY },
		pnt::vnt::snrL,
		pnt::vnt::snrT,
		pnt::vnt::snrR,
		pnt::dct::btnL,
		pnt::dct::btnR };

Button CameraButton(int cam)
{
	return Button((int)Button::Cam01 + cam);
}
Button CameraButton(Camera cam)
{
	return CameraButton((int)cam);
}

Button SystemButton(int system)
{
	return Button((int)Button::CameraSystem + system);
}
Button SystemButton(State system)
{
	return SystemButton((int)system);
}

POINT GetButtonPos(Button button)
{
	return buttonPositions[(int)button];
}

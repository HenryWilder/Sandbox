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

POINT GetButtonPos(Button button)
{
	switch (button)
	{
	case Button::Mask:
		return pnt::ofc::mask;
		break;
	case Button::ResetVent:
		return pnt::cam::resetVent;
		break;
	case Button::Cam01:
		return pnt::cam::cam01;
		break;
	case Button::Cam02:
		return pnt::cam::cam02;
		break;
	case Button::Cam03:
		return pnt::cam::cam03;
		break;
	case Button::Cam04:
		return pnt::cam::cam04;
		break;
	case Button::Cam05:
		return pnt::cam::cam05;
		break;
	case Button::Cam06:
		return pnt::cam::cam06;
		break;
	case Button::Cam07:
		return pnt::cam::cam07;
		break;
	case Button::Cam08:
		return pnt::cam::cam08;
		break;
	case Button::CameraSystem:
		return { pnt::cam::sysbX, pnt::cam::cSysY };
		break;
	case Button::VentSystem:
		return { pnt::cam::sysbX, pnt::cam::vSysY };
		break;
	case Button::DuctSystem:
		return { pnt::cam::sysbX, pnt::cam::dSysY };
		break;
	case Button::Snare_Left:
		return pnt::vnt::snrL;
		break;
	case Button::Snare_Top:
		return pnt::vnt::snrT;
		break;
	case Button::Snare_Right:
		return pnt::vnt::snrR;
		break;
	case Button::Duct_Left:
		return pnt::dct::btnL;
		break;
	case Button::Duct_Right:
		return pnt::dct::btnR;
		break;
	default:
		return { 0,0 };
		break;
	}
}

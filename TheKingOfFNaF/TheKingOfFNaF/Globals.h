#pragma once
#include "CustomTypes.h"

// Get a console handle
extern HWND window;

// Get a handle to device hdc
extern HDC hConsole;

extern int g_screenHeight;
extern int g_screenWidth;

extern BYTE* g_screenData;

extern GameState g_gameState;

extern HDC g_hDesktop; // get the desktop device context
extern HDC g_hInternal; // create a device context to use ourselves

// create a bitmap
extern HBITMAP g_hBitmap;

// Important positions on the screen
namespace pnt
{
	// Clock position
	constexpr POINT clk = { 1807, 85 };
	constexpr int clk_10secX = 1832;
	constexpr int clk_secX = 1849;
	constexpr int clk_decisecX = 1873;

	constexpr POINT temp = { 1818, 1012 };

	constexpr POINT coins = { 155, 75 };

	constexpr POINT powr = { 71, 910 };
	constexpr POINT pwrUsg = { 38, 969 };

	constexpr POINT noise = { 38,1020 };

	constexpr POINT openCam = { 1280, 1006 };

	namespace ofc
	{
		constexpr POINT mask = { 682, 1006 };
		constexpr POINT ventWarning = { 1580, 1040 };

		constexpr POINT foxy = { 801, 710 };
	}
	namespace cam
	{
		constexpr POINT ventWarning = { 1563, 892 }; // Location for testing vent warning in the cameras
		constexpr POINT resetVent = { 1700, 915 }; // Where the reset vent button is for clicking

		constexpr POINT cam01 = { 1133, 903 }; // WestHall
		constexpr POINT cam02 = { 1382, 903 }; // EastHall
		constexpr POINT cam03 = { 1067, 825 }; // Closet
		constexpr POINT cam04 = { 1491, 765 }; // Kitchen
		constexpr POINT cam05 = { 1122, 670 }; // PirateCove
		constexpr POINT cam06 = { 1422, 590 }; // ShowtimeStage
		constexpr POINT cam07 = { 1278, 503 }; // PrizeCounter
		constexpr POINT cam08 = { 988, 495 }; // PartsAndServices

		constexpr int sysbX = 1331; // System buttons X position
		constexpr int cSysY = 153; // Cam system button
		constexpr int vSysY = 263; // Vent system button
		constexpr int dSysY = 373; // Duct system button
	}
	namespace vnt
	{
		constexpr POINT snrL = { 548, 645 }; // Left snare
		constexpr POINT snrT = { 650, 536 }; // Top snare
		constexpr POINT snrR = { 747, 645 }; // Right snare
	}
	namespace dct
	{
		constexpr POINT chkL = { 500, 791 }; // Check left duct
		constexpr POINT chkR = { 777, 791 }; // Check right duct
		constexpr POINT btnL = { 331, 844 }; // Left duct button
		constexpr POINT btnR = { 1016, 844 }; // Right duct button
	}
}
namespace clr // Colors
{
	constexpr Color sysButton = { 40, 152, 120 };
	const CNorm sysButtonNrm = sysButton.Normal();
	constexpr Color camButton = { 136, 172, 0 };
	const CNorm camButtonNrm = camButton.Normal();
}

constexpr int camResponseMS = 300; // Time it takes for the camera to be ready for input

enum class Button
{
	Mask = 0,
	ResetVent = 1,
	Cam01 = 2, // WestHall
	Cam02 = 3, // EastHall
	Cam03 = 4, // Closet
	Cam04 = 5, // Kitchen
	Cam05 = 6, // PirateCove
	Cam06 = 7, // ShowtimeStage
	Cam07 = 8, // PrizeCounter
	Cam08 = 9, // PartsAndServices
	CameraSystem = 10,
	VentSystem = 11,
	DuctSystem = 12,
	Snare_Left = 13,
	Snare_Top = 14,
	Snare_Right = 15,
	Duct_Left = 16,
	Duct_Right = 17,
};

Button CameraButton(Camera cam);

Button SystemButton(State system);

extern const POINT buttonPositions[18];

POINT GetButtonPos(Button);

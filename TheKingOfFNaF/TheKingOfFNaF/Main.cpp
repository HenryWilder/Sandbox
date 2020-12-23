#include "Globals.h"
#include "CustomTypes.h"
#include "Input.h"
#include "Output.h"
#include "GameActions.h"

int main()
{
	g_gameState.Init();

	SelectObject(g_hInternal, g_hBitmap);

	while (true)
	{
		UpdateScreencap();

		RefreshGameData();

		ExecuteBestAction();

		g_gameState.DisplayData();

		/// !! SAFETY !!
		Sleep(32); // Give the user time to move the mouse
		POINT p = GetMousePos();
		if (!(p.x == 0 && p.y == 0) && // We want to reserve this point
			(p.x == 0 || p.y == 0 || p.x >= g_screenWidth || p.y >= g_screenHeight)) // Any edge
		{
			std::cout << "User has chosen to reclaim control.\nTask ended.\n";
			break; // I want to reserve 0,0 for the mouse to reset itself without closing the program
		}
	}

	ReleaseDC(NULL, g_hDesktop);

	DeleteObject(g_hBitmap);

	DeleteDC(g_hInternal);

	std::cin.ignore();

	return 0;
}

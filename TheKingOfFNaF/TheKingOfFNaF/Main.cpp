#include "Globals.h"
#include "CustomTypes.h"
#include "Input.h"
#include "Output.h"
#include "GameActions.h"

int main()
{
	SelectObject(g_hInternal, g_hBitmap);

	while (true)
	{
		UpdateScreencap();

		PingGameState(g_hDesktop);

		ExecuteBestAction();

		/// !! SAFETY !!
		Sleep(30); // Give the user time to move the mouse
		POINT p;
		if (GetCursorPos(&p)) {
			if (!(p.x == 0 && p.y == 0) && (p.x == 0 || p.y == 0 || p.x >= g_screenWidth || p.y >= g_screenHeight))
			{
				std::cout << "User has chosen to reclaim control.\nTask ended.\n";
				break; // I want to reserve 0,0 for the mouse to reset itself without closing the program
			}
		}
		if (g_time.GetPingsSinceChange() > 10)
		{
			std::cout << "Task pinged too many times without updated input from UCN.\nTask ended.\n";
			break;
		}
	}

	ReleaseDC(NULL, g_hDesktop);

	DeleteObject(g_hBitmap);

	DeleteDC(g_hInternal);

	std::cin.ignore();

	return 0;
}
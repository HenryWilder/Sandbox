#include "Globals.h"
#include "CustomTypes.h"
#include "Input.h"
#include "Output.h"
#include "GameActions.h"

bool Gameloop()
{
		UpdateScreencap(); // Update our internal copy of what the gamescreen looks like so we can sample its pixels

		RefreshGameData(); // Using the screencap we just generated, update the game data statuses for decision making

		g_gameState.DisplayData(); // Output the data for the user to view

		ActOnGameData(); // Based upon the game data, perform all actions necessary to return the game to a neutral state

		/// !! SAFETY !!
		Sleep(32); // Give the user time to move the mouse

		POINT p = GetMousePos(); // Check where the mouse is

		if (!(p.x == 0 && p.y == 0) && // I want to reserve 0,0 for the mouse to reset itself without closing the program
			(p.x == 0 || p.y == 0 || p.x >= g_screenWidth || p.y >= g_screenHeight)) // If the mouse is touching any edge (easy to flick your mouse to those positions)
		{
			std::cout << "User has chosen to reclaim control.\nTask ended.\n"; // Let the user know that the program has ended and why

			return false; // This tells the loop to break
		}
		else return true; // Return true if we should keep going
}

int main()
{
	g_gameState.Init(); // Set all values in the gamestate to their defaults

	SelectObject(g_hInternal, g_hBitmap); // I'm not 100% sure what this does but I know it's important

	while (true) // For as long as the game is running
	{
		if (!Gameloop()) break; // If the game loop returns false, exit
	}

	ReleaseDC(NULL, g_hDesktop); // Return the desktop handle

	DeleteObject(g_hBitmap); // Return the bitmap memory to the OS

	DeleteDC(g_hInternal); // Destroy our internal display handle

	std::cin.ignore(); // Wait for user input

	return 0; // Must have a return
}

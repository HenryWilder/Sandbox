#include "Globals.h"
#include "CustomTypes.h"
#include "Input.h"
#include "Output.h"
#include "GameActions.h"
#include "MultiThread.h"

int main()
{
	// SETUP //

	g_gameState.Init(); // Set all values in the gamestate to their defaults

	SelectObject(g_hInternal, g_hBitmap); // I'm not 100% sure what this does but I know it's important

	// GAME LOOP //

	CreateHelpers(); // Creates multiple threads for performing tasks

	// WRAPUP //

	ReleaseDC(NULL, g_hDesktop); // Return the desktop handle

	DeleteObject(g_hBitmap); // Return the bitmap memory to the OS

	DeleteDC(g_hInternal); // Destroy our internal display handle

	return 0; // Must have a return
}

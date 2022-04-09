#pragma once

namespace Win {
	#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
	// Windows Header Files
	#include <windows.h>
}

namespace NotWin {
	//#define USE_LIBTYPE_SHARED
	// Raylib header file
	#include <raylib.h>
}
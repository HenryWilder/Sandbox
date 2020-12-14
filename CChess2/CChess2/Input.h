#pragma once
#include "Universal.h"

extern HANDLE g_console;
extern HANDLE outh;

class Mouse {
	bool b_mouseClicked; // Has the mouse been clicked since we last checked?
	Coord m_clickCoord; // Where the mouse was when it was clicked
	Coord m_tileCoord; // The boardspace coordinate of the mouse (which tile it is over)

	Coord MouseToBoardspace(COORD pos); // Helper function
public:
	bool CheckMouseState(); // Returns whether the mouse has been clicked since the last time this function was called
	Coord GetMouseClickCoord(); // Returns the coordinate of the mouse when it was clicked
	Coord ReadMouseHover(); // Returns the most up-to-date position of the mouse

	void MouseEventProc(MOUSE_EVENT_RECORD mer);
};

extern Mouse g_mouse;

void InitInput();

void PingInput();
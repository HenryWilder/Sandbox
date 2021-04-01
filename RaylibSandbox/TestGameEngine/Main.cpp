#include <raylib.h>
#include "BaseTypes.h"

namespace Input
{
#include "InputMacros.h"

	INPUT_AXIS(Forward, KEY_W, 1.0f, KEY_S, -1.0f);
	void AXIS_EVENT(Forward)
	{
		// TODO: Event
	}
	INPUT_AXIS(Right, KEY_A, 1.0f, KEY_D, -1.0f);
	void AXIS_EVENT(Right)
	{
		// TODO: Event
	}

	INPUT_ACTION(Jump, KEY_SPACE);
	void AXIS_EVENT(Jump)
	{
		// TODO: Event
	}

#include "InputUndef.h"
}

// Tick prior to physics & input
void PreTick()
{
	// TODO Insert pre-tick function calls here
}
// Updates variables prior to frame tick (for things such as physics and collisions)
void PhysTick()
{
	{
		// Insert collision updates here
	}
	{
		// Insert physics updates here
	}
}
// Fires all event dispatchers if valid (Inputs first, then actors)
void EventTick()
{
	{
		Input::Action_Jump_Event();
		Input::Axis_Forward_Event();
	}
	{
		// TODO Insert non-input event calls here
	}
}
// Update the variables for this frame and calls all per-tick events
void Tick()
{
	// TODO Insert calls to all "On tick" functions
}
// Calls all cleanup events at the end of the frame after rendering
void PostTick()
{
	// TODO Insert calls to all "Post tick" functions
}
// Draws the 3D world
void DrawWorld()
{
	// Insert all actor draw calls
}
// Draws all GUI elements in 2D diagetically within the 3D world
void DrawGUIDiagetic()
{
	// Insert UI draw calls
}
// Draws all GUI elements in 2D overlaying the 3D world
void DrawGUIOverlay()
{
	// Insert UI draw calls
}

int main()
{
	int windowWidth = 1280;										// Width of the window (pixels)
	int windowHeight = 720;										// Height of the window (pixels)

	InitWindow(windowWidth, windowHeight, "Game Engine");		// Creates game window and names it
	SetTargetFPS(60);											// Target framerate

	Camera3D playerCamera_FP;
	playerCamera_FP.fovy = 45.0f;								// FOV
	playerCamera_FP.up = Vector3{ 0.0f, 1.0f, 0.0f };			// Don't worry about this
	playerCamera_FP.position = Vector3{ 0.0f, 0.0f, 0.0f };		// Location of the camera in the world
	playerCamera_FP.target = Vector3{ 1.0f, 0.0f, 0.0f };		// Where the camera is looking
	playerCamera_FP.type = CAMERA_FIRST_PERSON;					// First person cam

	// TODO allocate persistent memory and load resources

	while (!WindowShouldClose())
	{
		PreTick();						// Updates variable that need to be updated prior to physics & input

		PhysTick();						// Update physics prior to tick

		EventTick();					// Update events for this frame

		Tick();							// Tick all actors

		BeginDrawing(); {							// (Begin draw phase)

			ClearBackground(BLACK);					// (Clear the background)

			BeginMode3D(playerCamera_FP); {			// (Start drawing in 3D)

				DrawWorld();			// Draw the 3D world

				DrawGUIDiagetic();		// Draw the in-world GUI

			} EndMode3D();							// (Draw in 2D over the 3D frame)

			DrawGUIOverlay();			// Draw the GUI overlay

		} EndDrawing();								// (Finished drawing)

		PostTick();						// End of frame cleanup
	}

	// TODO clean up and unload resources

	CloseWindow();									// (This ocurrs when the user presses escape or closes the window)

	return 0;
}
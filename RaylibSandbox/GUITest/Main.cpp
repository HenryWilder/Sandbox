#include <raylib.h>
#include "GuiFunctions.h"

void GameLoop(std::vector<GUIButton> buttons)
{
	while (!WindowShouldClose())
	{
		// Simulation
		// ----------
		
		// Update GUI
		{
			SetMouseCursor(MOUSE_CURSOR_DEFAULT);

			for (GUIButton& button : buttons)
			{
				ButtonStateFlags state = GetButtonState(button);

				button.state = state;
			}
		}
		
		// Drawing
		// -------
		BeginDrawing(); {

			ClearBackground(BLACK);

			// Draw GUI
			{
				SetMouseCursor(MOUSE_CURSOR_DEFAULT);

				for (GUIButton& button : buttons)
				{
					if (IsButtonStateHovered(button.state))
					{
						SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
						break;
					}
				}
				for (GUIButton& button : buttons)
				{
					DrawGUIButton(button, button.state);
				}
			}

		} EndDrawing();
	}
}

void InitProgram(int windowWidth, int windowHeight, const char* windowName, int fps)
{
	InitWindow(windowWidth, windowHeight, windowName);
	SetTargetFPS(fps);

	Image img = GenImageColor(128, 128, WHITE);
	g_defaultTexture = LoadTextureFromImage(img);
	UnloadImage(img);

	GameLoop({
		GUIButton(Rectangle{ 0, 0, 128, 64 }, { {
			{ GUIMatElement(GUIMat(DARKBLUE),	WHITE) },
			//{ GUIMatElement(GUIMat(BLUE),		WHITE) },
			//{ GUIMatElement(GUIMat(SKYBLUE),	WHITE) },
			//{ GUIMatElement(GUIMat(GRAY),		WHITE) },
			}, {
			{ ButtonStateFlags::Up,			0 },
			//{ ButtonStateFlags::Hovered,	1 },
			//{ ButtonStateFlags::Down,		2 },
			//{ ButtonStateFlags::Pressed,	2 },
			//{ ButtonStateFlags::Released,	2 },
			//{ ButtonStateFlags::Disabled,	3 },
				} }),
			 }
	);

	UnloadTexture(g_defaultTexture);

	CloseWindow();
}

enum TestState {
	Dirty,
	Clean,
	Falling,
	Active,
	LowHealth,
	TurningHead,
};

int main()
{
	InitProgram(1280, 720, "GUI Test", 60);
	return 0;
}
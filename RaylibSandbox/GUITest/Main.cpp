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

	std::vector<GUIButton> buttons{
		GUIButton(Rectangle{ 0, 0, 128, 64 }, GUIMat(DARKBLUE), GUIMat(BLUE), GUIMat(SKYBLUE), GUIMat(GRAY)),
		GUIButton(Rectangle{ 128, 0, 128, 64 }, GUIMat(DARKBLUE), GUIMat(BLUE), GUIMat(SKYBLUE), GUIMat(GRAY)),
		GUIButton(Rectangle{ 256, 0, 128, 64 }, GUIMat(DARKBLUE), GUIMat(BLUE), GUIMat(SKYBLUE), GUIMat(GRAY)),
	};

	GameLoop(buttons);

	for (GUIButton& button : buttons)
	{
		UnloadGUIMat(button.mat_default);
		UnloadGUIMat(button.mat_disabled);
		UnloadGUIMat(button.mat_hovered);
		UnloadGUIMat(button.mat_pressed);
	}

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
	MatStateMachine<TestState> states(
		{ // Possible states
			{ Dirty,		GUIMat(BROWN)	},
			{ Clean,		GUIMat(SKYBLUE) },
			{ Falling,		GUIMat(ORANGE)	},
			{ Active,		GUIMat(YELLOW)	},
			{ LowHealth,	GUIMat(RED)		},
			{ TurningHead,	GUIMat(BLUE)	},
		}, // Init state
		Clean
	);

	InitProgram(1280, 720, "GUI Test", 60);
	return 0;
}
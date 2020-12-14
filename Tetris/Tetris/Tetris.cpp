#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>// ConsolePeekInput, ConsoleReadInput
#include <iostream> // cout
#include <chrono>   // time
#include <thread>   // sleep
#include "Math.h"
#include "Buffer.h"
#include "TetrisBlocks.h"

HANDLE g_console;
bool g_GameRunning = true;

enum class KeyButton : unsigned char
{
	LeftArrow = VK_LEFT,
	RightArrow = VK_RIGHT,
	UpArrow = VK_UP,
	DownArrow = VK_DOWN,

	/*
	 * VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
	 * 0x3A - 0x40 : unassigned
	 * VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
	 */
	 A = 0x41,
	 B = 0x42,
	 C = 0x43,
	 D = 0x44,
	 E = 0x45,
	 F = 0x46,
	 G = 0x47,
	 H = 0x48,
	 I = 0x49,
	 J = 0x4A,
	 K = 0x4B,
	 L = 0x4C,
	 M = 0x4D,
	 N = 0x4E,
	 O = 0x4F,
	 P = 0x50,
	 Q = 0x51,
	 R = 0x52,
	 S = 0x53,
	 T = 0x54,
	 U = 0x55,
	 V = 0x56,
	 W = 0x57,
	 X = 0x58,
	 Y = 0x59,
	 Z = 0x5A,
	 // ... @TODO


	 /*
	  * VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
	  * 0x3A - 0x40 : unassigned
	  * VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
	  */
	  Num_0 = 0x30,
	  Num_1 = 0x31,
	  Num_2 = 0x32,
	  Num_3 = 0x33,
	  Num_4 = 0x34,
	  Num_5 = 0x35,
	  Num_6 = 0x36,
	  Num_7 = 0x37,
	  Num_8 = 0x38,
	  Num_9 = 0x39,
	  // ... @TODO


	  // Other useful keys that you might want. (ie space
	  // VK_SPACE
	  // VK_ESCAPE
	  // etc... @TODO

};
enum class KeyState : unsigned char
{
	Invalid = 0,
	Up = 1,
	Pressed = 2,
	Down = 3,
	Released = 4,
};
const int KeyStateSlots = 512;
KeyState g_keyStates[KeyStateSlots] = {};

void InitInput()
{
	/* A handle for the input stream */
	g_console = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(g_console, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

	for (int i = 0; i < KeyStateSlots; ++i)
	{
		g_keyStates[i] = KeyState::Up;
	}
}
void UpdateInput()
{
	// Progreess the state of the key:
	// Pressed -> down
	// Released -> up
	for (int i = 0; i < KeyStateSlots; ++i)
	{
		KeyState key = g_keyStates[i];

		if (key == KeyState::Pressed)
		{
			key = KeyState::Down;
		}
		if (key == KeyState::Released)
		{
			key = KeyState::Up;
		}

		g_keyStates[i] = key;
	}

	// Handle new input
	{
		DWORD cc = 1;
		INPUT_RECORD irec;
		while (true)
		{
			// If there is any input to be read
			if (PeekConsoleInput(g_console, &irec, 1, &cc) && cc > 0)
			{
				ReadConsoleInput(g_console, &irec, 1, &cc);
				if (irec.EventType == KEY_EVENT)
				{
					KEY_EVENT_RECORD ker = irec.Event.KeyEvent;

					KeyState key = g_keyStates[ker.wVirtualKeyCode];

					if (ker.bKeyDown)
					{
						if (key == KeyState::Up)
						{
							key = KeyState::Pressed;
						}
					}
					else
					{
						if (key == KeyState::Down)
						{
							key = KeyState::Released;
						}
					}

					g_keyStates[ker.wVirtualKeyCode] = key;
				}
			}
			else
			{
				break;
			}

		}
	}
}
KeyState GetKeyState(KeyButton kb)
{
	return g_keyStates[int(kb)];
}

typedef std::chrono::high_resolution_clock        Time;
typedef std::chrono::duration<double, std::milli> f64Ms;  // Duration

void RunGame()
{
	static const f64Ms FrameTime = f64Ms(16.666666666667);
	Time::time_point startFrameTimeStamp = Time::now();

	while (g_GameRunning)
	{
		// Get Input
		UpdateInput();

		// Simulate Game
		// @TODO
		g_frame.SetPixelColor(1, 1, { 255,255,255 });


		// Draw Frame
		g_frame.Draw();

		if (GetKeyState(KeyButton::A) == KeyState::Pressed) // Left
		{
			std::cout << "Left" << std::endl;
			g_frame.SetPixelColor(2, 3, { 255,255,255 });
		}
		if (GetKeyState(KeyButton::S) == KeyState::Pressed) // Down
		{
			std::cout << "Down" << std::endl;
		}
		if (GetKeyState(KeyButton::D) == KeyState::Pressed) // Right
		{
			std::cout << "Right" << std::endl;
		}
		if (GetKeyState(KeyButton::Q) == KeyState::Pressed) // CC rotate
		{
			std::cout << "Rotate Left" << std::endl;
		}
		if (GetKeyState(KeyButton::E) == KeyState::Pressed) // CW rotate
		{
			std::cout << "Rotate Right" << std::endl;
		}

		// Sleep till next frame
		{
			f64Ms simulationTime = Time::now() - startFrameTimeStamp;
			if (FrameTime > simulationTime)
			{
				std::this_thread::sleep_for(FrameTime - simulationTime);
			}
			startFrameTimeStamp = Time::now();
		}
	}
}

//using namespace ext;
int main()
{
	// Initialize Systems
	{
		InitInput();
	}

	RunGame();

	return 0;
}
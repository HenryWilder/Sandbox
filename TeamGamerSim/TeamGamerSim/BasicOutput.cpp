#include "BasicOutput.h"

bool g_keysDown[0xBC + 1] = { };

void SimulateKeyPress(VirtualKey key)
{
	INPUT input;
	ZeroMemory(&input, sizeof(input));
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = (WORD)key;
	SendInput(1, &input, sizeof(INPUT));
	Sleep(2);
	g_keysDown[(WORD)key] = true;
}

void SimulateKeyRelease(VirtualKey key)
{
	INPUT input;
	ZeroMemory(&input, sizeof(input));
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = (WORD)key;
	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));
	Sleep(2);
	g_keysDown[(WORD)key] = false;
}

void SimulateKey(VirtualKey key)
{
	INPUT input;

	input.type = INPUT_KEYBOARD;
	input.ki.wScan = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = (WORD)key;
	input.ki.dwFlags = 0;
	SendInput(1, &input, sizeof(INPUT));

	Sleep(10);

	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));
	Sleep(2);
}

void Type(std::string text)
{	
	for (char c : text)
	{
		char key = text.at(c);
		bool b_shift = ( // Character requires shift key
			(key >= '!' && key <= '+' && key != '\'') ||
			key == ':' || key == '<' ||
			(key >= '>' && key <= '@') ||
			(key >= 'A' && key <= 'Z') ||
			key == '^' || key == '_' ||
			key == '{' || key == '~');

		INPUT input;
		ZeroMemory(&input, sizeof(input));
		input.type = INPUT_KEYBOARD;

		if (b_shift) // Shift down
		{
			input.ki.wVk = 0x10;
			SendInput(1, &input, sizeof(INPUT));
			Sleep(2);
		}

		input.ki.dwFlags = 0;
		input.ki.wVk = (WORD)key;
		SendInput(1, &input, sizeof(INPUT));
		Sleep(10);
		input.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &input, sizeof(INPUT));
		Sleep(2);

		if (b_shift) // Shift up
		{
			input.ki.wVk = 0x10;
			input.ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(1, &input, sizeof(INPUT));
			Sleep(2);
		}
	}
}

void SayPublic(std::string message)
{
	SimulateKey(VirtualKey::VK_Y);
	Type(message);
	SimulateKey(VirtualKey::VK_Enter);
}

void SayTeam(std::string message)
{
	SimulateKey(VirtualKey::VK_U);
	Type(message);
	SimulateKey(VirtualKey::VK_Enter);
}

POINT GetMousePos()
{
	POINT p;
	if (GetCursorPos(&p)) return p;
	else return { 0,0 };
}

void SimulateMouseMove(long x, long y)
{
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.mouseData = 0;
	input.mi.time = 0; // Pleaseeeee don't mess with this... it makes the monitor go funky...
	input.mi.dx = x;
	input.mi.dy = y;
	input.mi.dwFlags = MOUSEEVENTF_MOVE;
	SendInput(1, &input, sizeof(input));
}

void SimulateMouseMove(POINT p) { SimulateMouseMove(p.x, p.y); }

void SimulateMouseGoto(long x, long y)
{
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.mouseData = 0;
	input.mi.time = 0; // Pleaseeeee don't mess with this... it makes the monitor go funky...
	input.mi.dx = x * 34;
	input.mi.dy = y * 61;
	input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
	SendInput(1, &input, sizeof(input));
}

void SimulateMouseGoto(POINT p) { SimulateMouseGoto(p.x, p.y); }

void SimulateMouseClick(DWORD flag) {
	INPUT input;
	ZeroMemory(&input, sizeof(input));

	input.type = INPUT_MOUSE;
	input.mi.dwFlags = flag;
	SendInput(1, &input, sizeof(input));

	Sleep(10);

	input.mi.dwFlags += 2;
	SendInput(1, &input, sizeof(INPUT));
}
void SimulateMouseClickPrimary() { SimulateMouseClick(MOUSEEVENTF_LEFTDOWN); }
void SimulateMouseClickSecondary() { SimulateMouseClick(MOUSEEVENTF_RIGHTDOWN); }


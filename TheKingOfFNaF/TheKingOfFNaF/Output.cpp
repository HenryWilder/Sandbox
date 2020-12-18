#include "Output.h"

void SimulateKey(VirtualKey key)
{
	INPUT input;

	input.type = INPUT_KEYBOARD;
	input.ki.wScan = 0;
	input.ki.dwExtraInfo = 0;

	input.ki.wVk = (int)key;
	input.ki.dwFlags = 0;

	SendInput(1, &input, sizeof(INPUT));

	Sleep(10);

	input.ki.wVk = (int)key;
	input.ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput(1, &input, sizeof(INPUT));

#ifdef _DEBUG
	switch (key)
	{
	case VirtualKey::VK_W:
		std::cout << "Front vent";
		break;
	case VirtualKey::VK_A:
		std::cout << "Left door";
		break;
	case VirtualKey::VK_S:
		std::cout << "Monitor";
		break;
	case VirtualKey::VK_D:
		std::cout << "Right door";
		break;
	case VirtualKey::VK_F:
		std::cout << "Right vent";
		break;
	case VirtualKey::VK_C:
		std::cout << "Catch fish";
		break;
	case VirtualKey::VK_Enter:
		std::cout << "Close ad";
		break;
	case VirtualKey::VK_Space:
		std::cout << "Fan toggle";
		break;
	case VirtualKey::VK_1:
		std::cout << "";
		break;
	case VirtualKey::VK_2:
		std::cout << "";
		break;
	case VirtualKey::VK_3:
		std::cout << "";
		break;
	case VirtualKey::VK_4:
		std::cout << "";
		break;
	case VirtualKey::VK_5:
		std::cout << "";
		break;
	case VirtualKey::VK_6:
		std::cout << "";
		break;
	case VirtualKey::VK_X:
		std::cout << "";
		break;
	case VirtualKey::VK_Z:
		std::cout << "Flashlight";
		break;
	default:
		break;
	}
	std::cout << '\n';
#endif
}

void SimulateMouseMove(long dx, long dy, bool absolute)
{
	INPUT input;
	if (absolute)
	{
		input.type = INPUT_MOUSE;
		input.mi.mouseData = 0;
		input.mi.time = 0;
		input.mi.dx = -100000;
		input.mi.dy = -100000;
		input.mi.dwFlags = MOUSEEVENTF_MOVE;
		SendInput(1, &input, sizeof(input));
	}
	Sleep(1);

	input.type = INPUT_MOUSE;
	input.mi.mouseData = 0;
	input.mi.time = 0;
	input.mi.dx = dx;
	input.mi.dy = dy;
	input.mi.dwFlags = MOUSEEVENTF_MOVE;
	SendInput(1, &input, sizeof(input));
}

void SimulateMouseClick(long dx, long dy, bool absolute)
{
	if (dx || dy || absolute)
	{
		SimulateMouseMove(dx, dy, absolute);
	}

	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.mouseData = 0;
	input.mi.time = 0;
	input.mi.dx = 0;
	input.mi.dy = 0;
	input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &input, sizeof(input));

	Sleep(10);

	input.type = INPUT_MOUSE;
	input.mi.mouseData = 0;
	input.mi.time = 0;
	input.mi.dx = 0;
	input.mi.dy = 0;
	input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &input, sizeof(INPUT));
}

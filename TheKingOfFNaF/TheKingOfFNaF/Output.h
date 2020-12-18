#include "CustomTypes.h"
#include "Globals.h"
#pragma once

enum class VirtualKey : int
{
	VK_W = 0x57,
	VK_A = 0x41,
	VK_S = 0x53,
	VK_D = 0x44,
	VK_F = 0x46,
	VK_C = 0x43,
	VK_Enter = 0x0D,
	VK_Space = 0x20,
	VK_1 = 0x31,
	VK_2 = 0x32,
	VK_3 = 0x33,
	VK_4 = 0x34,
	VK_5 = 0x35,
	VK_6 = 0x36,
	VK_X = 0x58,
	VK_Z = 0x5A,
};

void SimulateKey(VirtualKey key);

void SimulateMouseMove(long dx, long dy, bool absolute = false);

void SimulateMouseClick(long dx = 0, long dy = 0, bool absolute = false);
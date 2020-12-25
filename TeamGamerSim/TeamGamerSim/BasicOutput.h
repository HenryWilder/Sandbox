#pragma once
#include "Globals.h"

////////////////////////////////////////////////////
// This is where we send basic output to the game //
// e.g.                                           //
// - Press "d" key                                //
// - Move mouse to { 24, 36 }                     //
////////////////////////////////////////////////////

enum class VirtualKey : WORD
{
	VK_W = 0x57, // Forward
	VK_A = 0x41, // Left
	VK_S = 0x53, // Back
	VK_D = 0x44, // Right
	VK_Ctrl = 0x11, // Crouch
	VK_Space = 0x20, // Jump

	VK_E = 0x45, // Call for medic
	VK_R = 0x52, // Reload

	VK_1 = 0x31,
	VK_2 = 0x32,
	VK_3 = 0x33,
	VK_4 = 0x34,
	VK_5 = 0x35,
	VK_6 = 0x36,
	VK_7 = 0x37,
	VK_8 = 0x38,
	VK_9 = 0x39,
	VK_0 = 0x30,

	VK_X = 0x58, // Speech menu 1
	VK_Z = 0x5A, // Speech menu 2
	VK_C = 0x43, // Speech menu 3

	VK_G = 0x47, // Taunt
	VK_Comma = 0xBC, // Change class
	VK_CAPS = 0x14, // Caps lock
	VK_Y = 0x59, // Public chat
	VK_U = 0x55, // Team chat
	VK_Enter = 0x0D, // Send chat message
};

extern bool g_keysDown[];

void SimulateKeyPress(VirtualKey);
void SimulateKeyRelease(VirtualKey);

void SimulateKey(VirtualKey);

void Type(std::string);
void SayPublic(std::string);
void SayTeam(std::string);

POINT GetMousePos();
void SimulateMouseMove(long, long);
void SimulateMouseMove(POINT);
void SimulateMouseGoto(long, long);
void SimulateMouseGoto(POINT);

void SimulateMouseClick(DWORD);
void SimulateMouseClickPrimary();
void SimulateMouseClickSecondary();

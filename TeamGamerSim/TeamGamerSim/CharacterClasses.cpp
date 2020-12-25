#include "CharacterClasses.h"

void Class::Turn(POINT delta)
{
	SimulateMouseMove(delta);
}

void Class::UpdateMovement(Direction newBearing)
{
	switch (newBearing.x) { // Left-right
	case -1: // Left
		SimulateKeyPress(VirtualKey::VK_A);
		if (g_keysDown) SimulateKeyRelease(VirtualKey::VK_D);
		break;
	case 0: // Middle
		if (g_keysDown) SimulateKeyRelease(VirtualKey::VK_D);
		if (g_keysDown) SimulateKeyRelease(VirtualKey::VK_A);
		break;
	case 1: // Right
		SimulateKeyPress(VirtualKey::VK_D);
		if (g_keysDown) SimulateKeyRelease(VirtualKey::VK_A);
		break;
	}
	switch (newBearing.y) { // Forward-backward
	case -1: // Backward
		SimulateKeyPress(VirtualKey::VK_W);
		if (g_keysDown) SimulateKeyRelease(VirtualKey::VK_S);
		break;
	case 0: // Middle
		if (g_keysDown) SimulateKeyRelease(VirtualKey::VK_S);
		if (g_keysDown) SimulateKeyRelease(VirtualKey::VK_W);
		break;
	case 1: // Forward
		SimulateKeyPress(VirtualKey::VK_W);
		if (g_keysDown) SimulateKeyRelease(VirtualKey::VK_S);
		break;
	}
}

void Class::Jump()
{
	SimulateKey(VirtualKey::VK_Space);
}

void Class::WeaponSwitchTo(Weapon switchTo)
{
	SimulateKey((VirtualKey)((int)VirtualKey::VK_0 + (int)switchTo));
}

void Class::WeaponFire()
{
	SimulateMouseClickPrimary();
}

void Class::WeaponAltFire()
{
	SimulateMouseClickSecondary();
}

#pragma once
#include "BasicOutput.h"

void Turn(POINT);

struct Direction {
	int x, y; // [-1..1]
};
void UpdateMovement(Direction);
void Jump();

enum class Weapon
{
	Primary = 1,
	Secondary = 2, // This is the sapper on spy
	Melee = 3,

	// Engineer: Construction PDA
	// Spy: Disgui
	PDA1 = 4,
	// Engineer: Destruction PDA
	PDA2 = 5,
};
void WeaponSwitchTo(Weapon);
void PrimaryAttack();
void AltAttack();


#pragma once
#include "DataProcessing.h"
#include "Activities.h"

class Class
{
	void Turn(POINT);

	struct Direction {
		int x, y; // [-1..1]
	};
	void UpdateMovement(Direction);
	void Jump();
	
	void Juke();
	void Dodge();
	void TakeCover();
	
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
	void WeaponFire();
	void WeaponAltFire();
	
	void LocateObjective();
	void LookForPlayer();
	void CheckPlayerTeam();
};

class Scout : Class
{

};

class Soldier : Class
{

};

class Trolldier : public Soldier
{

};

class Pyro : Class
{

};

class Demo : Class
{

};

class Heavy : Class
{

};

class FatScout : public Heavy
{

};

class Engie : Class
{

};

class Medic : Class
{

};

class Sniper : Class
{

};

class Spy : Class
{

};

class CloakAndRecon : public Spy
{

};

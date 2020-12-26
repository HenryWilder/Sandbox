#pragma once
#include "DataProcessing.h"
#include "Actions.h"
#include <chrono>

namespace Class
{
	enum class type
	{
		Scout = 0,
		Soldier = 1,
		Pyro = 2,

		Demo = 3,
		Heavy = 4,
		Engie = 5,

		Medic = 6,
		Sniper = 7,
		Spy = 8,
	};

	namespace Scout
	{

	}
	namespace Soldier
	{

	}
	namespace Pyro
	{

	}
	namespace Demo
	{

	}
	namespace Heavy
	{
		void FatScout();
	}
	namespace Engie
	{

	}
	namespace Medic
	{

	}
	namespace Sniper
	{

	}
	namespace Spy
	{
		void CloakAndRecon();
	}
}

extern Class::type g_myClass;

void ChooseClass(int pick);

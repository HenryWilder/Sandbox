#include "CharacterClasses.h"

namespace Class
{
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
		void FatScout()
		{
		}
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
		void CloakAndRecon()
		{
			UpdateMovement({ 0,1 });
			Turn({ -50,0 });
		}
	}
}

Class::type g_myClass;

void ChooseClass(int pick)
{
	--pick;
	const long x[] = { 333,458,573,768,877,997,1190,1307,1418 };
	SimulateMouseGoto(x[pick], 127);
	SimulateMouseClickPrimary();
	g_myClass = Class::type(pick);
}
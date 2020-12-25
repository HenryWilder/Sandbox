#include "CustomTypes.h"
#include "Globals.h"
#include "Vision.h"
#include "DataProcessing.h"
#include "BasicOutput.h"
#include "Activities.h"
#include "CharacterClasses.h"
#include "Multithreading.h"

int main()
{
	Class myClass;

	{
		const long x[] = { 333,458,573,768,877,997,1190,1307,1418 };
		int pick = rand() % 9;
		SimulateMouseGoto(x[pick],127);
		SimulateMouseClickPrimary();
		myClass;
	}
}

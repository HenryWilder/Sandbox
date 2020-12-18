#include "GameActions.h"

// Updates all known game information
void PingGameState(HDC context)
{
	ReadGameClock(context);
	CheckVentsReset(context);
}

void ExecuteBestAction()
{

}

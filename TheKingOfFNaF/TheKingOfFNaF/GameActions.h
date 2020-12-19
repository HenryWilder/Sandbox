#pragma once
#include "Globals.h"
#include "InputProcessing.h"
#include "Output.h"

// Updates all known game information
void PingGameState(HDC context);

// Playbook of actions
namespace actn
{
	void OpenCameraIfClosed(); // Returns true if cam was closed
	void FuntimeFoxy();
	void ResetVents();
}

void ExecuteBestAction();
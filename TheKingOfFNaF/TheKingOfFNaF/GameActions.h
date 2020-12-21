#pragma once
#include "Globals.h"
#include "InputProcessing.h"
#include "Output.h"

// Updates all known game information
void RefreshGameState();

// Playbook of actions
namespace actn
{
	namespace sub
	{
		void ConfirmSystem(State state); // TODO: think of a better name
		void OpenCameraIfClosed();
		void OpenMonitorIfClosed(); // Like OpenCameraIfClosed(), but slightly faster because it doesn't take into account the system we're on
		void CloseMonitorIfOpen();
		void EnterGameState(State state, Camera cam = Camera::WestHall);
	}
	void FuntimeFoxy();
	void ResetVents();
}

void ExecuteBestAction();

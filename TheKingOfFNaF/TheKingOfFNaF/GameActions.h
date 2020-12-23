#pragma once
#include "Globals.h"
#include "InputProcessing.h"
#include "Output.h"

void OfficeLookLeft();
void OfficeLookRight();

// Updates all known game information
void RefreshGameData();

void ConfirmSystem(State state); // TODO: think of a better name
void OpenCameraIfClosed();
void OpenMonitorIfClosed(); // Like OpenCameraIfClosed(), but slightly faster because it doesn't take into account the system we're on
void CloseMonitorIfOpen();
void EnterGameState(State state, Camera cam = Camera::WestHall);

// Playbook of actions
namespace action
{
	void FuntimeFoxy();
	void ResetVents();
	void HandleNMBB();
}

void ExecuteBestAction();

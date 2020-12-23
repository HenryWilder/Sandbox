#include "GameActions.h"

// Updates all known game information
void RefreshGameData()
{
	UpdateState();
	ReadGameClock();
	CheckVentsReset();
	//LocateOfficeLamp(); // Needs work
}

void ConfirmSystem(State system)
{
	if (g_gameState.state != system)
	{
		if (g_gameState.state == State::Office)
		{
			SimulateKey(VirtualKey::CameraToggle);
			Sleep(camResponseMS);
			UpdateState();
			if (g_gameState.state != system) SimulateMouseClickAt(GetButtonPos(SystemButton(system)));
		}
		else SimulateMouseClickAt(GetButtonPos(Button(10 + (int)system)));
	}
}
void OpenCameraIfClosed()
{
	switch (g_gameState.state)
	{
	case State::Camera:
		break;

	case State::Office:
		SimulateKey(VirtualKey::CameraToggle);
		Sleep(camResponseMS);
		UpdateState();
		if (g_gameState.state != State::Camera) SimulateMouseClickAt(GetButtonPos(Button::CameraSystem));
		break;

	default: // Vent or duct
		SimulateMouseClickAt(GetButtonPos(Button::CameraSystem));
		break;
	}
	Sleep(1); // In case the next step is another button press elsewhere
}

void OpenMonitorIfClosed()
{
	if (g_gameState.state == State::Office) {
		SimulateKey(VirtualKey::CameraToggle);
		Sleep(camResponseMS);
	}
}

void CloseMonitorIfOpen()
{
	if (g_gameState.state != State::Office) {
		SimulateKey(VirtualKey::CameraToggle);
		Sleep(camResponseMS);
	}
}

void EnterGameState(State state, Camera cam)
{
	switch (state)
	{
	case State::Office:
		CloseMonitorIfOpen();
		break;
	case State::Camera:
		OpenCameraIfClosed();
		if (g_gameState.stateData.cd.camera != cam) SimulateMouseClickAt(GetButtonPos(CameraButton(cam)));
		break;
	case State::Duct:
		OpenMonitorIfClosed();
		UpdateState();
		if (g_gameState.state != State::Duct) SimulateMouseClickAt(GetButtonPos(Button::DuctSystem));
		break;
	case State::Vent:
		OpenMonitorIfClosed();
		UpdateState();
		if (g_gameState.state != State::Vent) SimulateMouseClickAt(GetButtonPos(Button::VentSystem));
		break;
	}
	Sleep(1);
}

namespace action
{
	void FuntimeFoxy()
	{
		OpenCameraIfClosed();
		SimulateMouseClickAt(GetButtonPos(Button::Cam06));
	}
	void ResetVents()
	{
		OpenMonitorIfClosed(); // We don't need to care which system, only that the monitor is up.
		SimulateMouseClickAt(GetButtonPos(Button::ResetVent));
		g_gameState.gameData.ventilationNeedsReset = false;
		Sleep(10);
	}
	void HandleNMBB()
	{
		Sleep(10);
		CheckOnNMBB(); // Double check
		if (g_gameState.gameData.nmBB)
		{
			SimulateKey(VirtualKey::Flashlight);
			g_gameState.gameData.nmBB = false;
		}
	}
}

void ExecuteBestAction()
{
	// Actions to perform inside the office
	if (g_gameState.state == State::Office)
	{
		CheckOnNMBB();
		if (g_gameState.gameData.nmBB) action::HandleNMBB();
	}

	// Organized in order of importance
	if (g_gameState.gameData.ventilationNeedsReset)
	{
		action::ResetVents();
	}

	if ((450u/*deciseconds in an hour*/ - g_gameState.gameData.time.GetDecisecondsSinceHour()) <= (10u/*1 second*/ + (camResponseMS / 100))) // We have <= 1 seconds before the next hour
	{
		action::FuntimeFoxy();
		Sleep(10);
	}
}

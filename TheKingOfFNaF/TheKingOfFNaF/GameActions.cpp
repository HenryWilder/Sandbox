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
		Sleep(17); // Wait a little bit to make sure we have time for the screen to change
		UpdateScreencap();
		if (CheckOnNMBB()) // Double check--NMBB will kill us if we flash him wrongfully
			SimulateKey(VirtualKey::Flashlight); // If he is in fact still up, flash the light on him to put him back down
	}
}

void ActOnGameData()
{
	/*
	* Definitions
	* ===========
	* "Behavioral events" - Some things are not events so much as hints that our current behavior is unsustainable due to external data.
		When one of these occurs, we cannot simply 'handle' it and be done, and must change our behavioral pattern to better suit the needs of the event
		until the state has returned to neutral. Thankfully, the behavioral changes are usually transient and only require temporarily disabling certain systems.

	* "Breakflow events" - Events which give us abrupt notice which we have only a short window to react to.
		We don't know ahead of time when these events will occur, and they can trigger automatically without intervention.

	* "Deltastate events" - Events triggered by a change in gamestate (like opening or closing the monitor).
		These events usually aren't timed and can be done at leisure, but they limit the actions we can perform without handling them.

	* "Timed events" - Events which are time-sensitive relative to the in-game clock or a countdown.
		These are usually long-term and while high-priority, can be done when convenient.

	* "Transient events" - These are events which can be detected & reacted to without any dependence upon or changes to the current game state.
	* 
	* "Distractor events" - Depending on the event, these events can be quick difficult to react to. They make it much harder to react to other events,
		and may even take away our control. Thankfully these events are usually either very short in duration or can be handled by rote.
	*/

	if (g_gameState.state == State::Office)
	{
		if (CheckOnNMBB()) action::HandleNMBB();
	}

	if (g_gameState.gameData.ventilationNeedsReset)
	{
		action::ResetVents();
	}

	if ((450u/*deciseconds in an hour*/ - g_gameState.gameData.time.GetDecisecondsSinceHour()) <= (10u/*1 second*/ + (camResponseMS / 100))) // We have <= 1 seconds before the next hour
	{
		action::FuntimeFoxy();
		Sleep(10);
	}

	// Lowest priority actions should go down here //
}

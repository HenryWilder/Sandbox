#include "GameActions.h"

// Updates all known game information
void PingGameState(HDC context)
{
	ReadGameClock(context);
	CheckVentsReset(context);
}

namespace actn
{
	void OpenCameraIfClosed()
	{
		if (g_gameState.state == State::Office)
		{
			SimulateKey(VirtualKey::CameraToggle);
			Sleep(camResponseMS);
		}
	}
	void CloseCameraIfOpen()
	{
		if (g_gameState.state != State::Office)
		{
			SimulateKey(VirtualKey::CameraToggle);
			Sleep(camResponseMS);
		}
	}
	void FuntimeFoxy()
	{
		OpenCameraIfClosed();
		SimulateMouseClickAt(GetButtonPos(Button::Cam06));
	}
	void ResetVents()
	{
		OpenCameraIfClosed();
		SimulateMouseClickAt(GetButtonPos(Button::ResetVent));
		g_gameState.gameData.ventilationNeedsReset = false;
		Sleep(10);
		CloseCameraIfOpen();
	}
}


void ExecuteBestAction()
{
	GameState::StateData const& stateData = g_gameState.stateData;
	GameState::GameData const& gameData = g_gameState.gameData;

	// Organized in order of importance
	if (gameData.ventilationNeedsReset)
	{
		actn::ResetVents();
	}

	if ((450u/*deciseconds in an hour*/ - (gameData.time.GetDeciseconds() - gameData.time.GetWholeHourDeciseconds())) <= 10u/*1 second*/) // We have <= 1 seconds before the next hour
	{
		actn::FuntimeFoxy();
		Sleep(10);
	}
}

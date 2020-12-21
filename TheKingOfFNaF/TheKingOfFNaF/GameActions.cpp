#include "GameActions.h"

// Updates all known game information
void RefreshGameState()
{
	UpdateState();
	ReadGameClock();
	CheckVentsReset();
}

namespace actn
{
	namespace sub
	{
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
		void EnterGameState(State state)
		{
			switch (state)
			{
			case State::Office:
				if (g_gameState.state != State::Office) CloseMonitorIfOpen();
				break;
			case State::Camera:
				if (g_gameState.state != State::Camera)
				{
					if (g_gameState.state == State::Office)
					{
						SimulateKey(VirtualKey::CameraToggle);
						Sleep(camResponseMS);
						UpdateState();
						if (g_gameState.state != State::Camera) SimulateMouseClickAt(GetButtonPos(Button::CameraSystem));
					}
					else SimulateMouseClickAt(GetButtonPos(Button::CameraSystem));
				}
				break;
			case State::Duct:
				break;
			case State::Vent:
				break;
			}
		}
	}

	void FuntimeFoxy()
	{
		State storedState = g_gameState.state;
		sub::OpenCameraIfClosed();
		SimulateMouseClickAt(GetButtonPos(Button::Cam06));
	}
	void ResetVents()
	{
		sub::OpenMonitorIfClosed(); // We don't need to care which system, only that the monitor is up.
		SimulateMouseClickAt(GetButtonPos(Button::ResetVent));
		g_gameState.gameData.ventilationNeedsReset = false;
		Sleep(10);
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

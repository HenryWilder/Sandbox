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

		void EnterGameState(State state, Camera cam)
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

					UpdateState();
				}
				if (g_gameState.stateData.cd.camera != cam) SimulateMouseClickAt(GetButtonPos(CameraButton(cam)));
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
	const State storedState = g_gameState.state; // We need a copy of this, not just a reference
	const Camera storedCamera = g_gameState.stateData.cd.camera; // We need a copy of this, not just a reference


	// Organized in order of importance
	if (gameData.ventilationNeedsReset)
	{
		actn::ResetVents();
	}

	if ((450u/*deciseconds in an hour*/ - gameData.time.GetDecisecondsSinceHour()) <= (10u/*1 second*/ + (camResponseMS / 100))) // We have <= 1 seconds before the next hour
	{
		actn::FuntimeFoxy();
		Sleep(10);
	}

	actn::sub::EnterGameState(storedState, storedCamera);
}

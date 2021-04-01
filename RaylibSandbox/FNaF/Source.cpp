#include <raylib.h>
#include <stdlib.h>
#include <unordered_map>
#include "random.h"
#include "Animatronics.h"

struct CamButton
{
	CamButton() = default;
	CamButton(Cam _cam, Vector2 pos) : collision{ pos.x, pos.y, 80.0f, 50.0f }, cam(_cam) {};

	Rectangle collision;
	Cam cam;
};
const char* GetCamName(Cam cam)
{
	switch (cam) {
	default: return "";
	case Cam::Cam1A: return "1A";
	case Cam::Cam1B: return "1B";
	case Cam::Cam1C: return "1C";
	case Cam::Cam2A: return "2A";
	case Cam::Cam2B: return "2B";
	case Cam::Cam3:	 return "3";
	case Cam::Cam4A: return "4A";
	case Cam::Cam4B: return "4B";
	case Cam::Cam5:  return "5";
	case Cam::Cam6:  return "6";
	case Cam::Cam7:  return "7";
	}
}
bool IsHovered(CamButton& button)
{
	return CheckCollisionPointRec(GetMousePosition(), button.collision);
}
void DrawButton(CamButton& button, Color color)
{
	DrawRectangleRec(button.collision, color);
	DrawRectangleLinesEx(button.collision, 3, WHITE);
	DrawText("CAM", (int)button.collision.x + 5, (int)button.collision.y + 4, 20, WHITE);
	DrawText(GetCamName(button.cam), (int)button.collision.x + 5, (int)button.collision.y + 20, 20, WHITE);
}

int main()
{
	int windowWidth = 1920;
	int windowHeight = 1080;
	InitWindow(windowWidth, windowHeight, "Five Nights at Freddy's");
	ToggleFullscreen();
	SetTargetFPS(60);

	int power = 100;
	Cam activeCam = Cam::Cam1A;
	bool b_watchingCams = false;
	float officeRot = 0.0f;
	bool hoveringMonitorFlipperButtonRectangleUhhThingyWhateverIDKYouKnowWhatIMeanYeahYouKnow = false;

	enum {
		Animatronic_Bonnie,
		Animatronic_Chica,
		Animatronic_Foxy,
		Animatronic_Freddy,

		Animatronic_Total,
	};

	int cooldown_max[Animatronic_Total];
	cooldown_max[Animatronic_Bonnie ] = 200;
	cooldown_max[Animatronic_Chica	] = 300;
	cooldown_max[Animatronic_Foxy   ] = 700;
	cooldown_max[Animatronic_Freddy ] = 400;
	
	int cooldown[Animatronic_Total];
	for (int i = 0; i < Animatronic_Total; ++i) {
		cooldown[i] = cooldown_max[i];
	}

	std::map<Place, Texture2D> renders[Animatronic_Total];
	renders[Animatronic_Bonnie] = {
		{Place::ShowStage, LoadTexture(/* TODO */)},
	};
	renders[Animatronic_Chica ] = {
		{},
	};
	renders[Animatronic_Foxy  ] = {
		{},
	};
	renders[Animatronic_Freddy] = {
		{},
	};

	Bonnie bonnie;
	Chica chica;
	Freddy freddy;
	Foxy foxy;

	Animatronic* characters[4] = { &bonnie, &chica, &freddy, &foxy };

	Texture imTheMapimTheMapimTheMapIMTHEMAAAAAAAP = LoadTexture("FNaF Map-01.png");
	Texture theButtonBarThingyOnTheBottomThatOpensTheCameras = LoadTexture("FNaF_CamOpenButton-01.png");

	Texture office = LoadTexture("Office.PNG");
	Shader staticShader = LoadShader(NULL, "static.frag");

	CamButton buttons[] = {
		CamButton(Cam::Cam1A, Vector2{1498,492}),
		CamButton(Cam::Cam1B, Vector2{1470,570}),
		CamButton(Cam::Cam1C, Vector2{1425,680}),
		CamButton(Cam::Cam2A, Vector2{1500,844}),
		CamButton(Cam::Cam2B, Vector2{1500,900}),
		CamButton(Cam::Cam3,  Vector2{1380,818}),
		CamButton(Cam::Cam4A, Vector2{1647,844}),
		CamButton(Cam::Cam4B, Vector2{1647,900}),
		CamButton(Cam::Cam5,  Vector2{1322,609}),
		CamButton(Cam::Cam6,  Vector2{1793,795}),
		CamButton(Cam::Cam7,  Vector2{1795,610}),
	};

	int frame = 0;

	auto DrawTextureWithPerspective = [&windowWidth, &windowHeight, &officeRot](Texture& tex) {
		DrawTexturePro(tex,
					   Rectangle{ 0.0f, 0.0f, (float)tex.width, (float)tex.height },
					   Rectangle{ 0.0f, 0.0f, (float)tex.width, (float)windowHeight },
					   Vector2{ officeRot * (float)windowWidth, 0.0f },
					   0.0f, WHITE);
	};

	InitAudioDevice();

	Sound night1PhoneCall = LoadSound("Night1Call.wav");

	while (true) {
		if (IsAudioDeviceReady()) break;
	}
	
	PlaySound(night1PhoneCall);

	while (!WindowShouldClose())
	{
		DEBUG_ONLY(int i = 32;)
		for (Animatronic* anim : characters) {
			anim->Tick();
			DEBUG_ONLY(DrawText(FormatText("Time left: %i", anim->cooldown), 0, i += 32, 24, WHITE);)
		}
		DEBUG_ONLY(DrawText(FormatText("Freddy's stored crits: %i", freddy.storedCrits), 0, 256, 24, WHITE);)

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			for (CamButton& button : buttons)
			{
				if (IsHovered(button))
				{
					activeCam = button.cam;
					break;
				}
			}
		}
		
		if (CheckCollisionPointRec(GetMousePosition(), Rectangle{ 512 + 256, (float)windowHeight - 128, (float)windowWidth - 1024 - 512, 128 }))
		{
			if (!hoveringMonitorFlipperButtonRectangleUhhThingyWhateverIDKYouKnowWhatIMeanYeahYouKnow)
			{
				hoveringMonitorFlipperButtonRectangleUhhThingyWhateverIDKYouKnowWhatIMeanYeahYouKnow = true;

				b_watchingCams = !b_watchingCams;
				if (b_watchingCams) {
					freddy.Stun();
					foxy.Stun();
				}
				else {
					freddy.Release();
					foxy.Release();
				}
			}
		}
		else hoveringMonitorFlipperButtonRectangleUhhThingyWhateverIDKYouKnowWhatIMeanYeahYouKnow = false;

		if (!b_watchingCams)
		{
			float halfWidth = ((float)windowWidth * 0.5f);
			if ((officeRot < 0.975f) && (GetMousePosition().x > (windowWidth - 512)))
				officeRot += 0.025f;
			if ((officeRot > 0.0f) && (GetMousePosition().x < (512)))
				officeRot -= 0.025f;

			if (windowWidth < 0.0f) officeRot = 0.0f;
			//if (windowWidth < 0.0f) officeRot = 0.0f;
		}

		SetSoundVolume(night1PhoneCall, ((1.0f - officeRot) * 0.5f) + 0.5f);

		BeginDrawing();
		{
			ClearBackground(BLACK);

			if (b_watchingCams) {

				for (Animatronic* anim : characters) {
					if (anim->IsOnCam(activeCam))
					{
						DrawTexture(anim->GetTexture(frame), 0, 0, WHITE);
					}
				}

				DrawTexture(imTheMapimTheMapimTheMapIMTHEMAAAAAAAP, 0, 0, WHITE);

				for (CamButton& button : buttons)
				{
					if (button.cam == activeCam) DrawButton(button, LIME);
					else if (IsHovered(button)) DrawButton(button, BLUE);
					else DrawButton(button, DARKGRAY);
				}
			}
			else {
				for (Animatronic* anim : characters) {
					if (anim->IsOnCam(Cam::eDoor) || anim->IsOnCam(Cam::wDoor)) {
						Texture2D tex = anim->GetTexture(frame);
						DrawTextureWithPerspective(tex);
					}
				}

				DrawTextureWithPerspective(office);
			}

			DrawTexture(theButtonBarThingyOnTheBottomThatOpensTheCameras, 0, 0, WHITE);

#if _DEBUG
			DrawText(TextFormat("Rot: %f", officeRot), 0, 32, 48, WHITE);
#endif
		}
		EndDrawing();

		frame++;
	}

	UnloadTexture(imTheMapimTheMapimTheMapIMTHEMAAAAAAAP);
	UnloadTexture(theButtonBarThingyOnTheBottomThatOpensTheCameras);
	UnloadTexture(office);

	CloseAudioDevice();
	CloseWindow();

	return 0;
}
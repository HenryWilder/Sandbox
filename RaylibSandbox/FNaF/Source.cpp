#include <raylib.h>
#include <stdlib.h>
#include "random.h"
#include "Animatronics.h"

struct CamButton
{
	CamButton() = default;
	CamButton(Cam _cam, Vector2 pos) : collision{ pos.x, pos.y, 60.0f, 44.0f }, cam(_cam) {};

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
	case Cam::Cam3: return "3";
	case Cam::Cam4A: return "4A";
	case Cam::Cam4B: return "4B";
	case Cam::Cam5: return "5";
	case Cam::Cam6: return "6";
	case Cam::Cam7: return "7";
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
	DrawText("CAM", button.collision.x + 5, button.collision.y + 4, 20, WHITE);
	DrawText(GetCamName(button.cam), button.collision.x + 5, button.collision.y + 20, 20, WHITE);
}

int main()
{
	int windowWidth = 1920;
	int windowHeight = 1080;
	InitWindow(windowWidth, windowHeight, "Five Nights at Freddy's");
	SetTargetFPS(60);

	int power = 100;
	Cam activeCam = Cam::Cam1A;
	bool b_watchingCams = false;
	float officeRot = 0.0f;
	bool hoveringMonitorFlipperButtonRectangleUhhThingyWhateverIDKYouKnowWhatIMeanYeahYouKnow = false;

	Bonnie bonnie;
	Chica chica;
	Freddy freddy;
	Foxy foxy;

	Animatronic* characters[4] = { &bonnie, &chica, &freddy, &foxy };

	Texture imTheMapimTheMapimTheMapIMTHEMAAAAAAAP = LoadTexture("Map.PNG");

	Texture office = LoadTexture("Office.PNG");
	Shader staticShader = LoadShader(NULL, "static.frag");

	CamButton buttons[] = {
		CamButton(Cam::Cam1A, Vector2{1292,480}),
		CamButton(Cam::Cam1B, Vector2{1240,560}),
		CamButton(Cam::Cam1C, Vector2{1200,680}),
		CamButton(Cam::Cam2A, Vector2{1235,878}),
		CamButton(Cam::Cam2B, Vector2{1235,973}),
		CamButton(Cam::Cam3,  Vector2{1140,836}),
		CamButton(Cam::Cam4A, Vector2{1452,878}),
		CamButton(Cam::Cam4B, Vector2{1452,973}),
		CamButton(Cam::Cam5,  Vector2{1100,600}),
		CamButton(Cam::Cam6,  Vector2{1555,825}),
		CamButton(Cam::Cam7,  Vector2{1600,575}),
	};

	int frame = 0;

	auto DrawTextureWithPerspective = [&windowWidth, &windowHeight, &officeRot](Texture& tex) {
		DrawTexturePro(tex,
					   Rectangle{ 0.0f, 0.0f, (float)tex.width, (float)tex.height },
					   Rectangle{ 0.0f, 0.0f, (float)tex.width, (float)windowHeight },
					   Vector2{ officeRot * (float)windowWidth, 0.0f },
					   0.0f, WHITE);
	};

	while (!WindowShouldClose())
	{
#if _DEBUG
		int i = 32;
#endif
		for (Animatronic* anim : characters) {
			anim->Tick();
#if _DEBUG
			DrawText(FormatText("Time left: %i", anim->cooldown), 0, i += 32, 24, WHITE);
#endif
		}
#if _DEBUG
		DrawText(FormatText("Freddy's stored crits: %i", freddy.storedCrits), 0, 256, 24, WHITE);
#endif

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

		BeginDrawing();
		{
			ClearBackground(BLACK);

			if (b_watchingCams) {

				for (Animatronic* anim : characters) {
					if (anim->IsOnCam(activeCam)) anim->DrawSprite(frame);
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
						Texture* tex = anim->GetSprite(frame);
						if (tex) DrawTextureWithPerspective(*tex);
					}
				}

				DrawTextureWithPerspective(office);
			}

#if _DEBUG
			DrawText(TextFormat("Rot: %f", officeRot), 0, 32, 48, WHITE);
#endif
		}
		EndDrawing();

		frame++;
	}

	UnloadTexture(imTheMapimTheMapimTheMapIMTHEMAAAAAAAP);
	UnloadTexture(office);

	CloseWindow();

	return 0;
}
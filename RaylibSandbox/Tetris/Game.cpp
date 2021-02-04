#include <raylib.h>
#include <raymath.h>
#include <string.h>
#include <random>
#include <thread>
#include "AmyMath.h"

struct Shape;

int main() {
#pragma region Setup phase
	int windowWidth{ 1280 }, windowHeight{ 720 };
	InitWindow(windowWidth, windowHeight, "Tetris");

	InitAudioDevice();
	AudioStream stream = InitAudioStream(20500, 8 * sizeof(short), 1);
	short* data = new short[4096];
	short* buffer = new short[4096];
	PlayAudioStream(stream);
	int delay = 0;
	SetTargetFPS(60);
	std::thread();
#pragma endregion
	while (!WindowShouldClose()) {
#pragma region Simulation phase
		if (!delay)
		{
			float wavelength = 440;
			for (int i = 0; i < 4096; ++i)
			{
				data[i] = (short)(sinf(((2*PI*(float)i / wavelength))) * 12000); // 32000
			}
		}
		delay = (delay + 1) % (60); // frame count
		if (IsAudioStreamProcessed(stream))
		{
			memcpy(buffer, data, 4096 * sizeof(short));
			UpdateAudioStream(stream, buffer, 4096);
		}
#pragma endregion
#pragma region Drawing phase
		BeginDrawing(); {
			ClearBackground(BLACK);
			for (int i = 1; i < 4096; ++i)
			{
				DrawLine(((i) / 4), ((((float)(data[i]) / 32000.0f) + 1.0f) / 2.0f) * ((float)windowHeight / 3.0f) + ((float)windowHeight / 3.0f),
						 ((i-1) / 4), ((((float)(data[i-1]) / 32000.0f) + 1.0f) / 2.0f) * ((float)windowHeight / 3.0f) + ((float)windowHeight / 3.0f),
						 RED);
			}
		} EndDrawing();
#pragma endregion
	} CloseWindow();
#pragma region Cleanup phase
	CloseAudioDevice();
	CloseAudioStream(stream);
	delete[] data;
	delete[] buffer;
#pragma endregion
	return 0;
}
struct Shape
{
	int x, y;
	Color color;
	void Draw()
	{

	}
};
#include <raylib.h>
#include <raymath.h>
#include <string.h>
#include <random>
#include <thread>
#include "AmyMath.h"

struct Shape;

constexpr int c_audioBufferSize = 4096;
constexpr int c_sampleRate = 41000;

int MostOf(int cycleSize, int maxSpace)
{
	return (maxSpace / cycleSize) * cycleSize;
}

float WaveLen(float freq)
{
	return c_sampleRate / freq;
}

float Widen(float val)
{
	return (((val) * 2.0f) - 1.0f);
}

typedef float (*WaveForm)(int x, int wavelength);
float SquareWave(int x, int wavelength)
{
	int wavePoint = (x % wavelength);
	int midpoint = (wavelength / 2);
	if (wavePoint > midpoint) return 1.0f;
	else return -1.0f;
}
float SawtoothWave(int x, int wavelength)
{
	float wavePoint = (float)(x % wavelength);
	float y = wavePoint / (float)wavelength;
	return Widen(y);
}
float RampWave(int x, int wavelength)
{
	/***********************************
	*
	*	|        /   |
	*	|      /     |
	*	0------------1
	*	|  /         |
	*	|/           |
	*
	***********************************/
	int wavePoint = (x % wavelength);
	float reverse = (float)(wavelength - wavePoint);
	float y = reverse / (float)wavelength;
	return Widen(y);
}
float TriangleWave(int x, int wavelength)
{
	/***********************************
	*
	*	   1/4   3/4
	*	|  /|\    |  |
	*	| / | \   |  |
	*	0---+-1/2-+--1
	*	|   |   \ | /|
	*	|   |    \|/ |
	*
	***********************************/
	int wavePoint = (x % wavelength);
	int midpoint = (wavelength / 2);
	int quarterPoint = midpoint / 2;
	float m;
	if ((wavePoint > quarterPoint) && (wavePoint <= (midpoint + quarterPoint * 3)))
	{
		m = (float)(wavePoint) * 0.5f; // Downward
	}
	else
	{
		m = (float)(wavelength - wavePoint - quarterPoint) * 0.5f; // Upward
	}
	float y = m / (float)wavelength;
	return Widen(y);
}
float SineWave(int x, int wavelength)
{
	return 0.0f;
}
float Noise(int x, int wavelength)
{
	return 0.0f;
}
static WaveForm s_WaveFormFuncs[] = { SineWave, SquareWave, RampWave, SawtoothWave, TriangleWave, Noise  };

void FillAudioStream(short* out_buff, WaveForm func, int wavelength = WaveLen(440), int volume = 32000)
{
	for (int x = 0; x < MostOf(wavelength, c_audioBufferSize); ++x)
	{
		out_buff[x] = (short)(func(x, wavelength) * (float)volume);
	}
}

int main() {

#pragma region Setup phase

	int windowWidth{ 1280 }, windowHeight{ 720 };
	InitWindow(windowWidth, windowHeight, "A program I'm making in C++");
	constexpr int c_frameRate = 30;
	SetTargetFPS(c_frameRate);

	InitAudioDevice();
	AudioStream stream = InitAudioStream(41000, 8 * sizeof(short), 1);
	short* data = new short[c_audioBufferSize];
	PlayAudioStream(stream);
	int delay = 0;
	int update = 0;
	int wavelength = (int)WaveLen(220); // Must be almost perfectly evenly divisible by buffer size

#pragma endregion

	while (!WindowShouldClose()) {

#pragma region Simulation phase

		if (!delay)
		{
			FillAudioStream(data, TriangleWave, wavelength, 10000); // Insert waveform here
			//printf("Current wavelength: %i\n", wavelength);
		}
		delay = (delay + 1) % (30); // frame count
		if (IsAudioStreamProcessed(stream))
		{
			UpdateAudioStream(stream, data, MostOf(wavelength, c_audioBufferSize));
			//printf("Audiostream updated!\n");
		}
		
#pragma endregion

#pragma region Drawing phase

		BeginDrawing(); {
			ClearBackground(BLACK);
			int bufferSize = MostOf(wavelength, c_audioBufferSize);
			int bufferSizeOnScreen = bufferSize / 4;
			int x = 1;
			DrawLine(0, windowHeight / 2, windowWidth, windowHeight / 2, RAYWHITE);
			for (; x < bufferSize; ++x)
			{
				DrawLine(((x) / 4), ((((float)(data[x]) / 32000.0f) + 1.0f) / 2.0f) * ((float)windowHeight / 3.0f) + ((float)windowHeight / 3.0f),
						 ((x-1) / 4), ((((float)(data[x-1]) / 32000.0f) + 1.0f) / 2.0f) * ((float)windowHeight / 3.0f) + ((float)windowHeight / 3.0f),
						 RED);
			}
			DrawLine(bufferSizeOnScreen, 0, bufferSizeOnScreen, windowHeight, BLUE);
		} EndDrawing();

#pragma endregion

	} CloseWindow();

#pragma region Cleanup phase

	CloseAudioDevice();
	CloseAudioStream(stream);
	delete[] data;

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
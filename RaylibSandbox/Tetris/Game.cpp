#include <raylib.h>
#include <raymath.h>
#include <string.h>
#include <random>
#include <thread>
#include <vector>
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

enum class Note
{
	C,
	D,
	E,
	F,
	G,
	A,
	B,
};
float Key(Note key, int octave = 4)
{
	float notes[] = {
		WaveLen(262),WaveLen(293.665),WaveLen(329.628),WaveLen(349.228),WaveLen(391.995),WaveLen(440),WaveLen(493.883),
		WaveLen(523.25) };
	return notes[(int)key + (7 * (octave - 4))];
}

#pragma region Base wave functions

typedef float WFReturn_t;
WFReturn_t Widen(WFReturn_t val)
{
	return (((val) * (WFReturn_t)(2)) - (WFReturn_t)(1));
}
typedef int WavePos_t;
typedef float WaveLength_t;
#define WAVEFORM(name) WFReturn_t (name)(WavePos_t x, WaveLength_t wavelength)
typedef WAVEFORM(*WaveForm);

WAVEFORM(SquareWave)
{
	WavePos_t wavePoint = (WavePos_t)((int)x % (int)wavelength);
	WavePos_t midpoint = (WavePos_t)(wavelength / (WaveLength_t)(2));
	if (wavePoint > midpoint) return (WFReturn_t)(1);
	else return (WFReturn_t)(-1);
}
WAVEFORM(SawtoothWave)
{
	WFReturn_t wavePoint = (WFReturn_t)((int)x % (int)wavelength);
	WFReturn_t y = wavePoint / (WFReturn_t)wavelength;
	return Widen(y);
}
WAVEFORM(RampWave)
{
	WavePos_t wavePoint = (WavePos_t)((int)x % (int)wavelength);
	WFReturn_t reverse = (WFReturn_t)(wavelength - wavePoint);
	WFReturn_t y = reverse / (WFReturn_t)wavelength;
	return Widen(y);
}
WAVEFORM(TriangleWave)
{
	WavePos_t wavePoint = ((WavePos_t)(x + (wavelength / 4)) % (int)wavelength);
	return (WFReturn_t)( abs( ( (float)wavePoint / (float)wavelength ) - ( 0.5f ) ) - ( 0.25f ) ) * ( 2.0f );
}
WAVEFORM(SineWave)
{
	return (WFReturn_t)sinf(2.0f*PI*((float)x / (float)wavelength));
}
WAVEFORM(Noise)
{
	return (WFReturn_t)Widen((float)rand() / (float)RAND_MAX);
}

#pragma endregion

struct Oscillator
{
	WaveForm wave;
	float pitchScale, pitchOffset, amplitude;

	WAVEFORM(Function) // Must line up with type WaveForm
	{
		return wave(x, (wavelength * pitchScale) + pitchOffset) * amplitude;
	}
};
struct Synth
{
	std::vector<Oscillator> m_oscilators;

	WAVEFORM(Function) // Must line up with type WaveForm
	{
		float out = 1.0f;
		for (Oscillator osc : m_oscilators)
		{
			out *= osc.Function(x, wavelength);
		}
		return out;
	}
};

void FillAudioStream(AudioStream& stream, short* out_buff, WaveForm func, WaveLength_t wavelength = Key(Note::C,4), int volume = 32000)
{
	for (WavePos_t x = (WavePos_t)(0); x < c_audioBufferSize; x += (WavePos_t)(1))
	{
		out_buff[x] = (short)(func(x, wavelength) * (float)volume);
	}
}
void FillAudioStream(AudioStream& stream, short* out_buff, Synth synth, WaveLength_t wavelength = Key(Note::C,4), int volume = 32000)
{
	for (WavePos_t x = (WavePos_t)(0); x < c_audioBufferSize; x += (WavePos_t)(1))
	{
		out_buff[x] = (short)(synth.Function(x, wavelength) * (float)volume);
	}
}

static Synth s_InitPatch = { {
	{ SquareWave, 1.0f, 1.0f },
	{ SineWave, 0.5f, -1.0f }
} };

int main() {

#pragma region Setup phase

	int windowWidth{ 1280 }, windowHeight{ 720 };
	InitWindow(windowWidth, windowHeight, "A program I'm making in C++");
	constexpr int c_frameRate = 30;
	SetTargetFPS(c_frameRate);

	InitAudioDevice();
	AudioStream stream = InitAudioStream(c_sampleRate, 8 * sizeof(short), 1);
	short* data = new short[c_audioBufferSize];
	PlayAudioStream(stream);
	int delay = 0;
	int update = 0;
	int wavelength = (int)WaveLen(440); // Must be almost perfectly evenly divisible by buffer size
	float pos = 0.0f;
	float time = 0.0f;
	bool frameToggle = false;

	Synth synth1 = s_InitPatch;

	short* buffer = new short[c_audioBufferSize];
	int bufferSize;

	float wavelengthArr[] = {
		Key(Note::C, 4), Key(Note::D, 4), Key(Note::E, 4), Key(Note::F, 4), Key(Note::G, 4), Key(Note::A, 4), Key(Note::B, 4),
		Key(Note::C, 5) };
	int noteCount = sizeof(wavelengthArr) / sizeof(float);
	int note = 0;

#pragma endregion

	while (!WindowShouldClose()) {

#pragma region Simulation phase
		
		if (!time)
		{
			printf("Change\n");
			wavelength = wavelengthArr[note = (note + 1) % noteCount];
			delete[] buffer;
			bufferSize = MostOf(wavelength, c_audioBufferSize);
			buffer = new short[bufferSize];
			FillAudioStream(stream, data, synth1, wavelength, 10000);
			memcpy(buffer, data, bufferSize * sizeof(short));
		}
		time += GetFrameTime();
		if (time > 1.0) time = 0.0;

		if (IsAudioStreamProcessed(stream))
		{
			UpdateAudioStream(stream, buffer, bufferSize);
			
			if (frameToggle = !frameToggle)
			{
				printf("\\\n");
			}
			else
			{
				printf("/\n");
			}
		}
		delay = (delay + 1) % (c_frameRate); // frame count

		pos += (GetFrameTime() * (c_sampleRate));
		if (pos > MostOf(wavelength, c_audioBufferSize)) pos = 0.0f; // frame count
		
#pragma endregion

#pragma region Drawing phase

		BeginDrawing(); {
			ClearBackground(BLACK);
			int bufferSize = MostOf(wavelength, c_audioBufferSize);
			int bufferSizeOnScreen = bufferSize / 4;
			int x = 1;
			DrawLine(0, windowHeight / 2, windowWidth, windowHeight / 2, RAYWHITE);
			DrawLine(bufferSizeOnScreen, 0, bufferSizeOnScreen, windowHeight, BLUE);
			DrawLine(c_audioBufferSize / 4, 0, c_audioBufferSize / 4, windowHeight, YELLOW);
			for (; x < c_audioBufferSize; ++x)
			{
				DrawLine(((x) / 4), ((((float)(data[x]) / 32000.0f) + 1.0f) / 2.0f) * ((float)windowHeight / 3.0f) + ((float)windowHeight / 3.0f),
						 ((x-1) / 4), ((((float)(data[x-1]) / 32000.0f) + 1.0f) / 2.0f) * ((float)windowHeight / 3.0f) + ((float)windowHeight / 3.0f),
						 RED);
			}
			DrawLine(pos / 4.0f, 0, pos / 4.0f, windowHeight, GREEN);
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
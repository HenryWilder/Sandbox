#include <raylib.h>
#include <raymath.h>
#include <string.h>
#include <random>
#include <thread>
#include <vector>
#include <iostream>
#include <string>
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
	C, D, E, F, G, A, B,
};
float Key(Note key, int octave = 4)
{
	float notes[] = {
		WaveLen(262),WaveLen(293.665),WaveLen(329.628),WaveLen(349.228),WaveLen(391.995),WaveLen(440),WaveLen(493.883),
		WaveLen(523.25), WaveLen(587.33) };
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

#pragma region Audio layering

struct Oscillator
{
	Oscillator() : base(nullptr), pitchScale(1.0f), pitchOffset(0.0f), amplitude(1.0f) {};
	Oscillator(WaveForm _base) : base(_base), pitchScale(1.0f), pitchOffset(0.0f), amplitude(1.0f) {};
	Oscillator(WaveForm _base, float _pitchScale, float _amplitude) : base(_base), pitchScale(_pitchScale), pitchOffset(0.0f), amplitude(_amplitude) {};
	Oscillator(WaveForm _base, float _pitchScale, float _pitchOffset, float _amplitude) : base(_base), pitchScale(_pitchScale), pitchOffset(_pitchOffset), amplitude(_amplitude) {};

	WaveForm base;
	float pitchScale, pitchOffset, amplitude;

	WAVEFORM(Function) // Must line up with type WaveForm
	{
		return base(x, (wavelength * pitchScale) + pitchOffset) * amplitude;
	}
};

struct OscUnit;

// USER INTERFACE

#include "GUI_Elements.h"

struct SynthCtrl
{
	SynthCtrl(OscUnit* _parent) : parent(_parent), collision({ 10.0f, 10.0f, 200.0f, 50.0f }) {};

	OscUnit* parent;
	Rectangle collision;

	void Draw();

	void SetPos(float newY)
	{
		collision.y = newY;
	}
};

struct OscUnit // Make sure the first synth is always ADDATIVE
{
	enum CombineType
	{
		Addative,
		Subtractive,
		Multiplicative,
	};
	static std::string CMBName(CombineType cmb)
	{
		switch (cmb)
		{
		case Addative:
			return "Addative";
			break;
		case Subtractive:
			return "Subtractive";
			break;
		case Multiplicative:
			return "Multiplicative";
			break;
		}
	}

	OscUnit() : osc(Oscillator()), cmb(Addative), ui(this) { };
	OscUnit(Oscillator _osc) : osc(_osc), cmb(Addative), ui(this) { };
	OscUnit(Oscillator _osc, CombineType _cmb) : osc(_osc), cmb(_cmb), ui(this) { };

	Oscillator osc;
	CombineType cmb;
	SynthCtrl ui;
};

void SynthCtrl::Draw()
{
	Color color = DARKGRAY;
	if (CheckCollisionPointRec(GetMousePosition(), collision)) color = GRAY;
	DrawRectangleRec(collision, color);
	DrawText(FormatText("Width: %f\nStrength: %f\nCmb: %s", parent->osc.pitchScale, parent->osc.amplitude, OscUnit::CMBName(parent->cmb).c_str()), collision.x + 5, collision.y + 5, 10, WHITE);
}

struct Synth
{
	Synth() : m_oscilators() {};
	Synth(std::vector<OscUnit> _oscs) : m_oscilators(_oscs) {};

	std::vector<OscUnit> m_oscilators;

	WAVEFORM(Function) // Must line up with type WaveForm
	{
		WFReturn_t out = (WFReturn_t)(0);
		for (OscUnit unit : m_oscilators)
		{
			WFReturn_t result = unit.osc.Function(x, wavelength);
			switch (unit.cmb)
			{
			case OscUnit::Addative:
				out += result;
				break;
			case OscUnit::Subtractive:
				out -= result;
				break;
			case OscUnit::Multiplicative:
				out *= result;
				break;
			}
		}
		return out;
	}
};

#pragma endregion

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
void FillAudioStreamChord(AudioStream& stream, short* out_buff, Synth synth, std::vector<WaveLength_t> chord, int volume = 32000)
{
	for (WavePos_t x = (WavePos_t)(0); x < c_audioBufferSize; x += (WavePos_t)(1))
	{
		out_buff[x] = 0;
		for (WaveLength_t note : chord)
		{
			out_buff[x] += (short)((synth.Function(x, note) * (float)volume));
		}
	}
}

bool IsKeyChanged(int key)
{
	return (IsKeyPressed(key) || IsKeyReleased(key));
}

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

	Synth synth1 = Synth({
		OscUnit(Oscillator(SineWave, 1.0f, 1.0f), OscUnit::Addative)
	});

	short* buffer = new short[c_audioBufferSize];
	int bufferSize;

	float wavelengthArr[] = {
		Key(Note::C, 4), Key(Note::D, 4), Key(Note::E, 4), Key(Note::F, 4), Key(Note::G, 4), Key(Note::A, 4), Key(Note::B, 4),
		Key(Note::C, 5) };

	std::vector<WaveLength_t> chord;
	int kbKeys[] = { KEY_A, KEY_S, KEY_D, KEY_F, KEY_J, KEY_K, KEY_L, KEY_SEMICOLON };
	int kbKeysChordLocs[sizeof(kbKeys) / sizeof(int)] = { -1 };

	int noteCount = sizeof(wavelengthArr) / sizeof(float);
	int note = 0;
	bool b_noteChanged = true;

#pragma endregion

	while (!WindowShouldClose()) {

#pragma region Simulation phase

#pragma region Keyboard

		{
			for (int i = 0; i < (sizeof(kbKeys) / sizeof(int)); ++i)
			{
				if (IsKeyChanged(kbKeys[i]))
				{
					b_noteChanged = true;

					if (IsKeyPressed(kbKeys[i]))
					{
						printf("Key pressed\n");
						chord.push_back(wavelengthArr[i]);
						kbKeysChordLocs[i] = chord.size() - 1;
						printf("kbKeysChordLocs[%i] = %i\n", i, kbKeysChordLocs[i]);
					}
					if (IsKeyReleased(kbKeys[i]))
					{
						printf("Key released\n");
						printf("kbKeysChordLocs[%i] (%i) Due to be erased\n", i, kbKeysChordLocs[i]);
						auto it = chord.begin() + kbKeysChordLocs[i];
						chord.erase(it, it+1);
						for (int& k : kbKeysChordLocs)
						{
							if (k > kbKeysChordLocs[i]) --k;
						}
						kbKeysChordLocs[i] = -1;
					}
				}
			}
			if (b_noteChanged)
			{
				int i = 0;
				for (WaveLength_t n : chord)
				{
					printf("chord[%i]: %f\n", i++, n);
				}
			}
		}

		if (IsKeyPressed(KEY_SPACE))
		{
			std::pair<std::string, WaveForm> options[] = { {"sine", SineWave}, {"square", SquareWave}, {"ramp", RampWave}, {"sawtooth", SawtoothWave}, {"noise", Noise}, {"triangle", TriangleWave}, };
			std::cout << "Waveform options:\n";
			for (std::pair<std::string, WaveForm> choice : options)
			{
				std::cout << choice.first.c_str() << "\n";
			}
			std::string phrase;
			std::cout << "> ";
			std::cin >> phrase;
			WaveForm pick = nullptr;
			for (std::pair<std::string, WaveForm> choice : options)
			{
				if (phrase == choice.first) pick = choice.second;
			}

			if (pick)
			{
				std::cout << "wavelength scale:\n> ";
				float scale;
				std::cin >> scale;
				std::cout << "Strength:\n> ";
				float strn;
				std::cin >> strn;

				synth1.m_oscilators.push_back(OscUnit(Oscillator(pick, scale, strn), OscUnit::Addative));
				synth1.m_oscilators[synth1.m_oscilators.size() - 1].ui.SetPos(10.0f + 60.0f * (float)(synth1.m_oscilators.size() - 1));
				b_noteChanged = true;
			}
			else
			{
				if (phrase == std::string("remove"))
				{
					synth1.m_oscilators.pop_back();
					std::cout << "Oscillator removed.\n";
				}
				else
				{
					std::cout << "Failed to find a match.\n";
				}
			}
		}
		
#pragma endregion

		if (b_noteChanged)
		{
			printf("Change\n");
			//wavelength = wavelengthArr[note = (note + 1) % noteCount];
			delete[] buffer;
			bufferSize = MostOf(wavelength, c_audioBufferSize);
			buffer = new short[bufferSize];
			FillAudioStreamChord(stream, data, synth1, chord, 10000);
			memcpy(buffer, data, bufferSize * sizeof(short));
		}
		time += GetFrameTime();
		if (time > 1.0) time = 0.0;
		b_noteChanged = false;

		if (IsAudioStreamProcessed(stream))
		{
			UpdateAudioStream(stream, buffer, bufferSize);
			
			//if (frameToggle = !frameToggle)
			//{
			//	printf("\\\n");
			//}
			//else
			//{
			//	printf("/\n");
			//}
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
			//DrawLine(pos / 4.0f, 0, pos / 4.0f, windowHeight, GREEN);

			for (OscUnit element : synth1.m_oscilators)
			{
				element.ui.Draw();
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
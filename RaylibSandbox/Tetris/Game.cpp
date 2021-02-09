#include <raylib.h>
#include <raymath.h>
#include <string.h>
#include <random>
#include <thread>
#include <vector>
#include <iostream>
#include <string>
#include "AmyMath.h"

constexpr int c_audioBufferSize = 4096;
constexpr int c_sampleRate = 41000;

namespace aux
{
	enum class MOD : int {
		Add,
		Multiply,
		Subtract,
	};
	const char* GetModName(MOD mod)
	{
		switch (mod) {
		default: return "ERROR";

		case MOD::Add:		return "add";
		case MOD::Multiply:	return "mult";
		case MOD::Subtract: return "sub";
		}
	}

	enum class OSC : int {
		Sine,
		Square,
		Sawtooth,
		Ramp,
		Triangle,
		Noise,

		Total
	};
	const char* GetOscName(OSC osc)
	{
		switch (osc) {
		default: return "ERROR";

		case OSC::Sine:		return "sine";
		case OSC::Square:	return "square";
		case OSC::Sawtooth: return "sawtooth";
		case OSC::Ramp:		return "ramp";
		case OSC::Triangle: return "triangle";
		case OSC::Noise:	return "noise";
		}
	}

	enum class EFF : int {
		Reverb,
		Equalize,
		Compress,

		Total
	};
	const char* GetEffectName(EFF effect)
	{
		switch (effect) {
		default: return "ERROR";

		case EFF::Reverb:	return "reverb";
		case EFF::Equalize:	return "equalize";
		case EFF::Compress: return "compress";
		}
	}

	struct OSC_MOD
	{
	public:
		OSC_MOD(OSC type) {
			const char* oscName = GetOscName(type);
			for (int i = 0; i < 3; ++i) {
				mods[i] = LoadShader("wavenode.vert", TextFormat("osc_%s_%s.frag", oscName, GetModName(MOD(i))));
			}
		}

		Shader& operator[](MOD mod)
		{
			return mods[(int)(mod)];
		}

		void Unload()
		{
			for (Shader m : mods) {
				UnloadShader(m);
			}
		}

	private:
		Shader mods[3];
	};

	OSC_MOD oscillators[(int)(OSC::Total)]; // Shader = oscillators[int][MOD]
	Shader effects[(int)(EFF::Total)]; // Shader = effects[int]

	void Load()
	{
		// Oscillators 
		for (int i = 0; i < (int)(OSC::Total); ++i) {
			oscillators[i] = OSC_MOD(OSC(i));
		}
		// Effects
		for (int i = 0; i < (int)(EFF::Total); ++i) {
			effects[i] = LoadShader("wavenode.vert", TextFormat("eff_%s.frag", GetEffectName(EFF(i))));
		}
	}
	void Unload()
	{
		for (OSC_MOD osc : oscillators) osc.Unload();
		for (Shader eff : effects) UnloadShader(eff);
	}
}

int MostOf(int cycleSize, int maxSpace)
{
	return (maxSpace / cycleSize) * cycleSize;
}

float WaveLen(float freq)
{
	return c_sampleRate / freq;
}

enum class Note { C, D, E, F, G, A, B, };
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

typedef int WavePos_t; typedef float WaveLength_t;

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

Shader* ShaderOf(WaveForm wave);

#pragma endregion

#pragma region Audio layering

#pragma region Effect classes

// Base class
class Effect
{
public:

	Effect() : shader(nullptr) {};

	virtual void InitShader(Shader* _shader) = 0;

	virtual void Apply(RenderTexture& input, RenderTexture& output) = 0;

protected:

	Shader* shader;
};

class Compressor : public Effect
{
public:

	void Apply(RenderTexture& input, RenderTexture& output) override
	{
		BeginTextureMode(output); {

		} EndTextureMode();
	}

	void InitShader(Shader* _shader) override
	{

	}

private:

	static Shader* s_shader;
};

class Equalizer : public Effect
{
public:

	Equalizer(float _f0, float _q) {
		Effect::shader = s_shader;
		UpdateParams(_f0, _q);
	}

	void UpdateParams(float _f0, float _q)
	{
		f0 = _f0; 
		q = _q;

		float w0 = 2 * PI * f0 / c_sampleRate;
		float alpha = sinf(w0) / (2 * q);

		a[0] = (1 + alpha);
		a[1] = (-2 * cosf(w0));
		a[2] = (1 - alpha);

		b[0] = ((1 + cosf(w0)) / 2);
		b[1] = (-(1 + cosf(w0)));
		b[2] = ((1 + cosf(w0)) / 2);

		float a0 = 1 / a[0];
		for (float& _a : a) { _a *= a0; }
		for (float& _b : b) { _b *= a0; }
	}

	void Prepend(float(&arr)[3], float elem)
	{
		for (int i = 2; i > 0; --i) {
			arr[i] = arr[i - 1];
		}
		arr[0] = elem;
	}

	void Apply(RenderTexture& input, RenderTexture& output) override
	{
		BeginTextureMode(output); {

			// TODO: Make this work for the GPU

		} EndTextureMode();
	}

private:

	// Parameters
	static Shader* s_shader;
	float f0; // Cut-off (or center) frequency in Hz
	float q; // Filter Q

	// Uniforms produced from the parameters
	float a[3]; // Fi
	float b[3];
};
Shader* Equalizer::s_shader;

class Reverb : public Effect
{
public:
	// Parameters
	Reverb() {

	}

	void Apply(RenderTexture& input, RenderTexture& output) override
	{
		BeginTextureMode(output); {

		} EndTextureMode();
	}

private:
	static Shader* s_shader;
};
Shader* Reverb::s_shader;

#pragma endregion

struct Oscillator
{
	Oscillator() : base(nullptr), shader(nullptr), pitchScale(1.0f), pitchOffset(0.0f), amplitude(1.0f) {};
	Oscillator(WaveForm _base) : base(_base), shader(_shader), pitchScale(1.0f), pitchOffset(0.0f), amplitude(1.0f) {};
	Oscillator(WaveForm _base, float _pitchScale, float _amplitude) : base(_base), shader(_shader), pitchScale(_pitchScale), pitchOffset(0.0f), amplitude(_amplitude) {};
	Oscillator(WaveForm _base, float _pitchScale, float _pitchOffset, float _amplitude) : base(_base), shader(_shader), pitchScale(_pitchScale), pitchOffset(_pitchOffset), amplitude(_amplitude) {};

	WaveForm base;
	Shader* shader;
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

Shader* ShaderOf(WaveForm wave)
{
	return nullptr;
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
	bool b_audioChanged = true;

	RenderTexture target = LoadRenderTexture(c_audioBufferSize, 1);

#pragma endregion

	while (!WindowShouldClose()) {

	#pragma region Simulation Phase

		#pragma region Keyboard

		{
			for (int i = 0; i < (sizeof(kbKeys) / sizeof(int)); ++i)
			{
				if (IsKeyChanged(kbKeys[i]))
				{
					b_audioChanged = true;

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
			if (b_audioChanged)
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
				b_audioChanged = true;
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

		#pragma region Update vars

		if (b_audioChanged)
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
		b_audioChanged = false;

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

		#pragma region Audio Rendering

		if (b_audioChanged) // TODO: GPU rendering of audio file
		{
			BeginTextureMode(target); {

				BeginShaderMode(sine_multiply); {

					DrawTextureRec(target.texture, Rectangle{ 0.0f, 0.0f, (float)c_audioBufferSize, 0.0f }, Vector2{ 0,0 }, WHITE);

				} EndShaderMode();

			} EndTextureMode();
		}

		#pragma endregion

	#pragma endregion

	#pragma region Drawing Phase

		BeginDrawing(); {

			ClearBackground(BLACK);

			#pragma region Waves

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

			#pragma endregion

			#pragma region UI

			for (OscUnit element : synth1.m_oscilators)
			{
				element.ui.Draw();
			}

			#pragma endregion

		} EndDrawing();

	#pragma endregion

	} CloseWindow();

#pragma region Cleanup phase

	CloseAudioStream(stream);
	CloseAudioDevice();

	delete[] data;
	delete[] buffer;

	aux::Unload();
		
	UnloadRenderTexture(target);

#pragma endregion

	return 0;
}

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
constexpr int c_sampleRate = 48000;

static const char* s_baseVertShader = { R"(#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;

// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord;
out vec4 fragColor;

void main()
{
// Send vertex attributes to fragment shader
fragTexCoord = vec2(vertexTexCoord.x, 0);
fragColor = vertexColor;

vec3 pos = vertexPosition;

%s

// Calculate final vertex position
gl_Position = mvp * vec4(pos, 1.0);
})" };
static const char* s_baseFragShader = { R"(#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform heights
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// Custom uniforms
%s

highp float rand(vec2 co)
{
highp float a = 12.9898;
highp float b = 78.233;
highp float c = 43758.5453;
highp float dt= dot(co.xy ,vec2(a,b));
highp float sn= mod(dt,3.14);
return fract(sin(sn) * c);
}

void main()
{
float time = fragTexCoord.x;

float amp = 0.0;

%s

finalColor = vec4(amp, vec2(0.0), 1.0);
})" };
static const char* s_baseCmbShader = { R"(#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform heights
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform sampler2D texture1;

// Output fragment color
out vec4 finalColor;

void main()
{
float a = texture2D(texture0, fragTexCoord).x;
float b = texture2D(texture1, fragTexCoord).x;

float amp = 0.0;

%s

finalColor = vec4(amp, vec2(0.0), 1.0);
})" };
static const char* s_baseEffShader = { R"(#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform heights
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform float totalSize = float(1024);

// Output fragment color
out vec4 finalColor;

void main()
{
float time = fragTexCoord.x;
float t = 1 / totalSize;
float source = texture2D(texture0, vec2(t,0)).x;

float amp = 0.0;

%s

finalColor = vec4(amp, vec2(0.0), 1.0);
})" };

const char* BuildVertShader(const char* uniformInserts = "", const char* posCode = "")
{
	const char* out = TextFormat(
		R"(#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;

// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord;
out vec4 fragColor;

// Custom uniforms
%s

void main()
{
// Send vertex attributes to fragment shader
fragTexCoord = vec2(vertexTexCoord);
fragColor = vertexColor;

// Pos code
vec3 pos = vertexPosition;
%s

// Calculate final vertex position
gl_Position = mvp*vec4(pos, 1.0);
})",
uniformInserts,
posCode
	);

	//printf("\n-------------------------\nVertex shader code built!\n-------------------------\n\n%s\n~~~~~~~~~~~~\n\n", out);

	return out;
}
const char* BuildFragShader(const char* uniformInserts = "", const char* ampCode = "",  bool b_usesRand = false)
{
	const char* out = TextFormat(
		R"(#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform heights
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// Custom uniforms
%s
%s
void main()
{
float time = fragTexCoord.x;

finalColor = vec4(vec3(0.0), 1.0);

// Amp code
float amp = 0.0;
%s

finalColor.x = amp;
})",
		uniformInserts,
		(b_usesRand ?
			R"(
// rand()
highp float rand(vec2 co)
{
highp float a = 12.9898;
highp float b = 78.233;
highp float c = 43758.5453;
highp float dt = dot(co.xy, vec2(a, b));
highp float sn = mod(dt, 3.14);
return fract(sin(sn) * c);
}
)"
			: ""),
		ampCode
	);

	//printf("\n---------------------------\nFragment shader code built!\n---------------------------\n\n%s\n~~~~~~~~~~~~\n\n", out);

	return out;
}

namespace aux
{
	enum class CMB : int {
		Add,
		Multiply,
		Subtract,
		Average,
		Mix,

		Total
	};
	const char* GetCmbName(CMB cmb)
	{
		switch (cmb) {
		default: return "ERROR";

		case CMB::Add:		return "add";
		case CMB::Multiply:	return "mult";
		case CMB::Subtract: return "sub";
		case CMB::Average: return "ave";
		case CMB::Mix: return "mix";
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
		Delay,

		Total
	};
	const char* GetEffName(EFF effect)
	{
		switch (effect) {
		default: return "ERROR";

		case EFF::Reverb:	return "reverb";
		case EFF::Equalize:	return "equalize";
		case EFF::Compress: return "compress";
		case EFF::Delay: return "delay";
		}
	}

	Shader oscillators[(int)(OSC::Total)];
	Shader combines[(int)(CMB::Total)];
	Shader effects[(int)(EFF::Total)];

	void Load()
	{
		// Oscillators 
		for (int o = 0; o < (int)(OSC::Total); ++o) {
			oscillators[o] = LoadShader(
				"v_osc.vert",
				TextFormat("osc_%s.frag", GetOscName(OSC(o)))
			);
		}
		// Combines
		for (int c = 0; c < (int)(CMB::Total); ++c) {
			combines[c] = LoadShader(
				"v_cmb.vert",
				TextFormat("cmb_%s.frag", GetCmbName(CMB(c)))
			);
		}
		// Effects
		for (int e = 0; e < (int)(EFF::Total); ++e) {
			effects[e] = LoadShader(
				"v_eff.vert",
				TextFormat("eff_%s.frag", GetEffName(EFF(e)))
			);
		}
	}
	void Unload()
	{
		for (Shader osc : oscillators)	UnloadShader(osc);
		for (Shader cmb : combines)		UnloadShader(cmb);
		for (Shader eff : effects)		UnloadShader(eff);
	}
}

namespace render
{

}

float WaveLen(float freq)
{
	return c_sampleRate / freq;
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
	const char* timingCodeInsert;
	const char* oscCodeInsert;
};

// USER INTERFACE
//#include "GUI_Elements.h"

struct OscUnit : private Oscillator
{
	const char* combineCodeInsert;
	float pitchScale, ampScale;
};

class Synth
{
	Synth() : m_oscilators({ OscUnit() }) {};
	Synth(const std::vector<OscUnit>& _oscs) : m_oscilators(_oscs) {};

	std::vector<OscUnit> m_oscilators;
};

#pragma endregion

bool IsKeyChanged(int key)
{
	return (IsKeyPressed(key) || IsKeyReleased(key));
}

int main() {

#pragma region Setup phase

	int windowWidth = 1280;
	int windowHeight = 720;

	Rectangle canvas; {
		canvas.x = 0;
		canvas.y = 0;
		canvas.width = windowWidth;
		canvas.height = windowHeight;
	}

	InitWindow(windowWidth, windowHeight, "A program I'm making in C++");
	constexpr int c_frameRate = 30;
	SetTargetFPS(c_frameRate);

	Shader shader = LoadShaderCode(
		BuildVertShader(),
		BuildFragShader(
			// Uniforms
			"",
			// Code
			"amp = sin(radians(360.0) * time);"
			"finalColor.y = (abs(fragTexCoord.y - amp) < 0.001 ? 1.0 : 0.0);",
			// Include rand() function?
			false
		)
	);

	Shader display = LoadShaderCode(
		BuildVertShader(),
		BuildFragShader(
			// Uniforms
			"",
			// Code
			"float fragInputVal = texture2D(texture0, fragTexCoord).x;"
			"float dist = abs(fragInputVal + 0.5 - fragTexCoord.y);"
			"amp = texture2D(texture0, vec2(fragTexCoord.x * 3.2, fragTexCoord.y)).x;"
			"finalColor.y = (abs(fragTexCoord.y - amp) < 0.001 ? 1.0 : 0.0);"
		)
	);

	RenderTexture target = LoadRenderTexture(c_audioBufferSize, windowHeight);

	while (!WindowShouldClose())
	{
		BeginTextureMode(target); {

			ClearBackground(BLACK);

			BeginShaderMode(shader); {

				DrawTextureRec(target.texture, Rectangle{ 0, 0, (float)c_audioBufferSize, (float)windowHeight }, { 0,0 }, WHITE);

			} EndShaderMode();

		} EndTextureMode();

		BeginDrawing(); {

			BeginShaderMode(display); {

				DrawTextureRec(target.texture, Rectangle{ 0, 0, (float)windowWidth, (float)windowHeight }, { 0,0 }, WHITE);

			} EndShaderMode();

		} EndDrawing();
	}

	UnloadRenderTexture(target);

	UnloadShader(display);

	UnloadShader(shader);

	CloseWindow();

	return 0;

//	InitAudioDevice();
//	AudioStream stream = InitAudioStream(c_sampleRate, 8 * sizeof(short), 1);
//	short* data = new short[c_audioBufferSize];
//	PlayAudioStream(stream);
//
//	aux::Load();
//
//	LoadShaderCode();
//
//	bool b_audioChanged = true;
//
//	RenderTexture target = LoadRenderTexture(c_audioBufferSize, 1);
//
//#pragma endregion
//
//	while (!WindowShouldClose()) {
//
//	#pragma region Simulation Phase
//
//		#pragma region Keyboard
//
//		{
//			for (int i = 0; i < (sizeof(kbKeys) / sizeof(int)); ++i)
//			{
//				if (IsKeyChanged(kbKeys[i]))
//				{
//					b_audioChanged = true;
//
//					if (IsKeyPressed(kbKeys[i]))
//					{
//						printf("Key pressed\n");
//						chord.push_back(wavelengthArr[i]);
//						kbKeysChordLocs[i] = chord.size() - 1;
//						printf("kbKeysChordLocs[%i] = %i\n", i, kbKeysChordLocs[i]);
//					}
//					if (IsKeyReleased(kbKeys[i]))
//					{
//						printf("Key released\n");
//						printf("kbKeysChordLocs[%i] (%i) Due to be erased\n", i, kbKeysChordLocs[i]);
//						auto it = chord.begin() + kbKeysChordLocs[i];
//						chord.erase(it, it+1);
//						for (int& k : kbKeysChordLocs)
//						{
//							if (k > kbKeysChordLocs[i]) --k;
//						}
//						kbKeysChordLocs[i] = -1;
//					}
//				}
//			}
//			if (b_audioChanged)
//			{
//				int i = 0;
//				for (WaveLength_t n : chord)
//				{
//					printf("chord[%i]: %f\n", i++, n);
//				}
//			}
//		}
//
//		if (IsKeyPressed(KEY_SPACE))
//		{
//			std::pair<std::string, WaveForm> options[] = { {"sine", SineWave}, {"square", SquareWave}, {"ramp", RampWave}, {"sawtooth", SawtoothWave}, {"noise", Noise}, {"triangle", TriangleWave}, };
//			std::cout << "Waveform options:\n";
//			for (std::pair<std::string, WaveForm> choice : options)
//			{
//				std::cout << choice.first.c_str() << "\n";
//			}
//			std::string phrase;
//			std::cout << "> ";
//			std::cin >> phrase;
//			WaveForm pick = nullptr;
//			for (std::pair<std::string, WaveForm> choice : options)
//			{
//				if (phrase == choice.first) pick = choice.second;
//			}
//
//			if (pick)
//			{
//				std::cout << "wavelength scale:\n> ";
//				float scale;
//				std::cin >> scale;
//				std::cout << "Strength:\n> ";
//				float strn;
//				std::cin >> strn;
//
//				synth1.m_oscilators.push_back(OscUnit());
//				synth1.m_oscilators[synth1.m_oscilators.size() - 1].ui.SetPos(10.0f + 60.0f * (float)(synth1.m_oscilators.size() - 1));
//				b_audioChanged = true;
//			}
//			else
//			{
//				if (phrase == std::string("remove"))
//				{
//					synth1.m_oscilators.pop_back();
//					std::cout << "Oscillator removed.\n";
//				}
//				else
//				{
//					std::cout << "Failed to find a match.\n";
//				}
//			}
//		}
//		
//		#pragma endregion
//
//		#pragma region Update vars
//		
//		// TODO
//
//		#pragma endregion
//
//		#pragma region Audio Rendering
//
//		// TODO
//
//		#pragma endregion
//
//	#pragma endregion
//
//	#pragma region Drawing Phase
//
//		BeginDrawing(); {
//
//			ClearBackground(BLACK);
//
//			#pragma region Waves
//
//			DrawTextureRec();
//
//			int bufferSize = MostOf(wavelength, c_audioBufferSize);
//			int bufferSizeOnScreen = bufferSize / 4;
//			int x = 1;
//			DrawLine(0, windowHeight / 2, windowWidth, windowHeight / 2, RAYWHITE);
//			DrawLine(bufferSizeOnScreen, 0, bufferSizeOnScreen, windowHeight, BLUE);
//			DrawLine(c_audioBufferSize / 4, 0, c_audioBufferSize / 4, windowHeight, YELLOW);
//			for (; x < c_audioBufferSize; ++x)
//			{
//				DrawLine(((x) / 4), ((((float)(data[x]) / 32000.0f) + 1.0f) / 2.0f) * ((float)windowHeight / 3.0f) + ((float)windowHeight / 3.0f),
//						 ((x-1) / 4), ((((float)(data[x-1]) / 32000.0f) + 1.0f) / 2.0f) * ((float)windowHeight / 3.0f) + ((float)windowHeight / 3.0f),
//						 RED);
//			}
//			//DrawLine(pos / 4.0f, 0, pos / 4.0f, windowHeight, GREEN);
//
//			#pragma endregion
//
//			#pragma region UI
//
//			for (OscUnit element : synth1.m_oscilators)
//			{
//				element.ui.Draw();
//			}
//
//			#pragma endregion
//
//		} EndDrawing();
//
//	#pragma endregion
//
//	} CloseWindow();
//
//#pragma region Cleanup phase
//
//	CloseAudioStream(stream);
//	CloseAudioDevice();
//
//	delete[] data;
//	delete[] buffer;
//
//	aux::Unload();
//		
//	UnloadRenderTexture(target);
//
//#pragma endregion
//
//	return 0;
}

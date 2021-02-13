#pragma once
#include <raylib.h>
#include <unordered_map>
#include <map>
#include <any>

Color ColorTint(Color color, Color tint);

Rectangle GetShape(Rectangle rec);
Vector2 GetPos(Rectangle rec);

extern Texture g_defaultTexture;

struct GUIMat
{
	GUIMat(Color color) : type(MatType::Mat_Color), data(), src({ 0,0,0,0 }) { data.c = color; };
	GUIMat(Texture texture, Rectangle _src) : type(MatType::Mat_Texture), data(), src(_src) { data.t = texture;  };
	GUIMat(Material material, Rectangle _src) : type(MatType::Mat_Material), data(), src(_src) { data.m = material;  };
	GUIMat(const char* vertexShaderCode, const char* fragmentShaderCode) : type(MatType::Mat_Material), data(), src({ 0,0,0,0 }) {
		data.m.maps = nullptr;
		data.m.params = nullptr;
		data.m.shader = LoadShaderCode();
	};

	enum class MatType
	{
		Mat_Color, Mat_Texture, Mat_Material, Mat_Shader,
	};
	MatType type;

	union MatVariant
	{
		Color c;
		Texture t;
		Material m;
		Shader s;
	};
	MatVariant data;

	Rectangle src;
};
void UnloadGUIMat(GUIMat mat);
void DrawGUIMat(GUIMat mat, Rectangle dest, Vector2 origin, float rotation = 0.0f, Color tint = WHITE);

struct GUIMatElement
{
	GUIMatElement(Rectangle _drawSpace, GUIMat _material, Color _tint = WHITE) : drawSpace(_drawSpace), material(_material), tint(_tint) {};

	Rectangle drawSpace;
	GUIMat material;
	Color tint;
};

template<class stateEnum>
struct MatStateMachine
{
	MatStateMachine(std::map<stateEnum, GUIMatElement> _mat)
		: mat(_mat) {};

	std::map<stateEnum, GUIMatElement> mat;
};
template<class stateEnum>
GUIMatElement GetStateMaterial(MatStateMachine<stateEnum> stateMachine, stateEnum state)
{
	return stateMachine.mat.find(state)->second;
}


enum class ButtonStateFlags : unsigned char {
	Up = 0b00,
	Down = 0b01,
	Released = 0b10,
	Pressed = 0b11,

	Hovered = 0b100,
};
ButtonStateFlags operator|(ButtonStateFlags lval, ButtonStateFlags rval);
ButtonStateFlags& operator|=(ButtonStateFlags& lval, ButtonStateFlags rval);
ButtonStateFlags operator&(ButtonStateFlags lval, ButtonStateFlags rval);

bool IsButtonStateUp(ButtonStateFlags state);
bool IsButtonStateDown(ButtonStateFlags state);
bool IsButtonStateActive(ButtonStateFlags state);
bool IsButtonStatePassive(ButtonStateFlags state);
bool IsButtonStateHovered(ButtonStateFlags state);

struct GUIButton
{
	GUIButton(Rectangle _collision,
			  GUIMat defaultMat,
			  GUIMat hoveredMat,
			  GUIMat pressedMat,
			  GUIMat disabledMaterial,
			  bool startDisabled = false,
			  Color tint = WHITE) :
		collision(_collision),
		state(ButtonStateFlags::Up),
		mat_default(defaultMat),
		mat_hovered(hoveredMat),
		mat_pressed(pressedMat),
		mat_disabled(disabledMaterial),
		tint(tint),
		b_disabled(startDisabled) {};

	Rectangle collision;
	ButtonStateFlags state;
	bool b_disabled;
	MatStateMachine stateMaterials;
};
ButtonStateFlags GetButtonState(GUIButton button);

GUIMat& GetButtonDrawMat(GUIButton button, ButtonStateFlags state);
void DrawGUIButton(GUIButton button, ButtonStateFlags state);

struct GUISlider
{
	Rectangle collision;
	ButtonStateFlags state;
	GUIMat
		mat_default,
		mat_hovered,
		mat_pressed,
		mat_disabled;
	Color tint;
	bool b_disabled;

	Vector2 minPoint;
	Vector2 maxPoint;
	float value;
	float rangeMin, rangeMax;
};
struct GUIDial
{
	GUIButton dial;
	float rotation;
};
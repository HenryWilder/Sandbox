#pragma once
#include "HenryMath.h"

struct Ramp
{
	Ramp();
	Ramp(ColorRGB color1, ColorRGB color2);
	Ramp(ColorRGB color1, ColorRGB color2, ColorRGB color3);
	Ramp(ColorRGB color1, ColorRGB color2, ColorRGB color3, ColorRGB color4, ColorRGB color5);

	// The colors being interpolated between
	ColorRGB m_colorNode[5];

	ColorRGB Value(double value);
};

namespace pltt {

	enum class Palette {
		Black = 0,
		White = 1,
		Select = 2,
		Board = 3,
		Text = 4,
	};

	extern Ramp black;
	extern Ramp white;
	extern Ramp select;
	extern Ramp board;
	extern Ramp text;
	extern Ramp err;

	Ramp* PickPalette(Palette palette);
}

ColorRGB ColorRamp(double value, pltt::Palette palette);

struct VectorGraphic;

template <int sideLen = 16> // Default is 16x16
struct RasterGraphic
{
	int m_width = sideLen;
	double m_pixels[sideLen];

	int Valid(int x, int y);
	int Index(int x, int y);

public:
	int GetGray(int x, int y);
	int GetColor(int x, int y);
private:
	friend struct VectorGraphic;
	void Set(int index, double val);
	void Set(int x, int y, double val);
};

struct BezierNode
{
	Vec2 m_pos; // We kinda want these to be 0-1 inclusive only because it makes upscaling easier

	// Weights
	float m_wIn;
	float m_wOut;

	double m_nodeValue; // Vertex shading
};

// Any closed shape that may be disconnected from another closed shape in a vector graphic
// The final node in the array != to the first one, the first and last nodes will be interpolated between
struct VecSubGraphic
{
	int m_totalNodes;
	BezierNode m_nodes[32]; // Should be an array of largest number of nodes we will use in a sub-graphic

	double InterpShade(Vec2 pos); // Only interpolates the shade of up to the 3 closest nodes per pixel
	Rect Bounds(); // Outputs 0-1 inclusive. This is a function instead of a variable only because it's dependent on the nodes
};

// @Note to self: the vector graphics are *2D*, shading must be defined as sub graphics
struct VectorGraphic
{
	int m_totalGraphics;
	// Should be an array of largest number of sub-graphics we will use in the project
	// Graphic layer is the same as the index in the array
	VecSubGraphic m_shapes[8];

	template <int size>
	void Rasterize(RasterGraphic<size>& texture);
};

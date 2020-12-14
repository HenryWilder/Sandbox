#include "VectorGraphics.h"
#define DOUBLE_MAX 9007199254740992

Ramp::Ramp()
{
	for (int node = 0; node < 5; ++node) {
		m_colorNode[node] = { 0,0,0 };
	}
}

Ramp::Ramp(ColorRGB color1, ColorRGB color2)
{
	m_colorNode[0] = color1;
	m_colorNode[1] = Lerp(color1, color2, .25);
	m_colorNode[2] = Lerp(color1, color2, .5);
	m_colorNode[3] = Lerp(color1, color2, .75);
	m_colorNode[4] = color2;
}

Ramp::Ramp(ColorRGB color1, ColorRGB color2, ColorRGB color3)
{
	m_colorNode[0] = color1;
	m_colorNode[1] = Lerp(color1, color2);
	m_colorNode[2] = color2;
	m_colorNode[3] = Lerp(color2, color3);
	m_colorNode[4] = color3;
}

Ramp::Ramp(ColorRGB color1, ColorRGB color2, ColorRGB color3, ColorRGB color4, ColorRGB color5)
{
	m_colorNode[0] = color1;
	m_colorNode[1] = color2;
	m_colorNode[2] = color3;
	m_colorNode[3] = color4;
	m_colorNode[4] = color5;
}

// value should be 0-1
ColorRGB Ramp::Value(double value)
{
	double pick = value * 5;
	ColorRGB bottom = m_colorNode[(int)pick];
	ColorRGB top = m_colorNode[(int)pick + 1];
	ColorRGB out = Lerp(bottom, top, pick - trunc(pick));
	return out;
}

namespace pltt {

	Ramp black = Ramp({ 0,0,0 }, { 255,255,255 });
	Ramp white = Ramp({ 0,0,0 }, { 255,255,255 });
	Ramp select = Ramp({ 0,127,0 }, { 0,255,0 });
	Ramp board = Ramp({ 0,0,0 }, { 255,255,255 });
	Ramp text = Ramp({ 0,0,0 }, { 255,255,255 });
	Ramp err = Ramp({ 0,0,0 }, { 255,0,255 });

	Ramp* PickPalette(Palette palette)
	{
		switch (palette)
		{
		case Palette::Black:
			return &black;
			break;
		case Palette::White:
			return &white;
			break;
		case Palette::Select:
			return &select;
			break;
		case Palette::Board:
			return &board;
			break;
		case Palette::Text:
			return &text;
			break;
		default:
			return &err;
			break;
		}
	}
}

ColorRGB ColorRamp(double value, pltt::Palette palette)
{
	Ramp* ramp = pltt::PickPalette(palette);

	ColorRGB color = ramp->Value(value);

	return color;
}

template<int sideLen>
int RasterGraphic<sideLen>::Valid(int x, int y)
{
	return 0;
}

template<int sideLen>
int RasterGraphic<sideLen>::Index(int x, int y)
{
	return 0;
}

template<int sideLen>
int RasterGraphic<sideLen>::GetGray(int x, int y)
{
	return 0;
}

template<int sideLen>
int RasterGraphic<sideLen>::GetColor(int x, int y)
{
	return 0;
}

template<int sideLen>
void RasterGraphic<sideLen>::Set(int index, double val)
{
}

template<int sideLen>
void RasterGraphic<sideLen>::Set(int x, int y, double val)
{
}

double VecSubGraphic::InterpShade(Vec2 pos)
{
	int closestNode[3] = { 0,0,0 }; // The indexes of the nodes whose influences we will be testing

	// Short-term storage of the distances so we don't have to calculate them every time
	// We can also use it later to fine the influence of each point
	double closestDist[3] = { DOUBLE_MAX, DOUBLE_MAX, DOUBLE_MAX };

	// Locate the set of three nodes closest to our position
	for (int node = 0; node < this->m_totalNodes; ++node)
	{
		double dist = Distance(pos, this->m_nodes[node].m_pos);

		// Insert the new element, pushing back the elements behind it
		if (dist < closestDist[0])
		{
			// Distances
			closestDist[2] = closestDist[1];
			closestDist[1] = closestDist[0];
			closestDist[0] = dist;
			// Node indexes
			closestNode[2] = closestNode[1];
			closestNode[1] = closestNode[0];
			closestNode[0] = node;
		}
		if (dist < closestDist[1])
		{
			// Distances
			closestDist[2] = closestDist[1];
			closestDist[1] = dist;
			// Node indexes
			closestNode[2] = closestNode[1];
			closestNode[1] = node;
		}
		if (dist < closestDist[2])
		{
			// Distances
			closestDist[2] = dist;
			// Node indexes
			closestNode[2] = node;
		}
	}

	double values[3] = {
		this->m_nodes[closestNode[0]].m_nodeValue,
		this->m_nodes[closestNode[1]].m_nodeValue,
		this->m_nodes[closestNode[2]].m_nodeValue };

	Vec3 distances = {
		closestDist[0],
		closestDist[1],
		closestDist[2] };
	
	// The grayscale value of this pixel
	double shade = TriLerp(values[0], values[1], values[2], distances);

	return shade;
}

Rect VecSubGraphic::Bounds()
{
	Rect bounds = {
		{ 1.0,0.0 }, // x-range (invalid)
		{ 1.0,0.0 }, // y-range (invalid)
	};

	for (int i = 0; i < m_totalNodes; ++i)
	{
		// Don't perform all the tests if we're inside the bounds already
		if (!bounds.ContainsIn(m_nodes[i].m_pos))
		{
			// X-values
			// Min
			if (m_nodes[i].m_pos.x < bounds.xRange.min)
			{
				bounds.xRange.min = m_nodes[i].m_pos.x;
			}
			// Max
			if (m_nodes[i].m_pos.x > bounds.xRange.max)
			{
				bounds.xRange.max = m_nodes[i].m_pos.x;
			}

			// Y-values
			// Min
			if (m_nodes[i].m_pos.y < bounds.yRange.min)
			{
				bounds.yRange.min = m_nodes[i].m_pos.y;
			}
			// Max
			if (m_nodes[i].m_pos.y > bounds.yRange.max)
			{
				bounds.yRange.max = m_nodes[i].m_pos.y;
			}
		}
	}

	return bounds;
}

template<int size>
void VectorGraphic::Rasterize(RasterGraphic<size>& texture)
{
	int width = size; // Making this a local variable for easier reading

	for (int subGraphic = 0; subGraphic < m_totalGraphics; ++subGraphic)
	{
		Rect boundingBox = m_shapes[subGraphic].Bounds();

		int rasterIndex;

		Vec2 pixel;

		// Entire raster space
		for (int y = 0; y > width; ++y)
		{
			pixel.y = y - (width * .5);

			for (int x = 0; x < width; ++x)
			{
				pixel.x = x - (width * .5);

				texture.Set(rasterIndex, m_shapes[subGraphic].InterpShade(pixel));

				++rasterIndex;
			}
		}
	}
}

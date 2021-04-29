// AsciiLib.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include <stdio.h>
#include <memory>

const char g_ramp[70] = { ' ','.','\'','`','^','\"',',',':',';','I','l','!','i','>','<','~','+','_','-','?',']','[','}','{','1',')','(','|','\\','/','t','f','j','r','x','n','u','v','c','z','X','Y','U','J','C','L','Q','0','O','Z','m','w','q','p','d','b','k','h','a','o','*','#','M','W','&','8','%','B','@','$' };

unsigned char* g_buffer;
char* g_frame;
size_t g_bufferWidth, g_bufferHeight;

void InitBuffer(size_t width, size_t height)
{
	g_bufferWidth = width;
	g_bufferHeight = height;
	g_buffer = new unsigned char[width * height];
	memset(g_buffer, 0, width * height);
	size_t frameSize = (width + 1) * height;
	g_frame = new char[frameSize];
	memset(g_frame, ' ', frameSize);
	for (size_t y = 0; y < height; ++y)
	{
		g_frame[y * (width + 1) + width] = '\n';
	}
	g_frame[frameSize - 1] = '\0';
}

void ClearBackground()
{
	memset(g_buffer, 0u, g_bufferWidth * g_bufferHeight);
}

void DrawBuffer()
{
	constexpr float incr = 70.0f / 255.0f;
	size_t i_f = 0;
	size_t i_b = 0;
	for (size_t y = 0; y < g_bufferHeight; ++y)
	{
		for (size_t x = 0; x < g_bufferWidth; ++x)
		{
			unsigned char value = g_buffer[i_b];
			long rampIndex = lroundf((float)value * incr);
			g_frame[i_f] = g_ramp[rampIndex];
			++i_b;
			++i_f;
		}
		++i_f;
	}
	printf(g_frame);
}

size_t BufferIndex_i(size_t x, size_t y)
{
	return y * g_bufferWidth + x;
}
size_t BufferIndex_f(float x, float y)
{
	return BufferIndex_i(lroundf(x), lroundf(y));
}

void DrawPt(unsigned char c, float x, float y)
{
	g_buffer[BufferIndex_f(x,y)] = c;
}

void DrawTexture(Texture tex, long x, long y)
{
	size_t texXStart = 0;
	if (x < 0)
	{
		texXStart = abs(x);
		if (texXStart > tex.width)
			return;
	}
	size_t texYStart = 0;
	if (y < 0)
	{
		texYStart = abs(y);
		if (texYStart > tex.height)
			return;
	}
	size_t lineSize;
	{
		size_t a = tex.width - texXStart;
		size_t b = g_bufferWidth - x;
		lineSize = std::min(a,b);
	}
	size_t lines;
	{
		size_t a = tex.height - texYStart;
		size_t b = g_bufferHeight - y;
		lines = std::min(a,b);
	}

	for (size_t l = 0; l < lines; ++l)
	{
		size_t buffX = x + texXStart;
		size_t buffY = y + l + texYStart;
		size_t texX = texXStart;
		size_t texY = texYStart + l;

		size_t bufferI = g_bufferWidth * buffY + buffX;
		size_t texI = tex.width * texY + texX;
		memcpy(g_buffer + bufferI, tex.data + texI, lineSize);
	}
}

float Lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

// uv should be { 0..1, 0..1 }
unsigned char SampleTexture(const Texture& tex, Vector2 uv)
{
	uv.x = fmodf(uv.x, 1.0f);
	uv.y = fmodf(uv.y, 1.0f);
	uv.x *= (float)tex.width;
	uv.y *= (float)tex.height;
	return tex.data[lroundf(uv.y) * tex.width + lroundf(uv.x)];
}

void DrawTextureEx(Texture tex, Vector2 scale, Vector2 origin, Vector2 position)
{
	long px = lroundf(position.x - origin.x);
	long py = lroundf(position.y - origin.y);

	if (scale.x == 1.0f && scale.y == 1.0f)
		return DrawTexture(tex, px, py);

	Texture scaled;
	scaled.width = lroundf(tex.width * scale.x);
	scaled.height = lroundf(tex.height * scale.y);
	scaled.data = new unsigned char[scaled.width * scaled.height];

	Vector2 uv;
	Vector2 invScaledSize = { 1.0f / ((float)scaled.width - 1.0f), 1.0f / ((float)scaled.height - 1.0f) };
	for (size_t y = 0; y < scaled.height; ++y)
	{
		uv.y = (float)y * invScaledSize.y;
		for (size_t x = 0; x < scaled.width; ++x)
		{
			uv.x = (float)x * invScaledSize.x;
			scaled.data[y * scaled.width + x] = SampleTexture(tex, uv);
		}
	}

	DrawTexture(scaled, px, py);

	delete[] scaled.data;
}

void Draw
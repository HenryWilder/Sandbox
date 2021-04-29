#pragma once

struct Texture
{
	size_t width, height;
	unsigned char* data;
};

struct Vector2
{
	float x, y;
};

void InitBuffer(size_t width, size_t height);
void ClearBackground();
void DrawBuffer();
void DrawPt(unsigned char c, float x, float y);
void DrawTexture(Texture tex, long x, long y);
void DrawTextureEx(Texture tex, Vector2 scale, Vector2 origin, Vector2 position);
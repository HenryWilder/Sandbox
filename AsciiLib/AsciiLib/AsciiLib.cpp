// AsciiLib.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include <stdio.h>
#include <memory>

const char g_ramp_big[70] = { ' ','.','\'','`','^','\"',',',':',';','I','l','!','i','>','<','~','+','_','-','?',']','[','}','{','1',')','(','|','\\','/','t','f','j','r','x','n','u','v','c','z','X','Y','U','J','C','L','Q','0','O','Z','m','w','q','p','d','b','k','h','a','o','*','#','M','W','&','8','%','B','@','$' };
const char g_ramp_lil[10] = { ' ','.',':','-','=','+','*','#','%','@' };

typedef char FrameData_g;

struct FrameData_c
{
	const char pre[2] = { '\x1b','[' };
	char r[3] = { '0','0','0' };
	const char rs = ';';
	char g[3] = { '0','0','0' };
	const char gs = ';';
	char b[3] = { '0','0','0' };
	const char post = 'm';
	char c = ' ';
};

Gray* g_buff_g;
Color* g_buff_c;
char* g_frame; // What actually gets printed
size_t g_bufferWidth;
size_t g_bufferHeight;
size_t g_frameWidth;
size_t g_frameHeight;

void ClearBackground_g(Gray clearValue)
{
	for (size_t i = 0; i < (g_bufferWidth * g_bufferHeight); ++i)
	{
		g_buff_g[i] = clearValue;
	}
}
void ClearBackground_c(Color clearValue)
{
	for (size_t i = 0; i < (g_bufferWidth * g_bufferHeight); ++i)
	{
		g_buff_c[i] = clearValue;
	}
}

void DrawBufferl_g()
{
	
}
void DrawBufferb_g()
{

}
void DrawBuffer_c()
{
	size_t i = 0;
	for (int y = 0; y < g_bufferHeight; ++y)
	{
		for (int x = 0; x < g_bufferWidth; ++x)
		{
			char* pix = g_frame + (y * (g_bufferWidth * sizeof(FrameData_c) + 1) + x);
			pix g_buff_c[i].r, g_buff_c[i].g, g_buff_c[i].b, g_buff_c[i].c);
			++i;
		}
	}
}

void InitBuffer_g(int width, int height)
{
	g_bufferWidth = width;
	g_bufferHeight = height;
	size_t size = g_bufferWidth * g_bufferHeight;
	g_buff_g = new Gray[size];
	g_frameWidth = g_bufferWidth + 1;
	g_frameHeight = g_bufferHeight;
	g_frame_g = new FrameData_g[g_frameWidth * g_frameHeight];
	for (size_t y = 0; y < g_frameHeight - 1; ++y)
	{
		g_frame[g_frameWidth * y + g_frameWidth - 1] = '\n';
	}
	g_frame[g_frameWidth * g_frameHeight - 1] = '\0';
}
void InitBuffer_c(int width, int height)
{
	g_bufferWidth = width;
	g_bufferHeight = height;
	size_t size = (g_bufferWidth * g_bufferHeight) * sizeof(Color);
	g_buff_c = new Color[size];
	g_frameWidth = g_bufferWidth + 1;
	g_frameHeight = g_bufferHeight;
	g_frame_c = new FrameData_c[g_frameWidth * g_frameHeight];
	for (size_t y = 0; y < g_frameHeight - 1; ++y)
	{
		g_frame_c[g_frameWidth * y + g_frameWidth - 1] = '\n';
	}
	g_frame_c[g_frameWidth * g_frameHeight - 1] = '\0';
}

void FlushBuffer_g()
{
	g_bufferWidth = 0ull;
	g_bufferHeight = 0ull;
	delete[] g_buff_g;
	delete[] g_frame_g;
}
void FlushBuffer_c()
{
	g_bufferWidth = 0ull;
	g_bufferHeight = 0ull;
	delete[] g_buff_c;
	delete[] g_frame_c;
}

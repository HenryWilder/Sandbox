#pragma once
#include <Windows.h>
#include <stdio.h>
#include "resource.h"

namespace screen
{
    constexpr int width = 1920;
    constexpr int height = 1080;
    constexpr int area = width * height;
}

// Get a console handle
extern HWND window;
// Get a handle to device hdc
extern HDC hdc;

class FrameBuffer
{
private:
    COLORREF m_frame[screen::area];

public:
    void Draw();
    void DrawSprite();
};

extern FrameBuffer g_frameBuffer;
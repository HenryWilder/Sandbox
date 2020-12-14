#pragma once
#include <Windows.h>
#include "Data.h"

using namespace std;

// Get a console handle
extern HWND window;

// Get a handle to device context
extern HDC context;

// A 24-bit color in RGB space
struct Color
{
    unsigned char r, g, b;
};

struct Sprite
{
    enum class Size
    {
        s8 = 8,
        s16 = 16,
        s32 = 32,
    };

    Sprite(Size size)
    {
        Whole s = Whole(size);
        m_size = s * s;
        m_width = s;
        m_colors[0] = { 0,0,0 };
        m_colors[1] = { 255,0,0 };
        m_colors[2] = { 255,255,0 };
        m_colors[3] = { 0,255,0 };
        m_colors[4] = { 0,255,255 };
        m_colors[5] = { 0,0,255 };
        m_colors[6] = { 255,0,255 };
        m_colors[7] = { 255,255,255 };
        m_texture = new BYTE[m_size];
    }
    ~Sprite()
    {
        delete[] m_texture;
    }

    Whole m_size;

    Whole m_width;

    Color m_colors[8];

    BYTE* m_texture;
};

struct Sprite8x8
{

};

struct FrameBuffer
{
private:
    COLORREF m_frame[screen::area];

public:

    // Converts a coordinate to an index in the frame buffer
    // DOES NOT VALIDATE INPUT (may return values outside the buffer!)
    int Index(PixelPos pos);

    // Returns a bool for whether the input coordinate can be safely referenced in the frame buffer array without unexpected results
    bool Valid(PixelPos pos);

    /// <summary>
    /// Sets the color of a pixel at the given coordinates.
    /// </summary>
    /// <param name="pos">The position of the pixel being set.</param>
    /// <param name="c">The color the pixel will be set to.</param>
    void DrawToBuffer(PixelPos pos, Color c);

    void GrayToBuffer(PixelPos pos, unsigned char s);

    void GrayToBuffer(PixelPos pos, double s);

    void DrawSprite(PixelPos center, Sprite& sprite, WORD depth);

    /// <summary>
    /// Draw the entire frame buffer to the console.
    /// </summary>
    void Draw();
};

extern FrameBuffer g_frameBuffer;
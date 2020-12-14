#pragma once
#include <Windows.h>
#include "Screenspace.h"

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

typedef double (*GrayscaleOutput)(AbstrPos pos, void*);
typedef Color(*ColorOutput)(AbstrPos pos, void*);

/*
struct Sprite
{
    Sprite(int size)
    {
        m_size = size
        m_texture = new COLORREF[m_size];
    }
    ~Sprite()
    {
        delete [] m_texture;
    }

    int m_size

    COLORREF* m_texture;
};
*/

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

    /// <summary>
    /// Draw the entire frame buffer to the console.
    /// </summary>
    void Draw();

    void ApplyGrayscaleShader(GrayscaleOutput material, void* materialData);

    void ApplyColorShader(ColorOutput material, void* materialData);
};

extern FrameBuffer g_frameBuffer;
#include "Rendering.h"

// Get a console handle
HWND window = GetConsoleWindow();

// Get a handle to device context
HDC context = GetDC(window);

int FrameBuffer::Index(PixelPos pos)
{
    return (screen::width * pos.y) + pos.x;
}

bool FrameBuffer::Valid(PixelPos pos)
{
    return (
        (pos.x <= screen::width) && // The input x is not beyond the boundaries of the screen, which would wrap around to the next row
        (Index(pos) < screen::area)); // The input position does not produce an index outside the buffer, which would throw an exception

                                        // The input y does not need to be validated if both these are true.
                                        // The PixelPos type is made up of UINTs, and so "x >= 0" does not need to be tested for.
}

void FrameBuffer::DrawToBuffer(PixelPos pos, Color c)
{
    if (Valid(pos))
        m_frame[Index(pos)] = RGB(c.r, c.g, c.b);
}
void FrameBuffer::GrayToBuffer(PixelPos pos, unsigned char s)
{
    DrawToBuffer(pos, { s,s,s });
}

void FrameBuffer::GrayToBuffer(PixelPos pos, double s)
{
    GrayToBuffer(pos, (unsigned char)s);
}

/// <summary>
/// Draw the entire frame buffer to the console.
/// </summary>

void FrameBuffer::Draw()
{
    for (Whole i = 0; i < screen::area; ++i)
    {
        SetPixel(context, i % screen::width, i / screen::width, m_frame[i]); // Draws the pixel from the frame buffer to the console
    }
}

void FrameBuffer::ApplyGrayscaleShader(GrayscaleOutput material, void* materialData)
{
    for (Whole i = 0; i < screen::area; ++i)
    {
        PixelPos pixel = {
            i % screen::width,
            i / screen::width };

        GrayToBuffer(pixel, material(pixel, materialData));
    }
}

void FrameBuffer::ApplyColorShader(ColorOutput material, void* materialData)
{
    for (Whole i = 0; i < screen::area; ++i)
    {
        PixelPos pixel = {
            i % screen::width,
            i / screen::width };

        DrawToBuffer(pixel, material(pixel, materialData));
    }
}

FrameBuffer g_frameBuffer;
#include "Rendering.h"
#include <vector>

// Get a console handle
HWND window = GetConsoleWindow();

// Get a handle to device hdc
HDC hdc = GetDC(window);

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
    // Creating temp bitmap
    HBITMAP noiseMap;
    noiseMap = CreateBitmap(screen::width, screen::height, 1, 8 * sizeof(COLORREF), (void*)m_frame); // pointer to array

    // Temp HDC to copy picture
    HDC src = CreateCompatibleDC(hdc); // hdc - Device hdc for window, I've got earlier with GetDC(hWnd) or GetDC(NULL);

    SelectObject(src, noiseMap); // Inserting picture into our temp HDC

    // Copy image from temp HDC to window
    BitBlt(hdc, 0, 0, screen::width, screen::height, src, 0, 0, SRCCOPY);

    DeleteDC(src); // Deleting temp HDC
}

/*
    // Creating temp bitmap
    HBITMAP noiseMap;
    noiseMap = CreateBitmap(space::screen::boardWidth, space::screen::boardWidth, 1, 8 * sizeof(COLORREF), (void*)m_frame); // pointer to array

    // Temp HDC to copy picture
    HDC src = CreateCompatibleDC(hdc); // hdc - Device hdc for window, I've got earlier with GetDC(hWnd) or GetDC(NULL);

    SelectObject(src, noiseMap); // Inserting picture into our temp HDC

    // Copy image from temp HDC to window
    BitBlt(hdc, 10, 10, space::screen::boardWidth, space::screen::boardWidth, src, 0, 0, SRCCOPY);

    DeleteDC(src); // Deleting temp HDC
    */

void FrameBuffer::ApplyGrayscaleShader(GrayscaleOutput material, void* materialData)
{
    Whole i = 0;
    for (int x = 0; x < screen::width; ++x)
    {
        for (int y = 0; y < screen::width; ++y)
        {
            PixelPos pixel;
            pixel.x = x;
            pixel.y = y;

            GrayToBuffer(pixel, material(pixel, materialData));
        }
    }
}

void FrameBuffer::ApplyColorShader(ColorOutput material, void* materialData)
{
    Whole i = 0;
    for (int x = 0; x < screen::width; ++x)
    {
        for (int y = 0; y < screen::width; ++y)
        {
            PixelPos pixel;
            pixel.x = x;
            pixel.y = y;

            DrawToBuffer(pixel, material(pixel, materialData));
        }
    }
}

FrameBuffer g_frameBuffer;
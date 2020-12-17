#include "Rendering.h"

// Get a console handle
HWND window = GetConsoleWindow();
// Get a handle to device hdc
HDC hdc = GetDC(window);

FrameBuffer g_frameBuffer;

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

void FrameBuffer::DrawSprite()
{
    HDC src = CreateCompatibleDC(hdc); // hdc - Device hdc for window, I've got earlier with GetDC(hWnd) or GetDC(NULL);

    SelectObject(src, Mario.bmp); // Inserting picture into our temp HDC

    // Copy image from temp HDC to window
    BitBlt(hdc, 0, 0, screen::width, screen::height, src, 0, 0, SRCCOPY);

    DeleteDC(src); // Deleting temp HDC
}

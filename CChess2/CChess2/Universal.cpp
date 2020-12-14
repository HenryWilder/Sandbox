#include "Universal.h"

// Get a console handle
HWND window = GetConsoleWindow();

// Get a handle to device context
HDC hdc = GetDC(window);

void SleepForMS(int ms) {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(ms));
}
void SleepForMS(double ms) {
    std::this_thread::sleep_for(
        std::chrono::microseconds((int)(ms * 1000.0)));
}

int Snap(int value, int gridsize) {
    int snappedVal = value;
    snappedVal /= gridsize;
    snappedVal *= gridsize;
    return snappedVal;
}

bool Even(int a) {
    return a == Snap(a, 2);
}

PixelPos Snap(PixelPos pix, PixelPos gridsize)
{
    PixelPos snappedPix = pix;
    pix /= gridsize;
    pix *= gridsize;
    return pix;
}

PixelPos Snap(PixelPos pix, int gridsize)
{
    PixelPos snappedPix = pix;
    pix = pix / gridsize;
    pix = pix * gridsize;
    return pix;
}

PixelPos PixelSpace(const Coord tile)
{
    PixelPos out = tile;
    out = out * space::screen::tileWidth;
    return out;
}

Coord BoardSpace(const PixelPos pixel)
{
    Coord out = pixel;
    out = pixel / space::screen::tileWidth;
    return out;
}

PixelPos::operator Coord() const
{
    Coord out;
    out.x = x;
    out.y = y;
    return out;
}

Coord::operator PixelPos() const
{
    PixelPos out;
    out.x = x;
    out.y = y;
    return out;
}

BoardTile::BoardTile()
{
    start = {};
    end = {};
}

BoardTile::BoardTile(Coord tile)
{
    // Top-left pixel of the space
    start = PixelSpace(tile);
    // Bottom-right pixel of the space
    end = start + PixelPos{ space::screen::tileWidth, space::screen::tileWidth };
}

/*
BoardTile2::BoardTile2()
{
    for (int i = 0; i < space::screen::tileArea; ++i)
        pixels[i] = { 0,0 };
}

BoardTile2::BoardTile2(Coord tile)
{
    PixelPos start = PixelSpace(tile);

    PixelPos thisPix = start;

    unsigned int i = 0;

    for (thisPix.x = 0; thisPix.x < space::screen::tileWidth; ++thisPix.x)
        for (thisPix.y = 0; thisPix.y < space::screen::tileWidth; ++thisPix.y)
            pixels[i] = thisPix;
}
*/

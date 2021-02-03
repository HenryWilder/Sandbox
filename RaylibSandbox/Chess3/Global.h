#pragma once
namespace spce
{
    namespace game // Game
    {
        constexpr int g_sideTileCount = 8; // The number of spaces in a row
        constexpr int g_boardArea = g_sideTileCount * g_sideTileCount; // The total spaces on the board
    }

    namespace scrn // Screen
    {
        constexpr int g_tileWidth = 16; // The number of pixels on the side of a space
        constexpr int g_tileArea = g_tileWidth * g_tileWidth; // The total pixels in a space
        constexpr int g_boardWidth = g_tileWidth * game::g_sideTileCount; // The number of pixels on the side of the board
        constexpr int g_totalArea = g_boardWidth * g_boardWidth; // The total pixels in the board
        constexpr int g_gameScale = 2;

        namespace outp // Output
        {
            constexpr int g_otileWidth = g_tileWidth * g_gameScale; // The number of pixels on the side of a space, scaled by render output
            constexpr int g_oboardWidth = g_boardWidth * g_gameScale; // The number of pixels on the side of the entire board, scaled by render output
            constexpr int g_oboardLIndex = g_boardWidth - g_tileWidth; // The number of pixels on the side of the entire board, scaled by render output, minus one tile to act as an index
        }
    }
}

struct Texture;
typedef Texture Texture2D;

namespace sprite
{
    extern Texture2D* missing;

    extern Texture2D* pawn;
    extern Texture2D* rook;
    extern Texture2D* knight;
    extern Texture2D* bishop;
    extern Texture2D* queen;
    extern Texture2D* king;
}
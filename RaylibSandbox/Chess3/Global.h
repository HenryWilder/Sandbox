#pragma once
#pragma region Globals
//
// Spacial globals
//
namespace spce
{
    namespace g // Game
    {
        constexpr int g_sideTileCount = 8; // The number of spaces in a row
        constexpr int g_boardArea = g_sideTileCount * g_sideTileCount; // The total spaces on the board
    }

    namespace s // Screen
    {
        constexpr int g_tileWidth = 16; // The number of pixels on the side of a space
        constexpr int g_tileArea = g_tileWidth * g_tileWidth; // The total pixels in a space
        constexpr int g_boardWidth = g_tileWidth * g::g_sideTileCount; // The number of pixels on the side of the board
        constexpr int g_totalArea = g_boardWidth * g_boardWidth; // The total pixels in the board

        constexpr int g_gameScale = 4; // Number of sprite pixels to 

        namespace o // Output
        {
            constexpr int g_oTileWidth = g_tileWidth * g_gameScale; // The number of pixels on the side of a space, scaled by render out
            constexpr int g_oBoardWidth = g_boardWidth * g_gameScale; // The number of pixels on the side of the entire board, scaled by render out
        }
    }
}

//
// Sprite texture globals
//

struct Texture;
typedef Texture Texture2D;
namespace sprite
{
    extern Texture2D missing;

    extern Texture2D pawn;
    extern Texture2D rook;
    extern Texture2D knight;
    extern Texture2D bishop;
    extern Texture2D queen;
    extern Texture2D king;
}
#pragma endregion

#pragma region Conversions
//
// Conversions
//
struct Vector2;
struct Rectangle;

namespace conv
{
    int To(int i, int dest);
    float To(float f, int dest);
    Vector2 To(Vector2 vec, int dest);
    Rectangle To(Rectangle rec, int dest);

    int From(int i, int src);
    float From(float f, int src);
    Vector2 From(Vector2 vec, int src);
    Rectangle From(Rectangle rec, int src);

    Vector2 QuarterDim(Rectangle rec); // Dimensions of quarter
    Rectangle Quarter(Rectangle rec);
    Rectangle Offset(Rectangle rec, Vector2 amount);

    Rectangle Subdivide(Rectangle rec, bool xIsLarge, bool yIsLarge);
    Rectangle Subdivide(Rectangle rec, char quad); // 0bxy
}

template<typename T> T BoardToScreen(T val) { return conv::To(val, spce::s::o::g_oTileWidth); }
template<typename T> T ScreenToBoard(T val) { return conv::From(val, spce::s::o::g_oTileWidth); }
#pragma endregion
#include "Rendering.h"
#include "Board.h"

using namespace space;

COLORREF CRef(Color c)
{
    return RGB(c.r, c.g, c.b);
}

unsigned int FrameBuffer::Index(PixelPos pos)
{
    unsigned int index = (screen::boardWidth) * pos.y + pos.x;
    return index;
}

bool FrameBuffer::Valid(PixelPos pos)
{
    bool valid;
    valid = (
        (pos.x < screen::boardWidth) && // The input x is not beyond the boundaries of the screen, which would wrap around to the next row
        (pos.y < screen::boardWidth)); // The input position does not produce an index outside the buffer, which would throw an exception

    // The input y does not need to be validated if both these are true.
    // The PixelPos type is made up of UINTs, and so "x >= 0" does not need to be tested for.
    return valid;
}

COLORREF FrameBuffer::Get(PixelPos pixel)
{
    return m_frame[Index(pixel)];
}

COLORREF FrameBuffer::SpacePatternAtPos(Coord space)
{
    // Pick the color
    if (Even(space.x + space.y))
        return sprite::PaletteColor(sprite::Pltt::Board_Black);
    else
        return sprite::PaletteColor(sprite::Pltt::Board_White);
}

void FrameBuffer::DrawPixel(PixelPos pixel, COLORREF color)
{
    PixelPos pix = pixel * space::screen::gameScale;

    for (int x = pix.x; x < pix.x + space::screen::gameScale; ++x)
        for (int y = pix.y; y < pix.y + space::screen::gameScale; ++y)
            SetPixelV(hdc, x, y, color);
}
void FrameBuffer::DrawPixel(PixelPos pixel, Color color)
{
    DrawPixel(pixel, CRef(color));
}

bool FrameBuffer::DrawPixelSafe(PixelPos pixel, COLORREF color)
{
    if (Valid(pixel)) {
        DrawPixel(pixel, color);
        return true;
    }
    else
        return false;
}
bool FrameBuffer::DrawPixelSafe(PixelPos pixel, Color color)
{
    return DrawPixelSafe(pixel, CRef(color));
}

void FrameBuffer::DrawToBufferAndScreen(PixelPos pos, COLORREF c)
{
    m_frame[Index(pos)] = c;
    DrawPixel(pos, c);
}

void FrameBuffer::DrawToBufferAndScreen(PixelPos pos, Color c)
{
    DrawToBufferAndScreen(pos, CRef(c));
}

void FrameBuffer::WriteToBuffer(PixelPos pos, COLORREF c)
{
    m_frame[Index(pos)] = c;
}

void FrameBuffer::WriteToBuffer(PixelPos pos, Color c)
{
    WriteToBuffer(pos, CRef(c));
}

bool FrameBuffer::DrawToBufferSafe(PixelPos pos, COLORREF c)
{
    if (Valid(pos)) {
        m_frame[Index(pos)] = c;
        return true;
    }
    else
        return false;
}
bool FrameBuffer::DrawToBufferSafe(PixelPos pos, Color c)
{
    return DrawToBufferSafe(pos, CRef(c));
}

void FrameBuffer::DrawSpriteFAST(Coord space, sprite::Sprite* sprite, bool team, bool effect)
{
    BoardTile spaceBounds = BoardTile(space);

    PixelPos pix;

    unsigned int i = 0; // The index of the sprite's array

    for (pix.y = spaceBounds.start.y; pix.y < spaceBounds.end.y; ++pix.y)
    {
        for (pix.x = spaceBounds.start.x; pix.x < spaceBounds.end.x; ++pix.x)
        {
#if UCHARARRAY_SPRITE_METHOD
            if (sprite->m_texture[i] != 0) // Check is opaque
#else
            if (sprite->m_texture[i] != ' ') // Check is opaque
#endif
                DrawToBufferAndScreen(pix, sprite->SpriteColor(i, team)); // Draws the pixel from the frame buffer to the console

            ++i;
        }
    }
}

void FrameBuffer::DrawSpriteFASTWithBG(Coord space, sprite::Sprite* sprite, bool team, COLORREF bgColor, bool effect)
{
    BoardTile spaceBounds = BoardTile(space);

    PixelPos pix;

    unsigned int i = 0; // The index of the sprite's array

    for (pix.y = spaceBounds.start.y; pix.y < spaceBounds.end.y; ++pix.y)
    {
        for (pix.x = spaceBounds.start.x; pix.x < spaceBounds.end.x; ++pix.x)
        {
#if UCHARARRAY_SPRITE_METHOD
            if (sprite->m_texture[i] != 0) // Check is opaque
#else
            if (sprite->m_texture[i] != ' ') // Check is opaque
#endif
                DrawToBufferAndScreen(pix, sprite->SpriteColor(i, team)); // Draws the pixel from the frame buffer to the console
            else
                DrawToBufferAndScreen(pix, bgColor);

            ++i;
        }
    }
}

void FrameBuffer::BufferCleanplateSpace(Coord space)
{
    COLORREF color = SpacePatternAtPos(space);

    BoardTile spaceBounds = BoardTile(space);

    PixelPos pix;

    for (pix.y = spaceBounds.start.y; pix.y < spaceBounds.end.y; ++pix.y)
    {
        for (pix.x = spaceBounds.start.x; pix.x < spaceBounds.end.x; ++pix.x)
        {
            WriteToBuffer(pix, color); // Writes to buffer, does not draw to render
        }
    }
    //DrawGridSpaceFAST(space, 0); Sanity check
}

void FrameBuffer::DrawGhost(
    PixelPos spritePosTL, // The top-left corner of the sprite
    PixelPos cleanPosTL, // The top-left corner of the cleanplate
    sprite::Sprite* sprite, bool team)
{
    // Start and end WILL NOT be the same size as a tile. There will always be some amount of movement.
    PixelPos start;
    PixelPos end;

    // X-values
    if (spritePosTL.x > cleanPosTL.x) {
        start.x = cleanPosTL.x;
        end.x = spritePosTL.x + screen::tileWidth;
    }
    else {
        start.x = spritePosTL.x;
        end.x = cleanPosTL.x + screen::tileWidth;
    }
    // Y-values
    if (spritePosTL.y > cleanPosTL.y) {
        start.y = cleanPosTL.y;
        end.y = spritePosTL.y + screen::tileWidth;
    }
    else {
        start.y = spritePosTL.y;
        end.y = cleanPosTL.y + screen::tileWidth;
    }


    PixelPos pix; // The pixel we are currently drawing

    unsigned int i = 0; // The index of the sprite's array

    for (pix.y = start.y; pix.y < end.y; ++pix.y) {
        for (pix.x = start.x; pix.x < end.x; ++pix.x)
        {
            // We want to pause incrementing the sprite index when we are outside the bounds of the sprite's image.
            bool inSpriteBounds =
                pix.x >= spritePosTL.x &&
                pix.y >= spritePosTL.y &&
                pix.x < (spritePosTL.x + screen::tileWidth) &&
                pix.y < (spritePosTL.y + screen::tileWidth);

            if (inSpriteBounds)
            {
#if UCHARARRAY_SPRITE_METHOD
                if (sprite->m_texture[i] != 0) // Check is opaque
#else
                if (sprite->m_texture[i] != ' ') // Check is opaque
#endif
                    DrawPixel(pix, sprite->SpriteColor(i, team));
                else
                    DrawPixel(pix, Get(pix));

                ++i;
            }
            else
            {
                DrawPixel(pix, Get(pix));
            }
        }
    }
}

void FrameBuffer::DrawSprite(Coord space, sprite::Sprite* sprite, bool team, bool effect)
{
    BoardTile spaceBounds = BoardTile(space);

    PixelPos pix;

    unsigned int i = 0; // The index of the sprite's array

    for (pix.y = spaceBounds.start.y; pix.y < spaceBounds.end.y; ++pix.y)
    {
        for (pix.x = spaceBounds.start.x; pix.x < spaceBounds.end.x; ++pix.x)
        {
#if UCHARARRAY_SPRITE_METHOD
            if (sprite->m_texture[i] != 0) // Check is opaque
#else
            if (sprite->m_texture[i] != ' ') // Check is opaque
#endif
            {
                WriteToBuffer(pix, sprite->SpriteColor(i, team)); // Draws the pixel from the frame buffer to the console
            }

            ++i;
        }
    }
}

void FrameBuffer::DrawRectFAST(int startx, int starty, int endx, int endy, COLORREF color)
{
    PixelPos pos;

    for (int x = startx; x < endx; ++x)
    {
        pos.x = x;

        for (int y = starty; y < endy; ++y)
        {
            pos.y = y;

            DrawToBufferAndScreen(pos, color); // Draws the pixel from the frame buffer to the console
        }
    }
}

void FrameBuffer::DrawRect(int startx, int starty, int endx, int endy, COLORREF color)
{
    PixelPos pos;

    for (int x = startx; x < endx; ++x)
    {
        pos.x = x;

        for (int y = starty; y < endy; ++y)
        {
            pos.y = y;

            WriteToBuffer(pos, color); // Draws the pixel from the frame buffer to the console
        }
    }
}
void FrameBuffer::DrawRect(int startx, int starty, int endx, int endy, Color color)
{
    DrawRect(startx, starty, endx, endy, CRef(color));
}

void FrameBuffer::DrawGridSpaceFAST(Coord space, COLORREF color)
{
    BoardTile spaceBounds = BoardTile(space);
    DrawRectFAST(spaceBounds.start.x, spaceBounds.start.y, spaceBounds.end.x, spaceBounds.end.y, color);
}

void FrameBuffer::DrawGridSpace(Coord space, COLORREF color)
{
    BoardTile spaceBounds = BoardTile(space);
    DrawRect(spaceBounds.start.x, spaceBounds.start.y, spaceBounds.end.x, spaceBounds.end.y, color);
}

void FrameBuffer::DrawBoardPattern1SpaceFAST(Coord space)
{
    DrawGridSpaceFAST(space, SpacePatternAtPos(space));
}

void FrameBuffer::DrawBoardPattern1Space(Coord space)
{
    DrawGridSpace(space, SpacePatternAtPos(space));
}

void FrameBuffer::DrawBoardPattern()
{
    PixelPos space;

    for (space.y = 0; space.y < game::sideTileCount; ++space.y) // 8 rows
    {
        for (space.x = 0; space.x < game::sideTileCount; ++space.x) // 8 columns
        {
            DrawBoardPattern1Space(space);
        }
    }
}

void FrameBuffer::Draw()
{
    using namespace screen;

    unsigned int i = 0;

    PixelPos pixel = { 0U,0U };

    for (pixel.y = 0U; pixel.y < boardWidth; ++pixel.y)
    {
        for (pixel.x = 0U; pixel.x < boardWidth; ++pixel.x)
        {
            DrawPixel(pixel, m_frame[i]);

            ++i;
        }
    }
}

FrameBuffer g_frameBuffer;

namespace sprite
{
    const Color colorPalette[] = {

        // Board
        {236, 167, 95}, // Black
        {252, 219, 166},  // White

        // Black
        {80, 80, 90}, // Fill
        {50, 55, 60}, // Shade
        {40, 30, 50}, // Outline
        {100, 100, 110}, // Shine

        // White
        {240, 240, 230}, // Fill
        {180, 150, 120}, // Shade
        {100, 100, 90}, // Outline
        {255, 255, 255}, // Shine

        // Select
        {44, 200, 37}, // Unit we are moving
        {155, 235, 135}, // Available space we can move to
        {220, 20, 20}, // Available space with a piece we can take
    };

    COLORREF PaletteColor(Pltt col)
    {
        return CRef(colorPalette[(unsigned int)col]);
    }

    COLORREF Sprite::SpriteColor(int index, bool team)
    {
        unsigned char value = m_texture[index];

        Pltt out = Pltt::Board_Black;

        if (!team)
        {
            switch (value)
            {
#if UCHARARRAY_SPRITE_METHOD
            case 1:
                out = Pltt::UnitB_Fill;
                break;
            case 2:
                out = Pltt::UnitB_Shade;
                break;
            case 3:
                out = Pltt::UnitB_Edge;
                break;
            case 4:
                out = Pltt::UnitB_Shine;
                break;
            case 5: // Glow
                out = Pltt::Select_Unit;
                break;
#else
            case '1':
                out = Pltt::UnitB_Fill;
                break;
            case '2':
                out = Pltt::UnitB_Shade;
                break;
            case '3':
                out = Pltt::UnitB_Edge;
                break;
            case '4':
                out = Pltt::UnitB_Shine;
                break;
            case '5': // Glow
                out = Pltt::Select_Unit;
                break;
#endif
            }
        }
        else
        {
            switch (value)
            {
#if UCHARARRAY_SPRITE_METHOD
            case 1:
                out = Pltt::UnitW_Fill;
                break;
            case 2:
                out = Pltt::UnitW_Shade;
                break;
            case 3:
                out = Pltt::UnitW_Edge;
                break;
            case 4:
                out = Pltt::UnitW_Shine;
                break;
            case 5: // Glow
                out = Pltt::Select_Unit;
                break;
#else
            case '1':
                out = Pltt::UnitW_Fill;
                break;
            case '2':
                out = Pltt::UnitW_Shade;
                break;
            case '3':
                out = Pltt::UnitW_Edge;
                break;
            case '4':
                out = Pltt::UnitW_Shine;
                break;
            case '5': // Glow
                out = Pltt::Select_Unit;
                break;
#endif
            }
        }

        return PaletteColor(out);
    }

    namespace unit
    {
#if UCHARARRAY_SPRITE_METHOD
        Sprite null = { {} };
#else
        Sprite null = {
           "                "
           "                "
           "                "
           "                "
           "                "
           "                "
           "                "
           "                "
           "                "
           "                "
           "                "
           "                "
           "                "
           "                "
           "                "
           "                "
    };
#endif
#if UCHARARRAY_SPRITE_METHOD
        Sprite pawn = { {
           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
           0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,
           0,0,0,0,0,2,1,1,4,4,2,0,0,0,0,0,
           0,0,0,0,0,3,1,1,4,4,2,0,0,0,0,0,
           0,0,0,0,0,3,1,1,1,1,2,0,0,0,0,0,
           0,0,0,0,0,3,2,1,1,1,3,0,0,0,0,0,
           0,0,0,0,0,0,3,2,4,2,0,0,0,0,0,0,
           0,0,0,0,0,3,2,1,1,4,3,0,0,0,0,0,
           0,0,0,0,0,0,3,2,4,3,0,0,0,0,0,0,
           0,0,0,0,0,0,3,1,4,3,0,0,0,0,0,0,
           0,0,0,0,0,0,3,1,4,3,0,0,0,0,0,0,
           0,0,0,0,0,3,2,1,4,1,3,0,0,0,0,0,
           0,0,0,0,3,2,1,1,1,4,4,3,0,0,0,0,
           0,0,0,0,3,3,3,3,3,3,3,3,0,0,0,0,
        } };
#else
        Sprite pawn = {
           "                "
           "                "
           "                "
           "      2222      "
           "     211442     "
           "     311442     "
           "     311112     "
           "     321113     "
           "      3242      "
           "     321143     "
           "      3243      "
           "      3143      "
           "      3143      "
           "     321413     "
           "    32111443    "
           "    33333333    "
        };
#endif
#if UCHARARRAY_SPRITE_METHOD
        Sprite rook = { {
           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
           0,0,0,0,3,3,0,3,3,0,3,3,0,0,0,0,
           0,0,0,0,3,2,3,2,4,3,4,3,0,0,0,0,
           0,0,0,0,3,2,1,1,1,1,4,3,0,0,0,0,
           0,0,0,0,3,2,1,1,1,1,1,3,0,0,0,0,
           0,0,0,0,0,3,2,2,1,2,3,0,0,0,0,0,
           0,0,0,0,0,3,2,1,1,4,3,0,0,0,0,0,
           0,0,0,0,0,3,2,1,1,4,3,0,0,0,0,0,
           0,0,0,0,0,3,2,1,1,4,3,0,0,0,0,0,
           0,0,0,0,0,3,2,1,1,4,3,0,0,0,0,0,
           0,0,0,0,0,3,2,1,1,4,3,0,0,0,0,0,
           0,0,0,0,3,2,2,1,1,4,4,3,0,0,0,0,
           0,0,0,3,2,2,1,1,1,1,4,4,3,0,0,0,
           0,0,0,3,3,3,3,3,3,3,3,3,3,0,0,0,
        } };
#else
        Sprite rook = {
           "                "
           "                "
           "   3323333233   "
           "   3223113243   "
           "   3233113343   "
           "   3211111143   "
           "    32111113    "
           "     322123     "
           "     321143     "
           "     321143     "
           "     321143     "
           "    32211443    "
           "    32114443    "
           "   3222221143   "
           "   3221111443   "
           "   3333333333   "
        };
#endif
#if UCHARARRAY_SPRITE_METHOD
        Sprite knight = { {
           0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,
           0,0,0,0,0,3,2,1,2,0,0,0,0,0,0,0,
           0,0,0,0,3,2,1,4,2,3,3,3,3,3,0,0,
           0,0,0,3,2,1,1,1,4,4,2,2,4,4,3,0,
           0,0,0,3,2,1,1,1,1,1,1,1,1,2,3,0,
           0,0,0,3,2,1,1,2,1,1,1,1,1,1,3,0,
           0,0,0,0,3,2,1,2,1,1,1,1,3,3,0,0,
           0,0,0,0,3,2,1,1,2,2,2,3,0,0,0,0,
           0,0,0,0,0,3,2,1,4,4,2,0,0,0,0,0,
           0,0,0,0,0,0,3,2,1,4,4,2,0,0,0,0,
           0,0,0,0,0,0,3,2,1,1,4,4,3,0,0,0,
           0,0,0,0,0,0,3,2,1,1,4,1,3,0,0,0,
           0,0,0,0,0,3,2,1,1,1,1,2,3,0,0,0,
           0,0,0,0,3,2,2,1,1,1,2,3,0,0,0,0,
           0,0,0,3,2,2,1,1,1,1,4,4,2,0,0,0,
           0,0,0,3,3,3,3,3,3,3,3,3,3,0,0,0,
        } };
#else
        Sprite knight = {
           "      33        "
           "     3212       "
           "    3214233333  "
           "   321114422443 "
           "   321111111123 "
           "   321121111113 "
           "    3212111133  "
           "    32112223    "
           "     321442     "
           "      321442    "
           "      3211443   "
           "      3211413   "
           "     32111123   "
           "    32211123    "
           "   3221111442   "
           "   3333333333   "
        };
#endif
#if UCHARARRAY_SPRITE_METHOD
        Sprite bishop = { {
           0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,
           0,0,0,0,0,0,3,1,4,3,0,0,0,0,0,0,
           0,0,0,0,0,3,2,2,1,3,2,0,0,0,0,0,
           0,0,0,0,3,2,1,1,1,3,2,0,0,0,0,0,
           0,0,0,0,3,2,1,1,3,1,4,3,0,0,0,0,
           0,0,0,0,3,2,1,1,1,1,4,3,0,0,0,0,
           0,0,0,0,0,3,2,1,1,1,3,0,0,0,0,0,
           0,0,0,0,0,0,3,2,2,3,0,0,0,0,0,0,
           0,0,0,0,0,3,2,1,1,4,3,0,0,0,0,0,
           0,0,0,0,0,0,3,2,4,3,0,0,0,0,0,0,
           0,0,0,0,0,0,3,1,4,3,0,0,0,0,0,0,
           0,0,0,0,0,3,2,1,1,4,3,0,0,0,0,0,
           0,0,0,0,0,3,2,1,1,4,3,0,0,0,0,0,
           0,0,0,0,3,2,2,1,1,4,4,3,0,0,0,0,
           0,0,0,3,2,2,1,1,1,1,4,4,3,0,0,0,
           0,0,0,3,3,3,3,3,3,3,3,3,3,0,0,0,
        } };
#else
        Sprite bishop = {
           "       33       "
           "      3143      "
           "     322132     "
           "    3211132     "
           "    32113143    "
           "    32111143    "
           "     321113     "
           "      3223      "
           "     321143     "
           "      3243      "
           "      3143      "
           "     321143     "
           "     321143     "
           "    32211443    "
           "   3221111443   "
           "   3333333333   "
        };
#endif
#if UCHARARRAY_SPRITE_METHOD
        Sprite queen = { {
           0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,
           0,0,0,3,3,2,3,1,4,3,2,3,3,0,0,0,
           0,0,0,3,1,3,3,3,2,3,3,1,3,0,0,0,
           0,0,0,3,2,1,1,1,1,1,2,1,3,0,0,0,
           0,0,0,0,3,2,2,2,1,2,1,3,0,0,0,0,
           0,0,0,0,0,3,2,1,1,1,3,0,0,0,0,0,
           0,0,0,0,0,3,2,1,1,2,3,0,0,0,0,0,
           0,0,0,0,0,0,3,2,2,3,0,0,0,0,0,0,
           0,0,0,0,0,3,2,1,1,4,3,0,0,0,0,0,
           0,0,0,0,0,0,3,2,4,3,0,0,0,0,0,0,
           0,0,0,0,0,0,3,1,4,3,0,0,0,0,0,0,
           0,0,0,0,0,3,2,1,1,4,3,0,0,0,0,0,
           0,0,0,0,0,3,2,1,1,4,3,0,0,0,0,0,
           0,0,0,0,3,2,2,2,1,4,4,3,0,0,0,0,
           0,0,0,3,2,2,1,1,1,1,4,4,3,0,0,0,
           0,0,0,3,3,3,3,3,3,3,3,3,3,0,0,0,
        } };
#else
        Sprite queen = {
           "       33       "
           "   3323143233   "
           "   3133323313   "
           "   3211111213   "
           "    32221213    "
           "     321113     "
           "     321123     "
           "      3223      "
           "     321143     "
           "      3243      "
           "      3143      "
           "     321143     "
           "     321143     "
           "    32221443    "
           "   3221111443   "
           "   3333333333   "
        };
#endif
#if UCHARARRAY_SPRITE_METHOD
        Sprite king = { {
           0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,
           0,0,0,0,0,0,2,3,3,2,0,0,0,0,0,0,
           0,0,0,0,2,3,3,1,1,3,3,2,0,0,0,0,
           0,0,0,0,3,2,1,1,4,4,4,3,0,0,0,0,
           0,0,0,0,3,2,1,1,1,1,2,3,0,0,0,0,
           0,0,0,0,0,3,2,1,1,1,3,0,0,0,0,0,
           0,0,0,0,0,3,2,1,1,2,3,0,0,0,0,0,
           0,0,0,0,0,0,3,2,2,3,0,0,0,0,0,0,
           0,0,0,0,0,3,2,1,1,4,3,0,0,0,0,0,
           0,0,0,0,0,0,3,2,4,3,0,0,0,0,0,0,
           0,0,0,0,0,0,3,1,4,3,0,0,0,0,0,0,
           0,0,0,0,0,3,2,1,1,4,3,0,0,0,0,0,
           0,0,0,0,0,3,2,1,1,4,3,0,0,0,0,0,
           0,0,0,0,3,2,2,2,1,4,4,3,0,0,0,0,
           0,0,0,3,2,2,1,1,1,1,4,4,3,0,0,0,
           0,0,0,3,3,3,3,3,3,3,3,3,3,0,0,0,
        } };
#else
        Sprite king = {
           "       33       "
           "      2332      "
           "    23311332    "
           "    32114443    "
           "    32111123    "
           "     321113     "
           "     321123     "
           "      3223      "
           "     321143     "
           "      3243      "
           "      3143      "
           "     321143     "
           "     321143     "
           "    32221443    "
           "   3221111443   "
           "   3333333333   "
        };
#endif
    }

    namespace symbol
    {
#if UCHARARRAY_SPRITE_METHOD
        Sprite n0 = { {
           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
           0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,
           0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,
           0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,
           0,0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,
           0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0,
           0,0,0,0,1,0,0,1,1,0,1,0,0,0,0,0,
           0,0,0,0,1,0,1,1,0,0,1,0,0,0,0,0,
           0,0,0,0,1,1,1,0,0,0,1,0,0,0,0,0,
           0,0,0,0,1,1,0,0,0,0,1,0,0,0,0,0,
           0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,
           0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,
           0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,
           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        } };
#else
        Sprite n0 = { 
           "                "
           "      111       "
           "     1   1      "
           "    1     1     "
           "    1     1     "
           "    1    11     "
           "    1   1 1     "
           "    1  1  1     "
           "    1 1   1     "
           "    11    1     "
           "    1     1     "
           "     1   1      "
           "      111       "
           "                "
         };
#endif
        Sprite n1 = {
           "                "
           "       1        "
           "      11        "
           "     1 1        "
           "    1  1        "
           "       1        "
           "       1        "
           "       1        "
           "       1        "
           "       1        "
           "       1        "
           "       1        "
           "    1111111     "
           "                "
        };
    }
}

void Ghost::Move(PixelPos newPos)
{
    g_frameBuffer.DrawGhost(newPos, pos, sprite, team);
    pos = newPos;
}

// https://easings.net/#easeInOutQuad
double Ghost::EaseInOutQuad(double t)
{
    if (0.0 <= t && t < 0.5)
        return 2.0 * t * t;
    else if (0.5 <= t && t <= 1)
        return 1.0 - pow(-2.0 * t + 2.0, 2.0) / 2.0;
    else
        return 0.0;
}

Ghost::Ghost()
{
    sprite = &sprite::unit::null;
    pos = { 0,0 };
    team = 0;
}

Ghost::Ghost(Coord space, sprite::Sprite* sprite, bool team)
{
    this->sprite = sprite;
    this->pos = PixelSpace(space);
    this->team = team;
}

void Ghost::Glide(Coord _newPos)
{
    PixelPos startingPos = pos; // Storing this because pos will be changing over the course of this function.
    PixelPos newPos = PixelSpace(_newPos);
    PixelPos distance = newPos - pos;

    g_frameBuffer.BufferCleanplateSpace(BoardSpace(pos)); // Removes the unit sprite from the space in the buffer
    //g_frameBuffer.Draw(); // Sanity check
    
    int napTime = 20; // ms between frames

    double transitionTime = 1.0; // Seconds the transition should take

    double tPerFrame = transitionTime / (double)napTime; // How much t should change per frame

    double deltaX, deltaY;
    deltaX = (double)distance.x;
    deltaY = (double)distance.y;

    // Cumulative, get your mind out of the gutter.
    double cumX = 0.0;
    double cumY = 0.0;


    double t; // Time along the easing curve
    for (t = 0.0; t <= 1.0; t = t + tPerFrame)
    {
        Move(startingPos + PixelPos{ (int)(cumX + 0.5), (int)(cumY + 0.5)});
        cumX = deltaX * EaseInOutQuad(t);
        cumY = deltaY * EaseInOutQuad(t);
        SleepForMS(napTime); // 0.02 seconds
    }
    Move(newPos); // This is here to guarantee absolutely that the ghost will line up with the piece at the end.
}
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <memory.h>

#define sign(x) (((x) > (decltype(x))(0)) - ((x) < (decltype(x))(0)))

#ifndef _countof
#define _countof(x) (sizeof(x) / sizeof(x[0]))
#endif // !_countof


using Vector1 = float;

#pragma region Vector math

inline Vector2  operator- (Vector2  vec)          { return      Vector2Negate(vec);             }
inline Vector2  operator+ (Vector2  a, Vector2 b) { return      Vector2Add(a, b);               }
inline Vector2& operator+=(Vector2& a, Vector2 b) { return (a = Vector2Add(a, b));              }
inline Vector2  operator+ (Vector2  a, float val) { return      Vector2AddValue(a, val);        }
inline Vector2& operator+=(Vector2& a, float val) { return (a = Vector2AddValue(a, val));       }
inline Vector2  operator- (Vector2  a, Vector2 b) { return      Vector2Subtract(a, b);          }
inline Vector2& operator-=(Vector2& a, Vector2 b) { return (a = Vector2Subtract(a, b));         }
inline Vector2  operator- (Vector2  a, float val) { return      Vector2SubtractValue(a, val);   }
inline Vector2& operator-=(Vector2& a, float val) { return (a = Vector2SubtractValue(a, val));  }
inline Vector2  operator* (Vector2  a, Vector2 b) { return      Vector2Multiply(a, b);          }
inline Vector2& operator*=(Vector2& a, Vector2 b) { return (a = Vector2Multiply(a, b));         }
inline Vector2  operator* (Vector2  a, float scl) { return      Vector2Scale(a, scl);           }
inline Vector2& operator*=(Vector2& a, float scl) { return (a = Vector2Scale(a, scl));          }
inline Vector2  operator/ (Vector2  a, Vector2 b) { return      Vector2Divide(a, b);            }
inline Vector2& operator/=(Vector2& a, Vector2 b) { return (a = Vector2Divide(a, b));           }
inline Vector2  operator/ (Vector2  a, float div) { return      Vector2Scale(a, 1.0f / div);    }
inline Vector2& operator/=(Vector2& a, float div) { return (a = Vector2Scale(a, 1.0f / div));   }

inline Vector3  operator- (Vector3  vec)          { return      Vector3Negate(vec);             }
inline Vector3  operator+ (Vector3  a, Vector3 b) { return      Vector3Add(a, b);               }
inline Vector3& operator+=(Vector3& a, Vector3 b) { return (a = Vector3Add(a, b));              }
inline Vector3  operator+ (Vector3  a, float val) { return      Vector3AddValue(a, val);        }
inline Vector3& operator+=(Vector3& a, float val) { return (a = Vector3AddValue(a, val));       }
inline Vector3  operator- (Vector3  a, Vector3 b) { return      Vector3Subtract(a, b);          }
inline Vector3& operator-=(Vector3& a, Vector3 b) { return (a = Vector3Subtract(a, b));         }
inline Vector3  operator- (Vector3  a, float val) { return      Vector3SubtractValue(a, val);   }
inline Vector3& operator-=(Vector3& a, float val) { return (a = Vector3SubtractValue(a, val));  }
inline Vector3  operator* (Vector3  a, Vector3 b) { return      Vector3Multiply(a, b);          }
inline Vector3& operator*=(Vector3& a, Vector3 b) { return (a = Vector3Multiply(a, b));         }
inline Vector3  operator* (Vector3  a, float scl) { return      Vector3Scale(a, scl);           }
inline Vector3& operator*=(Vector3& a, float scl) { return (a = Vector3Scale(a, scl));          }
inline Vector3  operator/ (Vector3  a, Vector3 b) { return      Vector3Divide(a, b);            }
inline Vector3& operator/=(Vector3& a, Vector3 b) { return (a = Vector3Divide(a, b));           }
inline Vector3  operator/ (Vector3  a, float div) { return      Vector3Scale(a, 1.0f / div);    }
inline Vector3& operator/=(Vector3& a, float div) { return (a = Vector3Scale(a, 1.0f / div));   }

#pragma endregion

struct Int2
{
    int x, y;
};
struct IntRec
{
    int x, y, w, h;
};
void BeginScissorModeRec(IntRec section)
{
    BeginScissorMode(section.x, section.y, section.w, section.h);
}

enum BlockID : unsigned char
{
    BLOCK_NULL = 0,
    BLOCK_BORDER = 1,

    BLOCK_I,
    BLOCK_J,
    BLOCK_L,
    BLOCK_O,
    BLOCK_S,
    BLOCK_T,
    BLOCK_Z,

    BLOCK_IDS,
    BLOCK_SHAPES = BLOCK_IDS - BLOCK_I,
};
constexpr int GetBlockWidth(BlockID id)
{
    switch (id)
    {
    case BLOCK_I: return 1;
    case BLOCK_J: return 2;
    case BLOCK_L: return 2;
    case BLOCK_O: return 2;
    case BLOCK_S: return 3;
    case BLOCK_T: return 3;
    case BLOCK_Z: return 3;
    default: return 0;
    }
}
constexpr int GetBlockHeight(BlockID id)
{
    switch (id)
    {
    case BLOCK_I: return 4;
    case BLOCK_J: return 3;
    case BLOCK_L: return 3;
    case BLOCK_O: return 2;
    case BLOCK_S: return 2;
    case BLOCK_T: return 2;
    case BLOCK_Z: return 2;
    default: return 0;
    }
}
struct Piece
{
    struct Orientation
    {
        Int2 off[4];
    };
    Orientation rot[4]; // Four orientations
};
typedef Piece::Orientation Shape;

Color g_pallet[] = 
{
    BLACK,
    GRAY,
    SKYBLUE,
    BLUE,
    ORANGE,
    YELLOW,
    LIME,
    MAGENTA,
    RED,
};
const Piece g_pieces[] =
{
    /********
    * 0 = ^ *
    * 1 = > *
    * 2 = v *
    * 3 = < *
    ********/

    Piece{ /* I */
        Shape{ { { 0,0 }, { 0,1 }, { 0,2 }, { 0,3 } } },
        Shape{ { { 0,0 }, { 1,0 }, { 2,0 }, { 3,0 } } },
        Shape{ { { 0,0 }, { 0,1 }, { 0,2 }, { 0,3 } } },
        Shape{ { { 0,0 }, { 1,0 }, { 2,0 }, { 3,0 } } },
    },
    Piece{ /* J */
        Shape{ { { 1,0 }, { 1,1 }, { 1,2 }, { 0,2 } } },
        Shape{ { { 0,0 }, { 1,0 }, { 2,0 }, { 2,1 } } },
        Shape{ { { 0,0 }, { 0,1 }, { 0,2 }, { 1,0 } } },
        Shape{ { { 0,1 }, { 1,1 }, { 2,1 }, { 0,0 } } },
    },
    Piece{ /* L */
        Shape{ { { 0,0 }, { 0,1 }, { 0,2 }, { 1,2 } } },
        Shape{ { { 0,1 }, { 1,1 }, { 2,1 }, { 2,0 } } },
        Shape{ { { 0,0 }, { 1,0 }, { 1,1 }, { 1,2 } } },
        Shape{ { { 0,0 }, { 1,0 }, { 2,0 }, { 0,1 } } },
    },
    Piece{ /* O */
        Shape{ { { 0,0 }, { 1,0 }, { 1,1 }, { 0,1 } } },
        Shape{ { { 0,0 }, { 1,0 }, { 1,1 }, { 0,1 } } },
        Shape{ { { 0,0 }, { 1,0 }, { 1,1 }, { 0,1 } } },
        Shape{ { { 0,0 }, { 1,0 }, { 1,1 }, { 0,1 } } },
    },
    Piece{ /* S */
        Shape{ { { 0,1 }, { 1,1 }, { 1,0 }, { 2,0 } } },
        Shape{ { { 0,0 }, { 0,1 }, { 1,1 }, { 1,2 } } },
        Shape{ { { 0,1 }, { 1,1 }, { 1,0 }, { 2,0 } } },
        Shape{ { { 0,0 }, { 0,1 }, { 1,1 }, { 1,2 } } },
    },
    Piece{ /* T */
        Shape{ { { 0,0 }, { 1,0 }, { 2,0 }, { 1,1 } } },
        Shape{ { { 0,0 }, { 0,1 }, { 0,2 }, { 1,1 } } },
        Shape{ { { 0,1 }, { 1,1 }, { 2,1 }, { 1,0 } } },
        Shape{ { { 1,0 }, { 1,1 }, { 1,2 }, { 0,1 } } },
    },
    Piece{ /* Z */
        Shape{ { { 0,0 }, { 1,0 }, { 1,1 }, { 2,1 } } },
        Shape{ { { 1,0 }, { 1,1 }, { 0,1 }, { 0,2 } } },
        Shape{ { { 0,0 }, { 1,0 }, { 1,1 }, { 2,1 } } },
        Shape{ { { 1,0 }, { 1,1 }, { 0,1 }, { 0,2 } } },
    }
};

Color& GetBlockColor(BlockID id)
{
    return g_pallet[id];
}
const Shape& GetShape(BlockID id, size_t rot)
{
    return g_pieces[id - 2].rot[rot % 4];
}

template<typename T, size_t _width, size_t _height>
struct Board
{
    static constexpr size_t width()
    {
        return _width;
    }
    static constexpr size_t height()
    {
        return _height;
    }
    static constexpr size_t size()
    {
        return _width * _height;
    }

    T spaces[size()];

    T& operator[](int i)
    {
        return spaces[i];
    }
    T& at(size_t x, size_t y)
    {
        return spaces[y * _width + x];
    }

    const T& operator[](int i) const
    {
        return spaces[i];
    }
    const T& at(size_t x, size_t y) const
    {
        return spaces[y * _width + x];
    }

    void Fill(T value)
    {
        for (int i = 0; i < size(); ++i)
        {
            spaces[i] = value;
        }
    }

    bool CheckCollisionShape(BlockID id, size_t rotation, size_t x, size_t y)
    {
        const Shape& shape = GetShape(id,rotation);
        const Int2 baseOffset = { x,y };
        return (
            !!at(shape.off[0].x + x, shape.off[0].y + y) ||
            !!at(shape.off[1].x + x, shape.off[1].y + y) ||
            !!at(shape.off[2].x + x, shape.off[2].y + y) ||
            !!at(shape.off[3].x + x, shape.off[3].y + y));
    }

    void InsertShape(BlockID id, size_t rotation, size_t x, size_t y)
    {
        const Shape& shape = GetShape(id, rotation);
        const Int2 baseOffset = { x,y };
        at(shape.off[0].x + x, shape.off[0].y + y) = id;
        at(shape.off[1].x + x, shape.off[1].y + y) = id;
        at(shape.off[2].x + x, shape.off[2].y + y) = id;
        at(shape.off[3].x + x, shape.off[3].y + y) = id;
    }

    void ClearChunk(
        size_t startLine,   // The y-position of the beginning of the chunk
        size_t lineCount,   // The number of lines in the chunk
        int clearValue = 0, // What to clear the duplicate values with
        size_t border = 0)  // The size of the board's border
    {
        /***************
        * 
        *   ########
        *   AB    C#
        *   #   x  #
        *   # xxx  #
        *   #xx xxx#
        *   #x  x x#
        *   #x xx x#
        *   D------#
        *   #------#
        *   #xx  xx#
        *   #xxxxx #
        *   ########
        * 
        ***************/
        const size_t borderOffsetTop = width() * border;        // A
        const size_t gutsWidth = width() - 2 * border;          // Size of B..C
        T* const guts = spaces + borderOffsetTop + border;      // B
        const size_t chunkSize = width() * lineCount;           // Size of the dashed area
        const size_t copySize = chunkSize - borderOffsetTop;    // Size from A to D
        T* const offsetPos = guts + chunkSize;                  // Where the memory from the chunk A..D is getting moved to (which is A + the size of the dashed area)

        // Move (copy) the entire board starting at the top border and ending at the line being destroyed down a block, overwriting the lines being destroyed.
        memmove((void*)offsetPos, (void*)guts, copySize);

        // Now that the memory has been moved (copied), we need to reset the spaces that were originally at the top
        T* clearLine = guts;
        for (size_t y = 0; y < lineCount; ++y, clearLine += width())
        {
            memset((void*)clearLine, clearValue, gutsWidth);
        }
    }
};

int main()
{
    constexpr size_t borderWidth = 1;
    using GameBoard = Board<BlockID, 10 + 2 * borderWidth, 24 + 2 * borderWidth>;
    GameBoard board;
    board.Fill(BLOCK_NULL);
    for (size_t y = 0; y < board.height(); ++y)
    {
        board.at(0, y) = BLOCK_BORDER;
        board.at(board.width() - 1, y) = BLOCK_BORDER;
    }
    for (size_t x = 0; x < board.width(); ++x)
    {
        board.at(x, 0) = BLOCK_BORDER;
        board.at(x, board.height() - 1) = BLOCK_BORDER;
    }

    BlockID block_current = BLOCK_NULL;
    auto RandomBlock = [&block_current] { return (BlockID)((rand() % 7) + BLOCK_I); };
    BlockID block_next = RandomBlock();
    int rotation = 0;

    constexpr size_t boardMiddle = (GameBoard::width() - 2 * borderWidth) / 2ull + borderWidth;
    size_t positionX;
    size_t positionY;

    auto SpawnPiece = [&]
    {
        block_current = block_next;
        block_next = RandomBlock();
        positionX = boardMiddle - GetBlockWidth(block_current) / 2ull;
        positionY = borderWidth;
        rotation = 0;
    };

    size_t level = 0;
    size_t score = 0;

    size_t simTime = 30;
    constexpr size_t shiftTime = 0;
    size_t shift = 0;
    size_t timer = 0;

    bool win = false;

    constexpr size_t boardScale = 8;
    constexpr size_t uiWidth = 6;
    constexpr size_t windowWidth = (board.width() + uiWidth) * boardScale;
    constexpr size_t windowHeight = board.height() * boardScale;
    InitWindow(windowWidth, windowHeight, "Tetris");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    Texture2D blockTexture = LoadTexture("block.png");
    SetShapesTexture(blockTexture, { 0,0,32,32 });

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        int pXD = (int)IsKeyPressed(KEY_RIGHT) - (int)IsKeyPressed(KEY_LEFT);
        size_t pX = (size_t)((long long)positionX + (long long)pXD);
        bool canMoveX = !board.CheckCollisionShape(block_current, rotation, pX, positionY);
        pXD *= canMoveX;
        positionX = (size_t)((long long)positionX + (long long)pXD);

        if (IsKeyPressed(KEY_UP))
        {
            int pRotation = (rotation - 1) % 4;
            bool canRotatePos = !board.CheckCollisionShape(block_current, pRotation, positionX, positionY);
            if (canRotatePos)
            {
                rotation = pRotation;
            }
        }
        else if (IsKeyPressed(KEY_DOWN))
        {
            int pRotation = (rotation + 1) % 4;
            bool canRotateNeg = !board.CheckCollisionShape(block_current, pRotation, positionX, positionY);
            if (canRotateNeg)
            {
                rotation = pRotation;
            }
        }
        if (IsKeyPressed(KEY_RIGHT_SHIFT))
        {
            shift = level;
        }
        if (IsKeyDown(KEY_RIGHT_SHIFT) && shift == level && timer < (simTime - shiftTime))
        {
            timer = 0;
        }
        

        if (!timer && !win) // Simulation frame
        {
            if (!block_current) SpawnPiece();
            bool colliding = board.CheckCollisionShape(block_current, rotation, positionX, positionY + 1);
            if (colliding)
            {
                board.InsertShape(block_current, rotation, positionX, positionY);
                SpawnPiece();
                if (board.CheckCollisionShape(block_current, rotation, positionX, positionY + 1)) win = true; // TODO: Needs work.
                ++level;
            }
            else
            {
                ++positionY;
            }

            size_t startClear;
            size_t clearSize = 0;
            bool clearLine = false;
            for (size_t y = 1; y < board.height() - 1; ++y)
            {
                if (!clearLine) startClear = y;
                bool clearLine = true;
                for (size_t x = 1; x < board.width() - 1; ++x)
                {
                    if (!board.at(x, y))
                    {
                        clearLine = false;
                        break;
                    }
                }
                if (clearLine) ++clearSize;
                else if (clearSize > 0) // There are lines to clear
                {
                    board.ClearChunk(startClear, clearSize, BLOCK_NULL, 1);
                    score += 10;
                    clearSize = 0;
                }
            }

            timer = simTime; // Reset the clock
        }
        else
        {
            --timer;
        }

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            // Existing
            for (size_t y = 0; y < board.height(); ++y)
            {
                size_t screenY = y * boardScale;
                for (size_t x = 0; x < board.width(); ++x)
                {
                    size_t screenX = x * boardScale;
                    BlockID id = board.at(x, y);
                    if (id)
                    {
                        DrawRectangle((int)screenX, (int)screenY, (int)boardScale, (int)boardScale, GetBlockColor(id));
                    }
                }
            }
            // In-play
            {
                Color color = GetBlockColor(block_current);
                for (Int2 pos : GetShape(block_current, rotation).off)
                {
                    size_t screenX = (pos.x + positionX) * boardScale;
                    size_t screenY = (pos.y + positionY) * boardScale;
                    DrawRectangle((int)screenX, (int)screenY, (int)boardScale, (int)boardScale, color);
                }
            }
            // UI
            {
                for (size_t y = 0; y < board.height(); ++y)
                {
                    size_t screenY = y * boardScale;
                    for (size_t x = board.width(); x < board.width() + uiWidth; ++x)
                    {
                        size_t screenX = x * boardScale;
                        DrawRectangle((int)screenX, (int)screenY, (int)boardScale, (int)boardScale, GetBlockColor(BLOCK_BORDER));
                    }
                }
                IntRec elementRec;
                elementRec.x = (board.width() + 1) * boardScale;
                elementRec.y = boardScale;
                elementRec.w = (uiWidth - 2) * boardScale;
                elementRec.h = boardScale;
                BeginScissorModeRec(elementRec);
                {
                    ClearBackground(BLACK);
                    DrawText(TextFormat("LVL: %i",level), elementRec.x, elementRec.y, elementRec.h, WHITE);
                }
                elementRec.y *= 3;
                BeginScissorModeRec(elementRec);
                {
                    ClearBackground(BLACK);
                    DrawText(TextFormat("%i",score), elementRec.x, elementRec.y, elementRec.h, WHITE);
                }
                EndScissorMode();
                elementRec.y = boardScale * 6;
                elementRec.h = elementRec.w;
                BeginScissorModeRec(elementRec);
                {
                    ClearBackground(BLACK);

                    Color color = GetBlockColor(block_next);
                    size_t offsetX = ((4ull * boardScale) - (GetBlockWidth(block_next) * boardScale)) / 2ull;
                    size_t offsetY = ((4ull * boardScale) - (GetBlockHeight(block_next) * boardScale)) / 2ull;
                    for (Int2 pos : GetShape(block_next, 0).off)
                    {
                        size_t screenX = pos.x * boardScale + (size_t)elementRec.x + offsetX;
                        size_t screenY = pos.y * boardScale + (size_t)elementRec.y + offsetY;
                        DrawRectangle((int)screenX, (int)screenY, (int)boardScale, (int)boardScale, color);
                    }
                }
                EndScissorMode();
            }

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    UnloadTexture(blockTexture);

    CloseWindow();

	return 0;
}
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>

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
    size_t x, y;
};
Int2 operator+(Int2 a, Int2 b)
{
    return Int2{ a.x + b.x, a.y + b.y };
}
bool operator==(Int2 a, Int2 b)
{
    return a.x == b.x && a.y == b.y;
}
struct IntRec
{
    int x, y, w, h;
};
void BeginScissorModeRec(IntRec section)
{
    BeginScissorMode(section.x, section.y, section.w, section.h);
}

enum BlockID : char
{
    BLOCK_NULL,
    BLOCK_BORDER,
    BLOCK_I,
    BLOCK_J,
    BLOCK_L,
    BLOCK_O,
    BLOCK_S,
    BLOCK_T,
    BLOCK_Z,
};
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
    WHITE,
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
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
    },
    Piece{ /* J */
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
    },
    Piece{ /* L */
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
    },
    Piece{ /* O */
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
    },
    Piece{ /* S */
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
    },
    Piece{ /* T */
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
    },
    Piece{ /* Z */
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
        Shape{ { { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 } } },
    }
};

const Color& GetBlockColor(BlockID id)
{
    return g_pallet[id];
}
const Shape& GetShape(BlockID id, size_t rot)
{
    return g_pieces[id - 2].rot[rot];
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
    T& operator[](Int2 p)
    {
        return spaces[p.y * _width + p.x];
    }

    const T& operator[](int i) const
    {
        return spaces[i];
    }
    const T& operator[](Int2 p) const
    {
        return spaces[p.y * _width + p.x];
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
            !!(*this)[(shape.off[0] + baseOffset)] ||
            !!(*this)[(shape.off[1] + baseOffset)] ||
            !!(*this)[(shape.off[2] + baseOffset)] ||
            !!(*this)[(shape.off[3] + baseOffset)]);
    }

    void InsertShape(BlockID id, size_t rotation, size_t x, size_t y)
    {
        const Shape& shape = GetShape(id, rotation);
        const Int2 baseOffset = { x,y };
        (*this)[(shape.off[0] + baseOffset)] = id;
        (*this)[(shape.off[1] + baseOffset)] = id;
        (*this)[(shape.off[2] + baseOffset)] = id;
        (*this)[(shape.off[3] + baseOffset)] = id;
    }
};

int main()
{
    Board<BlockID, 10 + 2, 24 + 2> board;
    board.Fill(BLOCK_NULL);
    for (size_t y = 0; y < board.height(); ++y)
    {
        board[{ 0, y }] = BLOCK_BORDER;
        board[{ board.width() - 1, y }] = BLOCK_BORDER;
    }
    for (size_t x = 0; x < board.width(); ++x)
    {
        board[{ x, 0 }] = BLOCK_BORDER;
        board[{ x, board.height() - 1 }] = BLOCK_BORDER;
    }

    auto RandomBlock = [] { return (BlockID)((rand() % 7) + 2); };
    BlockID block_current = BLOCK_NULL;
    BlockID block_next = RandomBlock();
    int rotation = 0;

    auto RandomX = [&board] { return (size_t)((rand() % (board.width() - 2)) + 1); };
    size_t positionX;
    size_t positionY;

    auto SpawnPiece = [&]
    {
        block_current = block_next;
        block_next = RandomBlock();
        positionX = RandomX();
        positionY = 1;
    };

    size_t level = 0;
    size_t score = 0;

    size_t simTime = 10;
    size_t timer = 0;

    constexpr size_t boardScale = 32;
    constexpr size_t uiWidth = 6;
    constexpr size_t windowWidth = (board.width() + uiWidth) * boardScale;
    constexpr size_t windowHeight = board.height() * boardScale;
    InitWindow(windowWidth, windowHeight, "Tetris");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    Texture2D blockTexture = LoadTexture("block.png");
    SetShapesTexture(blockTexture, {0,0,32,32});

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        if (IsKeyPressed(KEY_LEFT))
        {
            bool canMoveLeft = !board.CheckCollisionShape(block_current, rotation, positionX - 1, positionY);
            if (canMoveLeft)
            {
                --positionX;
            }
        }
        else if (IsKeyPressed(KEY_RIGHT))
        {
            bool canMoveRight = !board.CheckCollisionShape(block_current, rotation, positionX + 1, positionY);
            if (canMoveRight)
            {
                ++positionX;
            }
        }
        if (IsKeyPressed(KEY_UP))
        {
            bool canRotatePos = !board.CheckCollisionShape(block_current, (rotation + 1) % 4, positionX, positionY);
            if (canRotatePos)
            {
                --rotation;
            }
        }
        else if (IsKeyPressed(KEY_DOWN))
        {
            bool canRotateNeg = !board.CheckCollisionShape(block_current, (rotation + 3) % 4, positionX, positionY);
            if (canRotateNeg)
            {
                ++rotation;
            }
        }
        if (IsKeyDown(KEY_RIGHT_SHIFT))
        {
            timer = 0;
        }
        

        if (!timer) // Simulation frame
        {
            if (!block_current) SpawnPiece();
            bool colliding = board.CheckCollisionShape(block_current, rotation, positionX, positionY + 1);
            if (colliding)
            {
                board.InsertShape(block_current, rotation, positionX, positionY);
                SpawnPiece();
                ++level;
            }
            else
            {
                ++positionY;
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
                    BlockID id = board[{ x, y }];
                    if (id)
                    {
                        DrawRectangle(screenX, screenY, boardScale, boardScale, GetBlockColor(id));
                    }
                }
            }
            // In-play
            {
                Color color = GetBlockColor(block_current);
                for (Int2 pos : GetShape(block_current, rotation).off)
                {
                    int screenX = (pos.x + positionX) * boardScale;
                    int screenY = (pos.y + positionY) * boardScale;
                    DrawRectangle(screenX, screenY, boardScale, boardScale, color);
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
                        DrawRectangle(screenX, screenY, boardScale, boardScale, GetBlockColor(BLOCK_BORDER));
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
            }

            DrawText(TextFormat("%i",timer),0,0,32,RED);

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    UnloadTexture(blockTexture);

    CloseWindow();

	return 0;
}
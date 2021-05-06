#include <raylib.h>
#include <raymath.h>
#include <deque>
#include <vector>
#include <stack>
#include <queue>

#define _EDITOR 1

#define sign(x) (((x) > (decltype(x))(0)) - ((x) < (decltype(x))(0)))

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

struct Ball
{
    Vector2 center;
    float radius;
    Vector2 velocity;
};
void Bounce(Ball& ball, Vector2 normal)
{
    ball.velocity = Vector2Reflect(ball.velocity, normal);
}

Color g_blockHealthColors[] =
{
    RED,
    ORANGE,
    YELLOW,
    BLUE,
    PURPLE,
    DARKGRAY,
};

struct Block
{
    Rectangle collision;
    int health;
};

struct DebugLineData
{
    Vector2 start;
    Vector2 end;
    Color color;
    float life;
};
std::deque<DebugLineData> g_debugLines;

void PushDebugLine(Vector2 start, Vector2 end, Color color, float life = 0.0f)
{
    if (life == 0.0f)
        return g_debugLines.push_front({ start,end,color,life });

    for (size_t i = 0; i < g_debugLines.size(); ++i)
    {
        if (g_debugLines[i].life >= life)
        {
            g_debugLines.insert(g_debugLines.begin() + std::max((long long)i - 1, 0ll), { start,end,color,life });
            return;
        }
    }
}

void DrawDebug()
{
    float time = GetFrameTime();
    size_t eol = 0;
    for (size_t i = 0; i < g_debugLines.size(); ++i)
    {
        if (g_debugLines[i].life >= 0.0f)
            DrawLineV(g_debugLines[i].start, g_debugLines[i].end, g_debugLines[i].color);
        g_debugLines[i].life -= time;
        eol += (int)(g_debugLines[i].life <= 0.0f);
    }
    for (size_t i = 0; i < eol; ++i)
    {
        g_debugLines.pop_front();
    }
}

// Returns a reference to the variable holding the smaller value (favors a)
template<typename T> T& rmin(T& a, T& b)
{
    if (a > b) return b;
    else return a;
}
// Returns a reference to the variable holding the smaller value (favors a)
template<typename T> T& rmax(T& a, T& b)
{
    if (a < b) return b;
    else return a;
}

template<typename T> std::tuple<T&, T&> rsort(T& a, T& b)
{
    if (a < b) return { a, b };
    else return { b, a };
}

#define NORM_RIGHT { 1.0f, 0.0f }
#define NORM_LEFT { -1.0f, 0.0f }
#define NORM_UP { 0.0f, 1.0f }
#define NORM_DOWN { 0.0f, -1.0f }

constexpr int c_brickScale = 16;
constexpr float c_brickWFactor = 1.0f / (float)(c_brickScale * 2);
constexpr float c_brickHFactor = 1.0f / (float)(c_brickScale);

#define BRICK_RECT(x,y) { (2 * c_brickScale) * (x), c_brickScale * (y), (2 * c_brickScale), c_brickScale }

Vector2 BrickSnap(Vector2 pos)
{
    pos.x -= (float)c_brickScale;
    pos.y -= (float)(c_brickScale / 2);
    pos.x = (int)roundf(pos.x * c_brickWFactor) * (float)(c_brickScale * 2);
    pos.y = (int)roundf(pos.y * c_brickHFactor) * (float)(c_brickScale);
    return pos;
}

int main()
{
    int windowWidth = c_brickScale * 10 * 2;
    int windowHeight = c_brickScale * 10;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    Texture blockTex = LoadTexture("brick_base.png");
    Texture ballTex = LoadTexture("ball.PNG");

    std::vector<Block> blocks
    {
        { BRICK_RECT(0.0f, 0.0f), 1 },
        { BRICK_RECT(1.0f, 0.0f), 1 },
        { BRICK_RECT(2.0f, 0.0f), 1 },
        { BRICK_RECT(3.0f, 0.0f), 1 },
        { BRICK_RECT(4.0f, 0.0f), 1 },
        { BRICK_RECT(5.0f, 0.0f), 1 },
        { BRICK_RECT(2.0f, 1.0f), 1 },
        { BRICK_RECT(3.0f, 1.0f), 1 },
        { BRICK_RECT(4.0f, 1.0f), 1 },
    };
    Rectangle paddle{ 0, windowHeight - 16, 32, 8 };
    std::vector<Ball> balls;
    balls.push_back({ { 100.0f,128.0f }, 6.0f, { 1.0f,-5.0f } });

    Vector2 mouse;
#if _EDITOR
    Block* hovered = nullptr;
#endif

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        mouse = GetMousePosition();

        Vector2 paddleVelocity = { paddle.x, paddle.y };
        paddle.x = mouse.x - paddle.width / 2;
        paddleVelocity = Vector2{ paddle.x, paddle.y } - paddleVelocity;

        for (Ball& ball : balls)
        {
            ball.center += ball.velocity;

            if (CheckCollisionCircleRec(ball.center, ball.radius, paddle))
            {
                Vector2 center = {
                    paddle.x + paddle.width * 0.5f,
                    paddle.y + paddle.height * 0.5f
                };
                Vector2 normal = ball.center - center;
                normal = Vector2Normalize(normal);

                normal.x = roundf(normal.x);
                normal.y = roundf(normal.y);

                normal = Vector2Normalize(normal);

                ball.velocity = Vector2Reflect(ball.velocity, normal) + paddleVelocity;
                ball.velocity = Vector2Normalize(ball.velocity) * 5.0f;

                while (CheckCollisionCircleRec(ball.center, ball.radius, paddle))
                {
                    ball.center += ball.velocity;
                }
            }

            if (ball.center.x - ball.radius <= 0.0f) // Left wall
            {
                ball.velocity = Vector2Reflect(ball.velocity, NORM_RIGHT);
                while (ball.center.x - ball.radius <= 0.0f)
                {
                    ball.center += ball.velocity;
                }
            }

            if (ball.center.x + ball.radius >= windowWidth) // Right wall
            {
                ball.velocity = Vector2Reflect(ball.velocity, NORM_LEFT);
                while (ball.center.x + ball.radius >= windowWidth)
                {
                    ball.center += ball.velocity;
                }
            }

            if (ball.center.y - ball.radius <= 0.0f) // Top wall
            {
                ball.velocity = Vector2Reflect(ball.velocity, NORM_DOWN);
                while (ball.center.y - ball.radius <= 0.0f)
                {
                    ball.center += ball.velocity;
                }
            }

            if (ball.center.y + ball.radius >= windowHeight) // Bottom wall
            {
                ball.velocity = Vector2Reflect(ball.velocity, NORM_UP);
                while (ball.center.y + ball.radius >= windowHeight)
                {
                    ball.center += ball.velocity;
                }
            }

            for (Block& b : blocks)
            {
                if (b.health >= 0 && CheckCollisionCircleRec(ball.center, ball.radius, b.collision))
                {
                    Vector2 center = {
                        b.collision.x + b.collision.width * 0.5f,
                        b.collision.y + b.collision.height * 0.5f
                    };
                    Vector2 normal = ball.center - center;
                    normal.x /= 2; // Twice as wide as tall
                    normal = Vector2Normalize(normal);

                    normal.x = roundf(normal.x);
                    normal.y = roundf(normal.y);

                    normal = Vector2Normalize(normal);
                    
                    ball.velocity = Vector2Reflect(ball.velocity, normal);
                    b.health--;
                    while (CheckCollisionCircleRec(ball.center, ball.radius, b.collision))
                    {
                        ball.center += ball.velocity;
                    }
                }
            }
            for (int i = blocks.size() - 1; i >= 0; --i)
            {
                if (blocks[i].health < 0)
                {
                    blocks.erase(blocks.begin() + i);
                }
            }
        }

#if _EDITOR
        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
        {
            for (size_t i = 0; i < blocks.size(); ++i)
            {
                if (CheckCollisionPointRec(mouse, blocks[i].collision))
                {
                    blocks.erase(blocks.begin() + i);
                    hovered = nullptr;
                    break;
                }
            }
        }
        else if (IsMouseButtonUp(MOUSE_LEFT_BUTTON))
        {
            hovered = nullptr;
            for (Block& b : blocks)
            {
                if (CheckCollisionPointRec(mouse, b.collision))
                {
                    hovered = &b;
                    break;
                }
            }
        }
        if (hovered)
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                Vector2 pos = BrickSnap(mouse);
                hovered->collision.x = pos.x;
                hovered->collision.y = pos.y;
            }
            else
            {
                hovered->health += (int)GetMouseWheelMove();
                hovered->health = std::min(std::max(0, hovered->health), (int)_countof(g_blockHealthColors) - 1);
            }
        }
        else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !hovered)
        {
            blocks.push_back({ BRICK_RECT(0.0f, 0.0f), 1 });
            hovered = &blocks.back();
            Vector2 pos = BrickSnap(mouse);
            hovered->collision.x = pos.x;
            hovered->collision.y = pos.y;
        }
#endif

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            SetShapesTexture(blockTex, { 0, 0, (float)blockTex.width, (float)blockTex.height });
            for (Block& block : blocks)
            {
                DrawRectangleRec(block.collision, g_blockHealthColors[block.health]);
            }
#if _EDITOR
            if (hovered)
            {
                Color color = g_blockHealthColors[hovered->health];
                color.r += 10;
                color.g += 10;
                color.b += 10;
                DrawRectangleRec(hovered->collision, color);
            }
#endif
            DrawRectangleRec(paddle, GRAY);

            SetShapesTexture(ballTex, { 0, 0, (float)ballTex.width, (float)ballTex.height });
            for (Ball& ball : balls)
            {
                DrawRectangleRec({ ball.center.x - ball.radius, ball.center.y - ball.radius, ball.radius * 2, ball.radius * 2 }, WHITE);
            }

            DrawDebug();

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    // @TODO: Unload variables

    UnloadTexture(blockTex);
    UnloadTexture(ballTex);

    CloseWindow();

#if _EDITOR // Copy and paste these values into the blocks initializer
    printf("\n{\n");
    for (Block& block : blocks)
    {
        printf("\t{ BRICK_RECT(%i.0f, %i.0f), 1 },\n",
               (int)roundf(block.collision.x * c_brickWFactor),
               (int)roundf(block.collision.y * c_brickHFactor) );
    }
    printf("};\n");
#endif

	return 0;
}
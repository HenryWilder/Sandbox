#include <raylib.h>
#include <raymath.h>
#include <functional>

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

float Circle(Vector2 pt)
{
    constexpr float radius = 100.0f;
    return (Vector2Distance(pt, { 100,100 }) / radius);
}

constexpr int squareSize = 16;

void DrawSquare(
    int x, int y,
    bool x0y0, bool x1y0,
    bool x0y1, bool x1y1,
    bool center = true)
{
    constexpr int f = squareSize;
    constexpr int h = squareSize / 2;

    int x0, y0,
        x1, y1;

    if (x0y0)
    {
        if (x1y0)
        {
            if (x0y1)
            {
                if (x1y1)
                {
                    return;
                }
                else // !x1y1
                {
                    x0 = h;

                }
            }
            else // !x0y1
            {
                if (x1y1)
                {

                }
                else // !x1y1
                {

                }
            }
        }
        else // !x1y0
        {
            if (x0y1)
            {
                if (x1y1)
                {

                }
                else // !x1y1
                {

                }
            }
            else // !x0y1
            {
                if (x1y1)
                {

                }
                else // !x1y1
                {

                }
            }
        }
    }
    else // !x0y0
    {
        if (x1y0)
        {
            if (x0y1)
            {
                if (x1y1)
                {

                }
                else // !x1y1
                {

                }
            }
            else // !x0y1
            {
                if (x1y1)
                {

                }
                else // !x1y1
                {

                }
            }
        }
        else // !x1y0
        {
            if (x0y1)
            {
                if (x1y1)
                {

                }
                else // !x1y1
                {

                }
            }
            else // !x0y1
            {
                if (x1y1)
                {

                }
                else // !x1y1
                {
                    return;
                }
            }
        }
    }

    DrawLine(x, y, x, y, WHITE);
}

int main()
{
    constexpr int windowWidth = 1280;
    constexpr int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    std::function<float(Vector2)> f = Circle;
    bool buff[windowHeight / squareSize][windowWidth / squareSize]; // Grid squares are 16x16

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        for (int y = 0; y < windowHeight / squareSize; ++y)
        {
            for (int x = 0; x < windowWidth / squareSize; ++x)
            {
                buff[y][x] = (f({ (float)x * squareSize, (float)y * squareSize }) <= 1.0f);
            }
        }

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            for (int y = 0; y < windowHeight / squareSize; ++y)
            {
                for (int x = 0; x < windowWidth / squareSize; ++x)
                {
                    if (buff[y][x])
                        DrawPixel(x * squareSize, y * squareSize, WHITE);
                }
            }

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    // @TODO: Unload variables

    CloseWindow();

	return 0;
}
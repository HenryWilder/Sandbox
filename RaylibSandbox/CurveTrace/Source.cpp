#include <raylib.h>
#include <raymath.h>

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

struct Body
{
    Vector3 position;
    float radius;
    Color color;
    Color emissive;
    float mass;
};
struct Photon
{
    Vector3 position;
    Vector3 velocity;
    float mass;
};

float Gravity(Vector3 p1, Vector3 p2, float m1, float m2)
{
    const static float G = 0.00000000006673f;  // Gravitational constant
    float d = Vector3Distance(p1, p2);
    return G * m1 * m2 / (d * d);
}

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    int ticksPerFrame = 1000;
    float dtPerTick = 10.0f;

    Camera3D camera;
    camera.position = {-50.0f,0,0};
    camera.target = {0,0,0};
    camera.up = {0,1,0};
    camera.fovy = 45;
    camera.type = CAMERA_FREE;

    Body bodies[] =
    {
        { { 0.0f, 0.0f, 0.0f}, 10.0f, WHITE, BLACK, 10.0f },
    };

    int x = 0;
    int y = 0;

    RenderTexture2D render = LoadRenderTexture(windowWidth, windowHeight);

    BeginTextureMode(render); {

        ClearBackground(BLACK);

    } EndTextureMode();

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        Color pixelColor = MAGENTA;

        float yPos = (float)y / (float)windowHeight;
        float xPos = (float)x / (float)windowWidth;

        Photon pixel;
        pixel.mass = 0.0000000001f;
        pixel.position = camera.position;
        {
            Vector3 forward = Vector3Normalize(camera.target - camera.position);
            Vector3 right = Vector3CrossProduct(forward, { 0.0f, -1.0f, 0.0f });
            Vector3 up = Vector3CrossProduct(right, forward);
            pixel.velocity = Vector3Normalize(forward + right * (xPos - 0.5) + up * (yPos - 0.5));
        }
        for (int t = 0; t < ticksPerFrame; ++t)
        {
            for (int i = 0; i < (sizeof(bodies) / sizeof(Body)); ++i)
            {
                Vector3 force = Vector3Normalize(bodies[i].position - pixel.position) * Gravity(pixel.position, bodies[i].position, pixel.mass, bodies[i].mass);
                pixel.velocity += (force / pixel.mass) * dtPerTick;
            }
            pixel.position += pixel.velocity;
            for (int i = 0; i < (sizeof(bodies) / sizeof(Body)); ++i)
            {
                if (Vector3Distance(pixel.position, bodies[i].position) <= bodies[i].radius)
                {
                    pixelColor = bodies[i].color;
                    goto NEXT;
                }
            }
        }

        NEXT:

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginTextureMode(render); {

            DrawPixel(x, y, pixelColor);

        } EndTextureMode();

        BeginDrawing(); {

            ClearBackground(BLACK);

            DrawTexture(render.texture, 0, 0, WHITE);

        } EndDrawing();

        ++x;
        if (x == windowWidth)
        {
            x = 0;
            ++y;
            if (y == windowHeight)
                y = 0;
        }
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    UnloadRenderTexture(render);

    CloseWindow();

	return 0;
}
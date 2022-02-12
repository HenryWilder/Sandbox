#include <raylib.h>
#include <raymath.h>
#include <algorithm>
#include <time.h>

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
inline Vector2  operator* (float scl, Vector2  a) { return      Vector2Scale(a, scl);           }
inline Vector2& operator*=(float scl, Vector2& a) { return (a = Vector2Scale(a, scl));          }
inline Vector2  operator/ (Vector2  a, Vector2 b) { return      Vector2Divide(a, b);            }
inline Vector2& operator/=(Vector2& a, Vector2 b) { return (a = Vector2Divide(a, b));           }
inline Vector2  operator/ (Vector2  a, float div) { return      Vector2Scale(a, 1.0f / div);    }
inline Vector2& operator/=(Vector2& a, float div) { return (a = Vector2Scale(a, 1.0f / div));   }
inline Vector2  operator/ (float div, Vector2  a) { return      Vector2Divide({ div,div }, a);  }
inline Vector2& operator/=(float div, Vector2& a) { return (a = Vector2Divide({ div,div }, a)); }


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
inline Vector3  operator* (float scl, Vector3  a) { return      Vector3Scale(a, scl);           }
inline Vector3& operator*=(float scl, Vector3& a) { return (a = Vector3Scale(a, scl));          }
inline Vector3  operator/ (Vector3  a, Vector3 b) { return      Vector3Divide(a, b);            }
inline Vector3& operator/=(Vector3& a, Vector3 b) { return (a = Vector3Divide(a, b));           }
inline Vector3  operator/ (Vector3  a, float div) { return      Vector3Scale(a, 1.0f / div);    }
inline Vector3& operator/=(Vector3& a, float div) { return (a = Vector3Scale(a, 1.0f / div));   }
inline Vector3  operator/ (float div, Vector3  a) { return      Vector3Divide({ div,div,div }, a);  }
inline Vector3& operator/=(float div, Vector3& a) { return (a = Vector3Divide({ div,div,div }, a)); }

#pragma endregion

float RandomFloatInRange(float min, float max)
{
    return ((float)rand() / (float)RAND_MAX) * (max - min) + min;
}

int main()
{
    int windowWidth = 720;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    Shader metaballShader = LoadShader(0,"Metaball.frag");
    RenderTexture2D screen = LoadRenderTexture(windowWidth, windowHeight);
    BeginTextureMode(screen);
    ClearBackground(WHITE);
    EndTextureMode();

    int metaCursorLoc = GetShaderLocation(metaballShader, "cursor");
    int metaPtLoc[4];
    int metaRadLoc[4];
    for (int i = 0; i < 4; ++i)
    {
        char name[3] = { 'p', '0' + i, 0 };
        metaPtLoc[i] = GetShaderLocation(metaballShader, name);
        name[0] = 'r';
        metaRadLoc[i] = GetShaderLocation(metaballShader, name);
    }

    srand(time(nullptr));

    Vector2 points[4] = {
        { RandomFloatInRange(0,(float)windowWidth), RandomFloatInRange(0,(float)windowHeight) },
        { RandomFloatInRange(0,(float)windowWidth), RandomFloatInRange(0,(float)windowHeight) },
        { RandomFloatInRange(0,(float)windowWidth), RandomFloatInRange(0,(float)windowHeight) },
        { RandomFloatInRange(0,(float)windowWidth), RandomFloatInRange(0,(float)windowHeight) },
    };
    float radius[4] = {
        RandomFloatInRange(20,50),
        RandomFloatInRange(20,50),
        RandomFloatInRange(20,50),
        RandomFloatInRange(20,50),
    };
    Vector2 velocity[4] = {
        Vector2Normalize({ RandomFloatInRange(-1,1), RandomFloatInRange(-1,1) }) * RandomFloatInRange(0,5),
        Vector2Normalize({ RandomFloatInRange(-1,1), RandomFloatInRange(-1,1) }) * RandomFloatInRange(0,5),
        Vector2Normalize({ RandomFloatInRange(-1,1), RandomFloatInRange(-1,1) }) * RandomFloatInRange(0,5),
        Vector2Normalize({ RandomFloatInRange(-1,1), RandomFloatInRange(-1,1) }) * RandomFloatInRange(0,5),
    };

    for (int i = 0; i < 4; ++i)
    {
        SetShaderValue(metaballShader, metaRadLoc[i], radius + i, UNIFORM_FLOAT);
    }

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        float data[2] = { (float)GetMouseX(), (float)GetMouseY() };
        SetShaderValue(metaballShader, metaCursorLoc, data, UNIFORM_VEC2);

        for (int i = 0; i < 4; ++i)
        {
            SetShaderValue(metaballShader, metaPtLoc[i], points + i, UNIFORM_VEC2);
        }
        for (int i = 0; i < 4; ++i)
        {
            points[i] += velocity[i];

            if (points[i].x > windowWidth || points[i].x < 0)
                velocity[i].x *= -1;

            if (points[i].y > windowHeight || points[i].y < 0)
                velocity[i].y *= -1;
        }
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                if (j == i)
                    continue;

                float dist = Vector2Distance(points[j], points[i]);

                if (dist > (radius[j] + radius[i]))
                    velocity[i] += Vector2Normalize(points[j] - points[i]) * ((radius[j] * radius[i] * 2.0f) / (dist * dist));

                // Drag
                else if (dist > 0.0f)
                {
                    Vector2 relativeVel = velocity[i] - velocity[j];
                    velocity[i] -= relativeVel * 0.5f * 0.3f * GetFrameTime();
                    velocity[i] += Vector2Normalize(points[i] - points[j]) * ((radius[j] * radius[i] * 2.0f) / (dist * dist));
                }
            }
        }

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            BeginShaderMode(metaballShader); {

                DrawTexture(screen.texture, 0, 0, WHITE);

            } EndShaderMode();

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    UnloadRenderTexture(screen);
    UnloadShader(metaballShader);

    CloseWindow();

	return 0;
}
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

constexpr float oneTwoHundredFiftyFifth = 1.0f / 255.0f;
Vector3 ColorToVector3(Color color)
{
    Vector3 returnValue = {
        (float)color.r * oneTwoHundredFiftyFifth,
        (float)color.g * oneTwoHundredFiftyFifth,
        (float)color.b * oneTwoHundredFiftyFifth
    };
    return returnValue;
}

namespace RaytraceShader
{
    Shader raytraceShader;

    namespace PlaneData
    {
        int planePositionsLoc = -1;
        int planeNormalsLoc = -1;
        int planeDiffuseColorsLoc = -1;
        int planeEmissiveColorsLoc = -1;
        int planeCountLoc = -1;

        // Keep this synced with the shader
        constexpr int planeCountMax = 5;
        Vector3 planePositions[planeCountMax] = {};
        Vector3 planeNormals[planeCountMax] = {};
        Vector3 planeDiffuseColors[planeCountMax] = {};
        Vector3 planeEmissiveColors[planeCountMax] = {};
        int planeCount = 0;

        // Push to the shader
        void UpdatePlanes()
        {

        }
        void AddPlane(Vector3 position, Vector3 normal, Color diffuseColor, Color emissiveColor)
        {

            ++planeCount;
            UpdatePlanes();
        }
        void RemovePlane(int index)
        {
            if (index != (planeCount - 1))
            {

            }
            --planeCount;
            UpdatePlanes();
        }
        void ModifyPlane(int index, Vector3 position, Vector3 normal, Color diffuseColor, Color emissiveColor)
        {

            UpdatePlanes();
        }
    }

    namespace SphereData
    {
        int spherePositionsLoc = -1;
        int sphereRadiiLoc = -1;
        int sphereDiffuseColorsLoc = -1;
        int sphereEmissiveColorsLoc = -1;
        int sphereCountLoc = -1;

        // Keep this synced with the shader
        constexpr int sphereCountMax = 5;
        Vector3 spherePositions[sphereCountMax] = {};
        float sphereRadii[sphereCountMax] = {};
        Vector3 sphereDiffuseColors[sphereCountMax] = {};
        Vector3 sphereEmissiveColors[sphereCountMax] = {};
        int sphereCount = 0;

        // Push to the shader
        void UpdateSpheres()
        {

        }
        void AddSphere(Vector3 position, float radius, Color diffuseColor, Color emissiveColor)
        {
            spherePositions[sphereCount] = position;
            sphereRadii[sphereCount] = radius;
            sphereDiffuseColors[sphereCount] = ColorToVector3(diffuseColor);
            sphereEmissiveColors[sphereCount] = ColorToVector3(emissiveColor);
            ++sphereCount;
            UpdateSpheres();
        }
        void RemoveSphere(int index)
        {
            if (index != (sphereCount - 1))
            {

            }
            --sphereCount;
            UpdateSpheres();
        }
        void ModifySphere(int index, Vector3 position, float radius, Color diffuseColor, Color emissiveColor)
        {

            UpdateSpheres();
        }
    }
}
using namespace RaytraceShader;
using namespace PlaneData;
using namespace SphereData;

int main()
{
    int windowWidth = 720;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    raytraceShader = LoadShader(0,"RayTrace.frag");
    RenderTexture2D placeholder = LoadRenderTexture(windowWidth, windowHeight);

    int cameraPositionLoc = GetShaderLocation(raytraceShader, "cameraPosition");
    int cameraTargetLoc = GetShaderLocation(raytraceShader, "cameraTarget");

    Camera3D camera = { { -50.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, CAMERA_PERSPECTIVE };
    SetCameraMode(camera, CAMERA_ORBITAL);

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        UpdateCamera(&camera);

        SetShaderValue(raytraceShader, cameraPositionLoc, &camera.position, UNIFORM_VEC3);
        SetShaderValue(raytraceShader, cameraTargetLoc, &camera.target, UNIFORM_VEC3);

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLUE);

            BeginShaderMode(raytraceShader); {

                DrawTexture(placeholder.texture, 0, 0, WHITE);

            } EndShaderMode();

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    UnloadRenderTexture(placeholder);
    UnloadShader(raytraceShader);

    CloseWindow();

    return 0;
}
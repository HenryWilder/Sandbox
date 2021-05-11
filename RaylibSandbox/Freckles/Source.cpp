#include <time.h>
#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>

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

double DistributedRand()
{
    return (double)rand() / (double)RAND_MAX;
}

Color ColorHSVBlend(Color y0, Color y1, float t)
{
    Vector3 y0_hsv = ColorToHSV(y0);
    Vector3 y1_hsv = ColorToHSV(y1);
    Vector3 lerp_result = (y0_hsv + ((y1_hsv - y0_hsv) * t));
    return ColorFromHSV(lerp_result.x, lerp_result.y, lerp_result.z);
}

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/
    srand((unsigned int)clock());

    Shader bloom = LoadShader(0,"bloom.frag");
    
    int texture1024Scale = 4;
    int textureWidth = 1024 * texture1024Scale;
    int textureHeight = textureWidth * 2;

    float freckleRadiusMin = 1.0f;
    float freckleRadiusScale = 5.0f * (float)texture1024Scale;
    int freckleCount = 4000;

    RenderTexture2D tex_emis = LoadRenderTexture(textureWidth, textureHeight);
    BeginTextureMode(tex_emis); {

        ClearBackground({ 0,0,0, 0 });

    } EndTextureMode();

    // Spawn freckles
    for (int i = 0; i < freckleCount; ++i)
    {
        double x = DistributedRand();
        double y = DistributedRand();
        double r = DistributedRand();
        {
            r *= r;
            r = (1.0 - r);
        }
        float freckleRadius = r * freckleRadiusScale + freckleRadiusMin;

        BeginTextureMode(tex_emis); {

            DrawCircleV({ (float)x * (float)tex_emis.texture.width, (float)y * (float)tex_emis.texture.height }, freckleRadius, WHITE);

        } EndTextureMode();
    }

    Camera3D camera;
    camera.fovy = 45.0f;
    camera.position = { -3,0,0 };
    camera.target = { 0,0,0 };
    camera.up = { 0,1,0 };
    camera.type = CAMERA_PERSPECTIVE;
    SetCameraMode(camera, CAMERA_FREE);

    Vector3 light;

    Model body = LoadModelFromMesh(GenMeshSphere(1.0f, 64, 64));
    
    body.materials[0] = LoadMaterialDefault();
    int uniformLoc_LightPos;

    {
        Material& mat = body.materials[0];

        mat.shader = LoadShader("lighting.vert", "lighting.frag");
        uniformLoc_LightPos = GetShaderLocation(mat.shader, "lightPos");
        {
            float size[2] = { (float)tex_emis.texture.width, (float)tex_emis.texture.height };
            SetShaderValue(mat.shader, GetShaderLocation(mat.shader, "size"), size, UNIFORM_VEC2);
        }
        mat.shader.locs[LOC_MAP_EMISSION] = GetShaderLocation(mat.shader, "textureEmis");

        mat.maps[MAP_EMISSION].texture = tex_emis.texture;

        SetTextureFilter(mat.maps[MAP_DIFFUSE].texture, FILTER_BILINEAR);
        SetTextureFilter(mat.maps[MAP_EMISSION].texture, FILTER_BILINEAR);
    }

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        // Move light source
        {
            float t = GetTime();
            light.x = cosf(t) * 8.0f;
            light.y = sinf(t / 3.0f) * 8.0f;
            light.z = sinf(t) * 8.0f;
            float pos[3] = { light.x, light.y, light.z };
            SetShaderValue(body.materials[0].shader, uniformLoc_LightPos, pos, UNIFORM_VEC3);
        }

        UpdateCamera(&camera);

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            BeginShaderMode(bloom); {

                BeginMode3D(camera); {

                    DrawModel(body, Vector3Zero(), 1.0f, WHITE);

                    DrawSphere(light, 0.2f, WHITE);

                } EndMode3D();

            } EndShaderMode();

            DrawFPS(0,0);

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    UnloadRenderTexture(tex_emis);
    UnloadShader(bloom);
    UnloadShader(body.materials[0].shader);
    UnloadModel(body);

    CloseWindow();

	return 0;
}
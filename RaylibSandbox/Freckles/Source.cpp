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

    Shader bloom = LoadShader(0, "bloom.frag");

    int texture1024Scale = 4;
    int textureWidth = 1024 * texture1024Scale;
    int textureHeight = textureWidth * 2;

    float freckleRadiusMin = 1.0f;
    float freckleRadiusScale = 5.0f * (float)texture1024Scale;
    int freckleCountPerFrame = 3;

    RenderTexture2D tex_freckles = LoadRenderTexture(textureWidth, textureHeight);
    BeginTextureMode(tex_freckles); {

        ClearBackground({ 0,0,0, 0 });

    } EndTextureMode();
    RenderTexture2D tex_frecklesTemp = LoadRenderTexture(textureWidth, textureHeight);

    RenderTexture2D tex_lighting = LoadRenderTexture(textureWidth, textureHeight);
    RenderTexture2D tex_accum = LoadRenderTexture(textureWidth, textureHeight);
    BeginTextureMode(tex_accum); {

        ClearBackground(BLACK);

    } EndTextureMode();

    Camera3D camera;
    camera.fovy = 45.0f;
    camera.position = { -3,0,0 };
    camera.target = { 0,0,0 };
    camera.up = { 0,1,0 };
    camera.type = CAMERA_PERSPECTIVE;
    SetCameraMode(camera, CAMERA_FREE);

    Vector3 light;

    Mesh mesh0 = GenMeshSphere(1.0f, 64, 64);
    Mesh mesh1 = GenMeshSphere(1.0f, 64, 64);
    Model body = LoadModelFromMesh(mesh0);
    Model lightCatcher = LoadModelFromMesh(mesh1);

    body.materials[0] = LoadMaterialDefault();
    int uniformLoc_LightPos[2];

    {
        body.materials[0].shader = LoadShader("lighting.vert", "lighting.frag");
        uniformLoc_LightPos[0] = GetShaderLocation(body.materials[0].shader, "lightPos");
        {
            float size[2] = { (float)tex_freckles.texture.width, (float)tex_freckles.texture.height };
            SetShaderValue(body.materials[0].shader, GetShaderLocation(body.materials[0].shader, "size"), size, UNIFORM_VEC2);
        }

        body.materials[0].shader.locs[LOC_MAP_ALBEDO] = GetShaderLocation(body.materials[0].shader, "lightAccum");
        body.materials[0].shader.locs[LOC_MAP_EMISSION] = GetShaderLocation(body.materials[0].shader, "freckleMask");
        body.materials[0].shader.locs[LOC_MAP_ROUGHNESS] = GetShaderLocation(body.materials[0].shader, "lightBake");

        body.materials[0].maps[MAP_ALBEDO].texture = tex_accum.texture;
        body.materials[0].maps[MAP_EMISSION].texture = tex_freckles.texture;
        body.materials[0].maps[MAP_ROUGHNESS].texture = tex_lighting.texture;

        SetTextureFilter(body.materials[0].maps[MAP_ALBEDO].texture, FILTER_BILINEAR);
        SetTextureFilter(body.materials[0].maps[MAP_EMISSION].texture, FILTER_BILINEAR);
        SetTextureFilter(body.materials[0].maps[MAP_ROUGHNESS].texture, FILTER_BILINEAR);
    }

    lightCatcher.materials[0] = LoadMaterialDefault();

    {
        lightCatcher.materials[0].shader = LoadShader("unwrap.vert", "bake.frag");
        uniformLoc_LightPos[1] = GetShaderLocation(lightCatcher.materials[0].shader, "lightPos");
        {
            float size[2] = { (float)tex_lighting.texture.width, (float)tex_lighting.texture.height };
            SetShaderValue(lightCatcher.materials[0].shader, GetShaderLocation(lightCatcher.materials[0].shader, "size"), size, UNIFORM_VEC2);
        }
    }

    Shader shd_accum = LoadShader(0, "accumulate.frag");
    Shader shd_gray = LoadShader(0, "grayscale.frag");

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        // Move light source
        {
            float t = GetTime();
            light.x = cosf(t) * 16.0f;
            light.y = 0.0f;
            //light.y = sinf(t) * 16.0f;
            light.z = sinf(t) * 16.0f;
            float pos[3] = { light.x, light.y, light.z };
            SetShaderValue(body.materials[0].shader, uniformLoc_LightPos[0], pos, UNIFORM_VEC3);
            SetShaderValue(lightCatcher.materials[0].shader, uniformLoc_LightPos[1], pos, UNIFORM_VEC3);
        }

        UpdateCamera(&camera);

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginTextureMode(tex_lighting); {

            ClearBackground(BLACK);
            DrawModel(lightCatcher, Vector3Zero(), 1.0f, WHITE);

        } EndTextureMode();

        BeginTextureMode(tex_accum); {

            BeginBlendMode(BLEND_ADD_COLORS); {

                DrawTexturePro(tex_lighting.texture,
                               { 0.0f, 0.0f, (float)tex_lighting.texture.width, -(float)tex_lighting.texture.height },
                               { 0.0f, 0.0f, (float)tex_accum.texture.width, (float)tex_accum.texture.height },
                               { 0.0f, 0.0f },
                               0.0f, { 0, 1, 2, 0 });

            } EndBlendMode();

        } EndTextureMode();

        BeginTextureMode(tex_accum); {

            BeginShaderMode(shd_accum); {

                DrawTexturePro(tex_accum.texture,
                               { 0.0f, 0.0f, (float)tex_accum.texture.width, -(float)tex_accum.texture.height },
                               { 0.0f, 0.0f, (float)tex_accum.texture.width, (float)tex_accum.texture.height },
                               { 0.0f, 0.0f },
                               0.0f, WHITE);

            } EndShaderMode();

        } EndTextureMode();

        double x = DistributedRand();
        double y = DistributedRand();
        double r = DistributedRand();
        float freckleRadius = r * freckleRadiusScale + freckleRadiusMin;

        BeginTextureMode(tex_frecklesTemp); {

            BeginShaderMode(shd_gray); {

                DrawTexturePro(tex_accum.texture,
                               { 0.0f, 0.0f, (float)tex_accum.texture.width, -(float)tex_accum.texture.height },
                               { 0.0f, 0.0f, (float)tex_accum.texture.width, (float)tex_accum.texture.height },
                               { 0.0f, 0.0f },
                               0.0f, WHITE);

            } EndShaderMode();

            BeginBlendMode(BLEND_MULTIPLIED); {

                DrawTexturePro(tex_freckles.texture,
                               { 0.0f, 0.0f, (float)tex_freckles.texture.width, -(float)tex_freckles.texture.height },
                               { 0.0f, 0.0f, (float)tex_freckles.texture.width, (float)tex_freckles.texture.height },
                               { 0.0f, 0.0f },
                               0.0f, WHITE);

            } EndBlendMode();
            
        } EndTextureMode();

        BeginTextureMode(tex_freckles); {

            BeginBlendMode(BLEND_ADD_COLORS); {

                DrawTexturePro(tex_frecklesTemp.texture,
                               { 0.0f, 0.0f, (float)tex_freckles.texture.width, -(float)tex_freckles.texture.height },
                               { 0.0f, 0.0f, (float)tex_freckles.texture.width, (float)tex_freckles.texture.height },
                               { 0.0f, 0.0f },
                               0.0f, WHITE);

            } EndBlendMode();

            DrawCircleV({ (float)x * (float)tex_freckles.texture.width, (float)y * (float)tex_freckles.texture.height }, freckleRadius, { 255,255,255,3 });

        } EndTextureMode();

        BeginDrawing(); {

            ClearBackground(BLACK);

            BeginShaderMode(bloom); {

                BeginMode3D(camera); {

                    DrawModel(body, Vector3Zero(), 1.0f, WHITE);

                    DrawSphere(light, 0.2f, WHITE);

                } EndMode3D();

            } EndShaderMode();

#if 1
            DrawTexturePro(tex_accum.texture,
                           { 0.0f, 0.0f, (float)tex_accum.texture.width, -(float)tex_accum.texture.height },
                           { 0.0f, 0.0f, (float)windowWidth * 0.125f, (float)windowHeight * 0.125f },
                           { 0.0f, 0.0f },
                           0.0f, WHITE);
            DrawTexturePro(tex_freckles.texture,
                           { 0.0f, 0.0f, (float)tex_freckles.texture.width, -(float)tex_freckles.texture.height },
                           { (float)windowWidth * 0.125f, 0.0f, (float)windowWidth * 0.125f, (float)windowHeight * 0.125f },
                           { 0.0f, 0.0f },
                           0.0f, WHITE);
#endif
            DrawFPS(0, 0);

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    UnloadShader(shd_accum);
    UnloadShader(shd_gray);
    UnloadRenderTexture(tex_freckles);
    UnloadRenderTexture(tex_frecklesTemp);
    UnloadRenderTexture(tex_lighting);
    UnloadRenderTexture(tex_accum);
    UnloadShader(bloom);
    UnloadShader(body.materials[0].shader);
    UnloadShader(lightCatcher.materials[0].shader);
    UnloadModel(body);
    UnloadModel(lightCatcher);

    CloseWindow();

    return 0;
}
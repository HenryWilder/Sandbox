#include <time.h>
#include <stdlib.h>
#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>
#include <vector>

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

struct ModelLighting
{
    RenderTexture2D tex_freckles;
    RenderTexture2D tex_frecklesTemp;
    RenderTexture2D tex_lighting;
    RenderTexture2D tex_accum;
    Model body;
    Model lightBake;
};
std::vector<ModelLighting*> models;
void SetupModel(Model model, int texW, int texH)
{
    RenderTexture2D tex_freckles = LoadRenderTexture(texW, texH);
    BeginTextureMode(tex_freckles); {

        ClearBackground({ 0,0,0, 0 });

    } EndTextureMode();
    RenderTexture2D tex_frecklesTemp = LoadRenderTexture(texW, texH);

    RenderTexture2D tex_lighting = LoadRenderTexture(texW, texH);
    RenderTexture2D tex_accum = LoadRenderTexture(texW, texH);
    BeginTextureMode(tex_accum); {

        ClearBackground(BLACK);

    } EndTextureMode();

    {
        model.materials[0].shader = LoadShader("lighting.vert", "lighting.frag");

        model.materials[0].shader.locs[LOC_MAP_ALBEDO] = GetShaderLocation(model.materials[0].shader, "lightAccum");
        model.materials[0].shader.locs[LOC_MAP_EMISSION] = GetShaderLocation(model.materials[0].shader, "freckleMask");
        model.materials[0].shader.locs[LOC_MAP_ROUGHNESS] = GetShaderLocation(model.materials[0].shader, "lightBake");

        model.materials[0].maps[MAP_ALBEDO].texture = tex_accum.texture;
        model.materials[0].maps[MAP_EMISSION].texture = tex_freckles.texture;
        model.materials[0].maps[MAP_ROUGHNESS].texture = tex_lighting.texture;

        SetTextureFilter(model.materials[0].maps[MAP_ALBEDO].texture, FILTER_BILINEAR);
        SetTextureFilter(model.materials[0].maps[MAP_EMISSION].texture, FILTER_BILINEAR);
        SetTextureFilter(model.materials[0].maps[MAP_ROUGHNESS].texture, FILTER_BILINEAR);
    }

    Material mat_lightBake = LoadMaterialDefault();

    {
        mat_lightBake.shader = LoadShader("unwrap.vert", "bake.frag");
        model.materials[0] = GetShaderLocation(mat_lightBake.shader, "lightPos");
        {
            float size[2] = { (float)tex_lighting.texture.width, (float)tex_lighting.texture.height };
            int loc = GetShaderLocation(mat_lightBake.shader, "size");
            SetShaderValue(mat_lightBake.shader, loc, size, UNIFORM_VEC2);
        }
    }
}
void CleanupModel()
{

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

    Camera3D camera;
    camera.fovy = 45.0f;
    camera.position = { -3,0,0 };
    camera.target = { 0,0,0 };
    camera.up = { 0,1,0 };
    camera.type = CAMERA_PERSPECTIVE;
    SetCameraMode(camera, CAMERA_FREE);

    Vector3 light;

    Model bodies[2];
    Model lightCatchers[2];
    bodies[0] = LoadModel("torso.obj");
    lightCatchers[0] = LoadModel("torso.obj");
    {
        Mesh meshes[2];
        for (int i = 0; i < 2; ++i)
        {
            meshes[i] = GenMeshSphere(1.0f, 64, 64); // Both must be identical
        }
        bodies[1] = LoadModelFromMesh(meshes[0]);
        lightCatchers[1] = LoadModelFromMesh(meshes[1]);
    }

    Material mat_body = LoadMaterialDefault();
    int uniformLoc_LightPos;

    {
        mat_body.shader = LoadShader("lighting.vert", "lighting.frag");

        mat_body.shader.locs[LOC_MAP_ALBEDO] = GetShaderLocation(mat_body.shader, "lightAccum");
        mat_body.shader.locs[LOC_MAP_EMISSION] = GetShaderLocation(mat_body.shader, "freckleMask");
        mat_body.shader.locs[LOC_MAP_ROUGHNESS] = GetShaderLocation(mat_body.shader, "lightBake");
        
        mat_body.maps[MAP_ALBEDO].texture = tex_accum.texture;
        mat_body.maps[MAP_EMISSION].texture = tex_freckles.texture;
        mat_body.maps[MAP_ROUGHNESS].texture = tex_lighting.texture;

        SetTextureFilter(mat_body.maps[MAP_ALBEDO].texture, FILTER_BILINEAR);
        SetTextureFilter(mat_body.maps[MAP_EMISSION].texture, FILTER_BILINEAR);
        SetTextureFilter(mat_body.maps[MAP_ROUGHNESS].texture, FILTER_BILINEAR);
    }
    for (Model& body : bodies)
        body.materials[0] = mat_body;

    Material mat_lightBake = LoadMaterialDefault();

    {
        mat_lightBake.shader = LoadShader("unwrap.vert", "bake.frag");
        uniformLoc_LightPos = GetShaderLocation(mat_lightBake.shader, "lightPos");
        {
            float size[2] = { (float)tex_lighting.texture.width, (float)tex_lighting.texture.height };
            int loc = GetShaderLocation(mat_lightBake.shader, "size");
            SetShaderValue(mat_lightBake.shader, loc, size, UNIFORM_VEC2);
        }
    }
    for (Model& lc : lightCatchers)
        lc.materials[0] = mat_lightBake;

    Shader shd_accum = LoadShader(0, "accumulate.frag");
    Shader shd_gray = LoadShader(0, "grayscale.frag");

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        // Move light source
        {
            constexpr float distance = 16.0f; // 16.0f
            float t = GetTime();
            light.x = cosf(t) * distance;
            //light.y = 0.0f;
            light.y = sinf(t) * distance;
            light.z = sinf(t) * distance;
            float pos[3] = { light.x, light.y, light.z };
            SetShaderValue(mat_lightBake.shader, uniformLoc_LightPos, pos, UNIFORM_VEC3);
        }

        UpdateCamera(&camera);

        // Drawing

        // Bake the dynamic lighting for this frame
        BeginTextureMode(tex_lighting); {

            ClearBackground(BLACK);
            for (Model& lc : lightCatchers)
                DrawModel(lc, Vector3Zero(), 1.0f, WHITE);

        } EndTextureMode();
        // Blur & reduce accumulated light
        BeginTextureMode(tex_accum); {

            BeginShaderMode(shd_accum); {

                DrawTexturePro(tex_accum.texture,
                               { 0.0f, 0.0f, (float)tex_accum.texture.width, -(float)tex_accum.texture.height },
                               { 0.0f, 0.0f, (float)tex_accum.texture.width, (float)tex_accum.texture.height },
                               { 0.0f, 0.0f },
                               0.0f, GRAY);

            } EndShaderMode();

        } EndTextureMode();
        // Accumulate baked lighting
        BeginTextureMode(tex_accum); {

            BeginBlendMode(BLEND_ADD_COLORS); {

                DrawTexturePro(tex_lighting.texture,
                               { 0.0f, 0.0f, (float)tex_lighting.texture.width, -(float)tex_lighting.texture.height },
                               { 0.0f, 0.0f, (float)tex_accum.texture.width, (float)tex_accum.texture.height },
                               { 0.0f, 0.0f },
                               0.0f, { 0, 1, 2, 0 });

            } EndBlendMode();

        } EndTextureMode();

        // Blend freckles so that they strengthen in light and weaken in dark
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

        } EndTextureMode();

        // Spawn a freckle
        {
            double x = DistributedRand();
            double y = DistributedRand();
            double r = DistributedRand();
            float freckleRadius = r * freckleRadiusScale + freckleRadiusMin;
            BeginTextureMode(tex_freckles); {

                DrawCircleV({ (float)x * (float)tex_freckles.texture.width, (float)y * (float)tex_freckles.texture.height }, freckleRadius, { 255,255,255,3 });

            } EndTextureMode();
        }


        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            BeginShaderMode(bloom); {

                BeginMode3D(camera); {

                    DrawModel(body, Vector3Zero(), 1.0f, WHITE); // Draw the model

                    DrawSphere(light, 0.2f, WHITE); // Draw the light source

                } EndMode3D();

            } EndShaderMode();

#if 1 // Debug
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
#endif // !Debug

            DrawFPS(0,0);

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
    UnloadShader(mat_body.shader);
    UnloadShader(mat_lightBake.shader);
    UnloadModel(body);
    UnloadModel(lightCatcher);

    CloseWindow();

    return 0;
}
#include <raylib.h>
#include <raymath.h>
#include <string>

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

struct FragCode
{
    const char* uniforms;
    const char* postLighting;
};

Texture textures[1]; // 0 = freckles

int UpdateModelShader(Model& model, const FragCode& inserts)
{
    static const const char* vertShader =
R"txt(#version 330

        // Input vertex attributes
        in vec3 vertexPosition;
    in vec2 vertexTexCoord;
    in vec3 vertexNormal;
    in vec4 vertexColor;

    // Input uniform values
    uniform mat4 mvp;
    uniform mat4 matModel;

    // Output vertex attributes (to fragment shader)
    out vec3 fragPosition;
    out vec2 fragTexCoord;
    out vec4 fragColor;
    out vec3 fragNormal;
    out vec3 vertexPos;

    // NOTE: Add here your custom variables

    void main()
    {
        // Send vertex attributes to fragment shader
        fragPosition = vertexPosition;
        //fragPosition = vec3(matModel*vec4(vertexPosition, 1.0));
        fragTexCoord = vertexTexCoord;
        fragColor = vertexColor;
        vertexPos = vertexPosition;

        mat3 normalMatrix = transpose(inverse(mat3(matModel)));
        fragNormal = normalize(vertexNormal);
        //fragNormal = normalize(normalMatrix*vertexNormal);

        // Calculate final vertex position
        gl_Position = mvp * vec4(vertexPosition, 1.0);
    })txt";

    static const char* fragShaderIO = R"txt(
#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

// Input uniform heights
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
uniform vec2 size = vec2(2048,2048);  // render size (1024,2048)
uniform vec3 lightPos = vec3(-16.0,16.0,16.0);
const float shininess = 16.0;
const float lightPower = 150.0;
float fragBrightness = 0.0;)txt";
    static const char* fragShaderLighting = R"txt(

void main()
{
	vec3 lightDir = lightPos - fragPosition;
    float dist = length(lightDir);
    dist *= dist;
    lightDir = normalize(lightDir);
    
    float lambertian = max((1.0 + dot(lightDir, fragNormal)) * 0.5, 0.0);
    float specular = 0.0;
    float specAngle = 0.0;

    if (lambertian > 0.0)
    {
        vec3 viewDir = normalize(-fragPosition);
        
        if (true) {
            vec3 reflectDir = reflect(lightDir, fragNormal);
            specAngle = max(dot(reflectDir, viewDir), 0.0);
            // note that the exponent is different here
            specular = pow(specAngle, shininess/4.0);
        }
        else { // this is blinn phong
            vec3 halfDir = normalize(lightDir + viewDir);
            specAngle = max((dot(halfDir, fragNormal) + 1.0), 0.0);
            specular = pow(specAngle, shininess);
        }
    }

    float ambientBrightness = 0.2;
    float diffuseBrightness = lambertian * lightPower / dist;
    float specularBrightness = specular * lightPower / dist;)txt";
    
    std::string fragShader = fragShaderIO;
    fragShader += inserts.uniforms;
    fragShader += fragShaderLighting;
    fragShader += inserts.postLighting;
    fragShader += "\n}";

    const char* uniforms;
    UnloadShader(model.materials[0].shader);
    model.materials[0].shader = LoadShaderCode(vertShader, fragShader.c_str());
    int lightPosLoc = GetShaderLocation(model.materials[0].shader, "lightPos");

    int freckleMaskLoc = GetShaderLocation(model.materials[0].shader, "freckleMask");
    if (freckleMaskLoc)
    {
        model.materials[0].shader.locs[LOC_MAP_ALBEDO] = GetShaderLocation(model.materials[0].shader, "freckleMask");
        model.materials[0].maps[MAP_ALBEDO].texture = textures[0];
        SetTextureFilter(model.materials[0].maps[MAP_ALBEDO].texture, FILTER_BILINEAR);
    }

    return lightPosLoc;
}


struct CircleSlider
{
    bool beingDragged = false;
    float value;
    float radius;
    Vector2 center;

    constexpr float GetHandleRadius()
    {
        return 5.0f;
    }
    Vector2 GetHandleCenter()
    {
        return {
            center.x + cosf(value * 2.0f * PI) * radius,
            center.y + sinf(value * 2.0f * PI) * radius
        };
    }
    bool OnRail(Vector2 pt)
    {
        return abs(Vector2Distance(center, pt) - radius) < GetHandleRadius();
    }
    bool OnHandle(Vector2 pt)
    {
        return Vector2Distance(GetHandleCenter(), pt) < GetHandleRadius();
    }
    void Update(Vector2 pt)
    {
        value = Vector2Angle(center, pt) / 360.0f;
    }
    void DrawRail(Color color)
    {
        DrawCircleV(center, radius + 1.0f, color);
        DrawCircleV(center, radius - 1.0f, BLACK);
    }
    void DrawFacingArrow(float turns, Color color)
    {
        static float r2o2 = sqrtf(2.0f) / 2.0f;
        Vector2 a, b, c;

        a = { 1.0f, 0.0f };
        b = { -r2o2, r2o2 };
        c = { -r2o2, -r2o2 };

        a = Vector2Rotate(a, turns * 360.0f);
        b = Vector2Rotate(b, turns * 360.0f);
        c = Vector2Rotate(c, turns * 360.0f);

        a = center + (a * 20.0f);
        b = center + (b * 20.0f);
        c = center + (c * 20.0f);

        DrawTriangle(a, c, b, color);
    }
    void DrawBearingArrow(float turns, Color color)
    {
        static float r2o2 = sqrtf(2.0f) / 2.0f;
        Vector2 a, b, c;

        float cosX = cosf(turns * 2.0f * PI);

        a = { cosX, 0.0f };
        b = { -cosX, r2o2 };
        c = { -cosX, -r2o2 };

        a = center + (a * 20.0f);
        b = center + (b * 20.0f);
        c = center + (c * 20.0f);

        if (cosX > 0.0f)
            DrawTriangle(a, c, b, color);
        else {
            DrawTriangle(a, b, c, {
                (unsigned char)(255 - color.r),
                (unsigned char)(255 - color.g),
                (unsigned char)(255 - color.b),
                color.a });
        }
    }
    void DrawHandle(Color color)
    {
        DrawCircleV(GetHandleCenter(), GetHandleRadius(), color);
    }
    void DrawHandleAt(Vector2 pt, Color color)
    {
        DrawCircleV(center + (Vector2Normalize(pt - center) * radius), GetHandleRadius(), color);
    }
};

// TODO: Make slider for light distance & brightness

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    textures[0] = LoadTexture("freckles.png");

    FragCode shaders[] = {
        {   // Default

R"txt(
)txt",

R"txt(
finalColor = vec4(vec3(ambientBrightness + diffuseBrightness + specularBrightness), 1.0);
)txt"

        }, {   // Amity freckles

R"txt(
uniform sampler2D freckleMask;

const vec3 glow = vec3(121,23,207) / vec3(255);
const vec3 shine = vec3(32,142,249) / vec3(255);
const vec3 skin = (vec3(57,75,68) / vec3(255)) * 0.75;
const vec3 skinShine = vec3(81,106,96) / vec3(255);
const vec3 skinShade = (vec3(50,65,59) / vec3(255)) * 0.75;
)txt",

R"txt(    
    float freckle = texture(freckleMask, fragTexCoord).r;

    float fragBrightness = ambientBrightness + diffuseBrightness + specularBrightness;

    float shineStrength = clamp(pow(fragBrightness, 3.0) * 0.5, 0.0, 1.0);
    vec3 emissiveColor = mix(glow, shine, shineStrength);
    
    float brightness = (ambientBrightness + diffuseBrightness * 0.5 + specularBrightness * 0.3) * 0.15;

    float facing = dot(fragNormal, normalize(lightPos - fragPosition));
    vec3 diffuseColor = mix(skin, skinShade, clamp(facing, -0.125, 0.0) * -8.0);

    diffuseColor = mix(diffuseColor, skinShine, max(facing - 0.9, 0.0) / (1.0 - 0.9));

    if (facing > 0.99)
        diffuseColor += (facing - 0.99) * 20.0;

    finalColor = vec4(mix(diffuseColor, emissiveColor, freckle), 1.0);
)txt"

        }, {   // Amity hair

R"txt(
const vec3 glow = vec3(121,23,207) / vec3(255);
const vec3 shine = vec3(32,142,249) / vec3(255);
)txt",

R"txt(
    float fragBrightness = ambientBrightness + diffuseBrightness + specularBrightness;

    vec3 color = mix(glow, shine, clamp(((fragPosition.y + 1.0) * 0.5 * clamp(fragBrightness, 0.0, 1.0) * 1.5) - 0.25, 0.0, 1.0));

    float facing = dot(fragNormal, normalize(lightPos - fragPosition));
    if (facing > 0.995)
        color += vec3(0.1) * 0.5 * (1.0 + cos(fragTexCoord.y * radians(360.0 * 200.0)));

    finalColor = vec4(color, 1.0);
)txt"

        }, {   // Circuit hair

R"txt(
)txt",

R"txt(
finalColor = vec4(vec3(ambientBrightness + diffuseBrightness + specularBrightness), 1.0);
)txt"

        },
    };
    size_t shader_id = 0;

#if 1
    Model model = LoadModel("torso.obj");
#else
    Model model = LoadModelFromMesh(GenMeshSphere(1.0f, 64, 64));
#endif
    model.materials[0] = LoadMaterialDefault();
    int lightPosLoc = UpdateModelShader(model, shaders[shader_id]);

    Vector3 lightPos;
    float lightDist = 16.0f;

    Camera3D camera;
    camera.fovy = 45.0f;
    camera.position = { -3,0,0 };
    camera.target = { 0,0,0 };
    camera.up = { 0,1,0 };
    camera.type = CAMERA_PERSPECTIVE;
    SetCameraMode(camera, CAMERA_FREE);

    CircleSlider lightOrbitSlider_Y{ false, 0, 50, { 100, 100 } };
    CircleSlider lightOrbitSlider_Z{ false, 0, 50, { 250, 100 } };

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && lightOrbitSlider_Y.OnRail(GetMousePosition()))
            lightOrbitSlider_Y.beingDragged = true;
        else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            lightOrbitSlider_Y.beingDragged = false;

        if (lightOrbitSlider_Y.beingDragged)
            lightOrbitSlider_Y.Update(GetMousePosition());

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && lightOrbitSlider_Z.OnRail(GetMousePosition()))
            lightOrbitSlider_Z.beingDragged = true;
        else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            lightOrbitSlider_Z.beingDragged = false;

        if (lightOrbitSlider_Z.beingDragged)
            lightOrbitSlider_Z.Update(GetMousePosition());

        if (IsKeyPressed(KEY_ENTER))
        {
            shader_id = (shader_id + 1) % (sizeof(shaders) / sizeof(shaders[0]));
            lightPosLoc = UpdateModelShader(model, shaders[shader_id]);
        }

        float cosY = cosf(lightOrbitSlider_Y.value * 2.0f * PI);
        float sinY = sinf(lightOrbitSlider_Y.value * 2.0f * PI);
        float cosZ = cosf(-lightOrbitSlider_Z.value * 2.0f * PI);
        float sinZ = sinf(-lightOrbitSlider_Z.value * 2.0f * PI);

        lightOrbitSlider_Y.radius = 50.0f * abs(cosZ);


        lightPos.x = lightDist * 0.5f * (cosY * cosZ);
        lightPos.y = lightDist * 0.5f * (sinZ);
        lightPos.z = lightDist * 0.5f * (sinY * cosZ);
        {
            float pos[3] = { lightPos.x, lightPos.y, lightPos.z };
            SetShaderValue(model.materials[0].shader, lightPosLoc, pos, UNIFORM_VEC3);
        }

        UpdateCamera(&camera);

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            BeginMode3D(camera); {

                DrawModel(model, Vector3Zero(), 1.0f, WHITE);

                DrawSphere(lightPos, 0.2f, WHITE);

            } EndMode3D();

            lightOrbitSlider_Y.DrawRail(DARKGREEN);
            lightOrbitSlider_Y.DrawFacingArrow(0.0f, GRAY);
            lightOrbitSlider_Y.DrawHandle(lightOrbitSlider_Y.beingDragged ? GREEN : (lightOrbitSlider_Y.OnRail(GetMousePosition()) ? LIME : DARKGREEN));
            if (lightOrbitSlider_Y.OnRail(GetMousePosition()) && !lightOrbitSlider_Y.beingDragged && !lightOrbitSlider_Y.OnHandle(GetMousePosition()))
                lightOrbitSlider_Y.DrawHandleAt(GetMousePosition(), { 255,255,255,127 });

            lightOrbitSlider_Z.DrawRail(DARKBLUE);
            lightOrbitSlider_Z.DrawBearingArrow(-lightOrbitSlider_Y.value, LIGHTGRAY);
            lightOrbitSlider_Z.DrawHandle(lightOrbitSlider_Z.beingDragged ? SKYBLUE : (lightOrbitSlider_Z.OnRail(GetMousePosition()) ? BLUE : DARKBLUE));
            if (lightOrbitSlider_Z.OnRail(GetMousePosition()) && !lightOrbitSlider_Z.beingDragged && !lightOrbitSlider_Z.OnHandle(GetMousePosition()))
                lightOrbitSlider_Z.DrawHandleAt(GetMousePosition(), { 255,255,255,127 });

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    UnloadShader(model.materials[0].shader);

    UnloadModel(model);

    for (Texture& tex : textures)
    {
        UnloadTexture(tex);
    }

    CloseWindow();

	return 0;
}
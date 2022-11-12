#include <raylib.h>
#include <raymath.h>
#include <algorithm>
#include <time.h>
#include <string>
#include <regex>

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

constexpr Vector2 right{ 1.0f, 0.0f };
constexpr Vector2 left{ -1.0f, 0.0f };
constexpr Vector2 up{ 0.0f, -1.0f };
constexpr Vector2 down{ 0.0f, 1.0f };

#pragma endregion

float RandomFloatInRange(float min, float max)
{
    return ((float)rand() / (float)RAND_MAX) * (max - min) + min;
}

// Constants

constexpr int g_ballCount = 8;

constexpr float g_speedLimit = 10;

constexpr int windowSize = 738;

constexpr float minRadius = 20;
constexpr float maxRadius = 40;

// Hard
constexpr int hardBoundsMinX = -32;
constexpr int hardBoundsMinY = -32;
constexpr int hardBoundsWidth = 738 + 32;
constexpr int hardBoundsHeight = 413 + 32;
constexpr int hardBoundsMaxX = hardBoundsMinX + hardBoundsWidth;
constexpr int hardBoundsMaxY = hardBoundsMinY + hardBoundsHeight;

constexpr float hardBoundsMinXF = (float)hardBoundsMinX;
constexpr float hardBoundsMinYF = (float)hardBoundsMinY;
constexpr float hardBoundsMaxXF = (float)hardBoundsMaxX;
constexpr float hardBoundsMaxYF = (float)hardBoundsMaxY;

// Soft
constexpr int softBoundsPadding = 32;

constexpr int softBoundsMinX = hardBoundsMinX + softBoundsPadding;
constexpr int softBoundsMinY = hardBoundsMinY + softBoundsPadding;
constexpr int softBoundsMaxX = hardBoundsMaxX - softBoundsPadding;
constexpr int softBoundsMaxY = hardBoundsMaxY - softBoundsPadding;
constexpr int softBoundsWidth  = softBoundsMaxX - softBoundsMinX;
constexpr int softBoundsHeight = softBoundsMaxY - softBoundsMinY;

constexpr float softBoundsMinXF = (float)softBoundsMinX;
constexpr float softBoundsMinYF = (float)softBoundsMinY;
constexpr float softBoundsMaxXF = (float)softBoundsMaxX;
constexpr float softBoundsMaxYF = (float)softBoundsMaxY;

constexpr float G = 0.1f; // Gravitational constant

const std::string vertShader = R"TXT(
#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;

// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord;
out vec4 fragColor;

// NOTE: Add here your custom variables

void main()
{
    // Send vertex attributes to fragment shader
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;

    // Calculate final vertex position
    gl_Position = mvp*vec4(vertexPosition, 1.0);
}
)TXT";

std::string g_fragShader = R"TXT(
#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform heights
uniform sampler2D texture0;
uniform vec4 colDiffuse;

const int numBalls = )TXT"+std::to_string(g_ballCount)+R"TXT(;
uniform vec2 p[numBalls];
uniform float r[numBalls];
const vec3 colorEdge = vec3(129,21,206)/255;
const vec3 colorCore = vec3(29,25,40)/255;
const vec3 colorInner = vec3(38,144,252)/255;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
uniform vec2 resolution = vec2(720.0);

float Falloff(float distanceToPoint)
{
    return pow(1.0 / pow(distanceToPoint, 2), 2);
}

void main()
{
	float activity = 0.0;

    vec2 screenPos = fragTexCoord * resolution;
    vec2 boundsPos = (screenPos - vec2()TXT" + std::to_string(hardBoundsMinX) + "," + std::to_string(hardBoundsMinY) + R"TXT()) / vec2()TXT" + std::to_string(hardBoundsWidth) + "," + std::to_string(hardBoundsHeight) + R"TXT();
	for (int i = 0; i < numBalls; ++i)
	{
        float radius = r[i];
        float distance = distance(screenPos, p[i]);
        activity += pow(radius / pow(distance, 2), 2) * pow(radius, 2);
	}

    float amount = clamp(pow(activity - 1.0, 0.125), 0.0, 1.0);
    float height = boundsPos.y;
    vec3 outerColor = mix(colorInner, colorEdge, height);
    vec3 innerColor = mix(mix(1.0-colorCore, mix(colorInner, colorEdge, height), height), mix(mix(colorEdge, colorInner, height), colorCore, height), height);
    vec3 color = mix(outerColor, innerColor, pow(amount, 0.0675));
    finalColor = vec4(color, clamp(activity, 0.0, 1.0));
}
)TXT";



template<ShaderUniformDataType TYPE, int COUNT = 1>
struct ShaderUniform
{
    ShaderUniform(Shader shader, const char* uniformName) :
        shader(shader), loc(GetShaderLocation(shader, uniformName)) {}

    Shader shader;
    int loc;

    void operator=(void* value)
    {
        SetShaderValueV(shader, loc, value, TYPE, COUNT);
    }
};

template<ShaderUniformDataType TYPE>
struct ShaderUniform<TYPE, 1>
{
    ShaderUniform(Shader shader, const char* uniformName) :
        shader(shader), loc(GetShaderLocation(shader, uniformName)) {}

    Shader shader;
    int loc;

    void operator=(void* value)
    {
        SetShaderValue(shader, loc, value, TYPE);
    }
};

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;

    InitWindow(windowWidth, windowHeight, "Bloodgoop Metaballs");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    Shader metaballShader = LoadShaderCode(vertShader.c_str(), g_fragShader.c_str());
    RenderTexture2D screen = LoadRenderTexture(windowWidth, windowHeight);
    BeginTextureMode(screen);
    ClearBackground(WHITE);
    EndTextureMode();

    ShaderUniform<UNIFORM_VEC2, g_ballCount> metaPt(metaballShader, "p");
    ShaderUniform<UNIFORM_FLOAT, g_ballCount> metaRad(metaballShader, "r");
    ShaderUniform<UNIFORM_VEC2> metaRes(metaballShader, "resolution");

    Vector2 resolution = { 1280.0f, 720.0f };
    metaRes = &resolution;

    srand(time(nullptr));

    
    float radii[g_ballCount] = {};
    for (int i = 0; i < g_ballCount; ++i)
        radii[i] = RandomFloatInRange(minRadius, maxRadius);

    Vector2 points[g_ballCount] = {};
    for (int i = 0; i < g_ballCount; ++i)
        points[i] = { RandomFloatInRange(softBoundsMinX + radii[i], softBoundsMaxX - radii[i]), RandomFloatInRange(softBoundsMinY + radii[i], softBoundsMaxY - radii[i]) };

    constexpr float fourThirdsPi = (PI * 4.0f) / 3.0f;

    // Density is even for the material, so mass is dictaded by volume.
    // Specifically, density is roughly that of water, so about 1 mass unit = 1 volume unit.
    float masses[g_ballCount] = {};
    for (int i = 0; i < g_ballCount; ++i)
        masses[i] = fourThirdsPi * (radii[i] * radii[i] * radii[i]);
    
    Vector2 velocities[g_ballCount] = {};
    for (int i = 0; i < g_ballCount; ++i)
        velocities[i] = Vector2Normalize({ RandomFloatInRange(-1,1), RandomFloatInRange(-1,1) }) * RandomFloatInRange(0, 5);
    
    metaRad = radii;

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        float time = GetFrameTime();

        metaPt = points;

        for (int step = 0; step < 1; ++step)
        {
            // Collision
            {
                for (int i = 0; i < g_ballCount; ++i)
                {
                    for (int j = 0; j < g_ballCount; ++j)
                    {
                        // Don't apply gravity to self
                        if (j == i) continue;

                        float dist = Vector2Distance(points[j], points[i]);

                        // Skip to avoid division errors;
                        if (dist == 0.0f) continue;

                        Vector2 towards = Vector2Normalize(points[j] - points[i]);
                        float force = G * ((masses[j] * masses[i]) / (dist * dist));
                        float acceleration = force / masses[i];
                        float speed = acceleration * time;

                        // Distance in order to touch
                        float touchDistance = (radii[j] + radii[i]);

                        // Repel when overlapping, attract when not
                        velocities[i] += towards * (dist > touchDistance ? speed : -speed);
                    }
                }
            }

            // Bounds
            {
                // Soft boundary
                for (int i = 0; i < g_ballCount; ++i)
                {
                    float touchDistance = radii[i];

                    auto ApplyWallGrav = [&](float distanceToWall, Vector2 reflectDirection)
                    {
                        if (distanceToWall <= touchDistance)
                        {
                            // Walls push back with the same mass as the bodies
                            float force = G * ((masses[i] * masses[i]) / (distanceToWall * distanceToWall));
                            float acceleration = force / masses[i];
                            float speed = acceleration * time;
                            velocities[i] += reflectDirection * speed;
                        }
                    };

                    float distanceToL = points[i].x - softBoundsMinXF;
                    float distanceToR = softBoundsMaxXF - points[i].x;
                    float distanceToT = points[i].y - softBoundsMinYF;
                    float distanceToB = softBoundsMaxYF - points[i].y;

                    ApplyWallGrav(distanceToL, right);
                    ApplyWallGrav(distanceToR, left);
                    ApplyWallGrav(distanceToT, down);
                    ApplyWallGrav(distanceToB, up);
                }

                // Hard boundary
                for (int i = 0; i < g_ballCount; ++i)
                {
                    bool tooLeft  = points[i].x - radii[i] < hardBoundsMinX;
                    bool tooHigh  = points[i].y - radii[i] < hardBoundsMinY;
                    bool tooRight = points[i].x + radii[i] > hardBoundsMaxX;
                    bool tooLow   = points[i].y + radii[i] > hardBoundsMaxY;

                    if (tooLeft || tooRight) velocities[i].x *= -1;
                    if (tooHigh || tooLow)   velocities[i].y *= -1;

                    if (tooLeft)  points[i].x = hardBoundsMinX + radii[i];
                    if (tooHigh)  points[i].y = hardBoundsMinY + radii[i];
                    if (tooRight) points[i].x = hardBoundsMaxX - radii[i];
                    if (tooLow)   points[i].y = hardBoundsMaxY - radii[i];
                }
            }

            // Limit speeds and update positions
            {
                for (int i = 0; i < g_ballCount; ++i)
                {
                    if (Vector2Length(velocities[i]) > g_speedLimit)
                        velocities[i] = Vector2Normalize(velocities[i]) * g_speedLimit;

                    points[i] += velocities[i];
                }
            }
        }
        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(Color{ 0,0,0,255 });

            BeginShaderMode(metaballShader); {

                DrawTexture(screen.texture, 0, 0, WHITE);

            } EndShaderMode();

#if _DEBUG

            // Draw body collisions
            for (int i = 0; i < g_ballCount; ++i)
                DrawCircleLines(points[i].x, points[i].y, radii[i], MAGENTA);

            // Draw body interactions
            for (int i = 0; i < g_ballCount; ++i)
            {
                for (int j = 0; j < g_ballCount; ++j)
                {
                    if (j == i)
                        break;

                    float dist = Vector2Distance(points[j], points[i]);
                    float touchDist = radii[i] + radii[j];

                    DrawLine(points[i].x, points[i].y, points[j].x, points[j].y, (dist > touchDist ? GREEN : ORANGE));
                }
            }

            DrawRectangleLines(softBoundsMinX, softBoundsMinY, softBoundsWidth, softBoundsHeight, ORANGE);
            DrawRectangleLines(hardBoundsMinX, hardBoundsMinY, hardBoundsWidth, hardBoundsHeight, RED);

            DrawFPS(0,0);

#endif

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
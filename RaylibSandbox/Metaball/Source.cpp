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

namespace Window
{
    // Render area
    constexpr int windowWidth = 1280;
    constexpr int windowHeight = 720;

    // Hard bounds
    constexpr int hardBoundsMinX = -32;
    constexpr int hardBoundsMinY = -32;
    constexpr int hardBoundsWidth = 738 + 32;
    constexpr int hardBoundsHeight = 413 + 32;
    constexpr int hardBoundsMaxX = hardBoundsMinX + hardBoundsWidth;
    constexpr int hardBoundsMaxY = hardBoundsMinY + hardBoundsHeight;

    // Float
    constexpr float hardBoundsMinXF = (float)hardBoundsMinX;
    constexpr float hardBoundsMinYF = (float)hardBoundsMinY;
    constexpr float hardBoundsMaxXF = (float)hardBoundsMaxX;
    constexpr float hardBoundsMaxYF = (float)hardBoundsMaxY;

    // Soft bounds
    constexpr int softBoundsPadding = 32; // Inset

    constexpr int softBoundsMinX = hardBoundsMinX + softBoundsPadding;
    constexpr int softBoundsMinY = hardBoundsMinY + softBoundsPadding;
    constexpr int softBoundsMaxX = hardBoundsMaxX - softBoundsPadding;
    constexpr int softBoundsMaxY = hardBoundsMaxY - softBoundsPadding;
    constexpr int softBoundsWidth  = softBoundsMaxX - softBoundsMinX;
    constexpr int softBoundsHeight = softBoundsMaxY - softBoundsMinY;

    // Float
    constexpr float softBoundsMinXF = (float)softBoundsMinX;
    constexpr float softBoundsMinYF = (float)softBoundsMinY;
    constexpr float softBoundsMaxXF = (float)softBoundsMaxX;
    constexpr float softBoundsMaxYF = (float)softBoundsMaxY;
}

namespace Physics
{
    // Gravitational constant
    constexpr float G = 0.1f;
    // Speed of causality (universal speed limit)
    constexpr float c = 8.0f;
    // 4/3pi
    constexpr float fourThirdsPi = (PI * 4.0f) / 3.0f;
}

namespace Goop
{
    // Number of balls in the simulation
    constexpr int entityCount = 32;

    // Info for procedural generation
    constexpr float minRadius = 20.0f;
    constexpr float maxRadius = 60.0f;
    constexpr float minStartSpeed = 0.0f;
    constexpr float maxStartSpeed = 5.0f;

    // Physical properties of the fluid
    namespace Fluid
    {
        constexpr float density = 1.0f;
        constexpr float dragCoeff = 0.7f; // Drag coefficient
        constexpr float repulsion = 0.4f; // Multiplier for force when repelling
        constexpr float viscosity = 0.9f; // Multiplier for combining speeds when overlapping
        constexpr float touchPercent = 0.4f; // Percent of radii allowed to overlap before repelling
    }
}

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

const int numBalls = )TXT"+std::to_string(Goop::entityCount)+R"TXT(;
const vec2 boundsMin = vec2()TXT" + std::to_string(Window::hardBoundsMinX) + "," + std::to_string(Window::hardBoundsMinY) + R"TXT();
const vec2 boundsSize = vec2()TXT" + std::to_string(Window::hardBoundsWidth) + "," + std::to_string(Window::hardBoundsHeight) + R"TXT();
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
    vec2 boundsPos = (screenPos - boundsMin) / boundsSize;
	for (int i = 0; i < numBalls; ++i)
	{
        float radius = r[i];
        float distance = distance(screenPos, p[i]);
        activity += pow(radius / pow(distance, 2), 2) * pow(radius, 2);
	}

    float height = boundsPos.y;
    // Gradient glow/reflection
    vec3 outerColor = mix(colorInner, colorEdge, height);
    // Core
    vec3 innerColor = mix(mix(1.0-colorCore, mix(colorInner, colorEdge, height), height), mix(mix(colorEdge, colorInner, height), colorCore, height), height);
    // Transition from edge to core
    float coreTransition = clamp(pow(activity - 1.0, 0.125) * 0.7, 0.0, 1.0);

    vec3 color = mix(outerColor, innerColor, coreTransition);
    finalColor = vec4(vec3(color), 1.0);
    if (activity < 1.0)
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

float DragForce(Vector2 velocitySelf, Vector2 velocityOther, float overlapArea)
{
    constexpr float p = Goop::Fluid::density;
    constexpr float CD = Goop::Fluid::dragCoeff;
    float A = overlapArea;
    float v = Vector2Length(velocitySelf - velocityOther);
    float FD = 0.5 * p * (v*v) * CD * A;
    return FD;
}

float ForceOfGravity(float distance, float mass1, float mass2)
{
    return Physics::G * ((mass1 * mass2) / (distance * distance));
}

float ForceOfGravity(Vector2 position1, Vector2 position2, float mass1, float mass2)
{
    float distance = Vector2Distance(position1, position2);
    return ForceOfGravity(distance, mass1, mass2);
}

Vector2 ForceToVelocity(Vector2 direction, float force, float mass, float dt)
{
    float acceleration = force / mass;
    float speed = acceleration * dt;
    return direction * speed;
}

int main()
{
    InitWindow(Window::windowWidth, Window::windowHeight, "Bloodgoop Metaballs");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    Shader metaballShader = LoadShaderCode(vertShader.c_str(), g_fragShader.c_str());
    RenderTexture2D screen = LoadRenderTexture(Window::windowWidth, Window::windowHeight);
    BeginTextureMode(screen);
    ClearBackground(WHITE);
    EndTextureMode();

    ShaderUniform<UNIFORM_VEC2, Goop::entityCount> metaPt(metaballShader, "p");
    ShaderUniform<UNIFORM_FLOAT, Goop::entityCount> metaRad(metaballShader, "r");
    ShaderUniform<UNIFORM_VEC2> metaRes(metaballShader, "resolution");

    Vector2 resolution = { (float)Window::windowWidth, (float)Window::windowHeight };
    metaRes = &resolution;

    srand(time(nullptr));

    
    float radii[Goop::entityCount] = {};
    for (int i = 0; i < Goop::entityCount; ++i)
        radii[i] = RandomFloatInRange(Goop::minRadius, Goop::maxRadius);

    Vector2 points[Goop::entityCount] = {};
    for (int i = 0; i < Goop::entityCount; ++i)
        points[i] = { RandomFloatInRange(Window::softBoundsMinX + radii[i], Window::softBoundsMaxX - radii[i]), RandomFloatInRange(Window::softBoundsMinY + radii[i], Window::softBoundsMaxY - radii[i]) };

    // Density is even for the material, so mass is dictaded by volume.
    // Specifically, density is roughly that of water, so about 1 mass unit = 1 volume unit.
    float masses[Goop::entityCount] = {};
    for (int i = 0; i < Goop::entityCount; ++i)
        masses[i] = Physics::fourThirdsPi * (radii[i] * radii[i] * radii[i]);
    
    Vector2 velocities[Goop::entityCount] = {};
    for (int i = 0; i < Goop::entityCount; ++i)
        velocities[i] = Vector2Normalize({ RandomFloatInRange(-1,1), RandomFloatInRange(-1,1) }) * RandomFloatInRange(Goop::minStartSpeed, Goop::maxStartSpeed);
    
    metaRad = radii;

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        float dt = GetFrameTime();

        metaPt = points;

        for (int step = 0; step < 1; ++step)
        {
            // Collision
            {
                for (int i = 0; i < Goop::entityCount; ++i)
                {
                    for (int j = 0; j < Goop::entityCount; ++j)
                    {
                        // Don't apply gravity to self
                        if (j == i) continue;

                        float dist = Vector2Distance(points[j], points[i]);

                        // Skip to avoid division errors;
                        if (dist == 0.0f) continue;

                        Vector2 towards = Vector2Normalize(points[j] - points[i]);
                        float force = ForceOfGravity(dist, masses[j], masses[i]);
                        Vector2 gravVelocity = ForceToVelocity(towards, force, masses[i], dt);

                        // Distance in order to touch
                        float touchDistance = (radii[j] + radii[i]) * Goop::Fluid::touchPercent;

                        // Repel when overlapping, attract when not
                        velocities[i] += (dist > touchDistance ? gravVelocity : (gravVelocity * -Goop::Fluid::repulsion));
                    }
                }
            }

            // Friction
            {
                for (int i = 0; i < Goop::entityCount; ++i)
                {
                    for (int j = 0; j < Goop::entityCount; ++j)
                    {
                        // Don't apply gravity to self
                        if (j == i) continue;

                        float dist = Vector2Distance(points[j], points[i]);

                        // Skip to avoid division errors;
                        if (dist == 0.0f) continue;

                        // Distance in order to apply friction
                        float overlapDistance = (radii[j] + radii[i]);

                        // Distance in order to touch (and thus repel)
                        float touchDistance = overlapDistance * Goop::Fluid::touchPercent;

                        // Apply friction when overlapping
                        if (dist <= overlapDistance && dist > touchDistance)
                        {
                            float dragForce = DragForce(velocities[i], velocities[j], overlapDistance - dist);
                            Vector2 dragVelocity = ForceToVelocity(Vector2Normalize(velocities[i]) * -1.0f, dragForce, masses[i], dt);
                            velocities[i] += dragVelocity;
                        }
                    }
                }
            }

            // Bounds
            {
                // Soft boundary
                for (int i = 0; i < Goop::entityCount; ++i)
                {
                    float touchDistance = radii[i];

                    auto ApplyWallGrav = [&](float distanceToWall, Vector2 reflectDirection)
                    {
                        if (distanceToWall <= touchDistance)
                        {
                            // Walls push back with the same mass as the bodies
                            float force = ForceOfGravity(distanceToWall, masses[i], masses[i]);
                            velocities[i] += ForceToVelocity(reflectDirection, force, masses[i], dt);
                        }
                    };

                    float distanceToL = points[i].x - Window::softBoundsMinXF;
                    float distanceToR = Window::softBoundsMaxXF - points[i].x;
                    float distanceToT = points[i].y - Window::softBoundsMinYF;
                    float distanceToB = Window::softBoundsMaxYF - points[i].y;

                    ApplyWallGrav(distanceToL, right);
                    ApplyWallGrav(distanceToR, left);
                    ApplyWallGrav(distanceToT, down);
                    ApplyWallGrav(distanceToB, up);
                }

                // Hard boundary
                for (int i = 0; i < Goop::entityCount; ++i)
                {
                    bool tooLeft  = points[i].x - radii[i] < Window::hardBoundsMinX;
                    bool tooHigh  = points[i].y - radii[i] < Window::hardBoundsMinY;
                    bool tooRight = points[i].x + radii[i] > Window::hardBoundsMaxX;
                    bool tooLow   = points[i].y + radii[i] > Window::hardBoundsMaxY;

                    if (tooLeft || tooRight) velocities[i].x *= -1;
                    if (tooHigh || tooLow)   velocities[i].y *= -1;

                    if (tooLeft)  points[i].x = Window::hardBoundsMinX + radii[i];
                    if (tooHigh)  points[i].y = Window::hardBoundsMinY + radii[i];
                    if (tooRight) points[i].x = Window::hardBoundsMaxX - radii[i];
                    if (tooLow)   points[i].y = Window::hardBoundsMaxY - radii[i];
                }
            }

            // Limit speeds and update positions
            {
                for (int i = 0; i < Goop::entityCount; ++i)
                {
                    if (Vector2Length(velocities[i]) > Physics::c)
                        velocities[i] = Vector2Normalize(velocities[i]) * Physics::c;

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
            for (int i = 0; i < Goop::entityCount; ++i)
                DrawCircleLines(points[i].x, points[i].y, radii[i], MAGENTA);

            // Draw body interactions
            for (int i = 0; i < Goop::entityCount; ++i)
            {
                for (int j = 0; j < Goop::entityCount; ++j)
                {
                    if (j == i)
                        break;

                    float dist = Vector2Distance(points[j], points[i]);
                    float touchDist = radii[i] + radii[j];

                    DrawLine(points[i].x, points[i].y, points[j].x, points[j].y, (dist > touchDist ? GREEN : ORANGE));
                }
            }

            DrawRectangleLines(Window::softBoundsMinX, Window::softBoundsMinY, Window::softBoundsWidth, Window::softBoundsHeight, ORANGE);
            DrawRectangleLines(Window::hardBoundsMinX, Window::hardBoundsMinY, Window::hardBoundsWidth, Window::hardBoundsHeight, RED);

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
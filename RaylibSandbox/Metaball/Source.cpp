#include <raylib.h>
#include <raymath.h>
#include <algorithm>
#include <time.h>
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

constexpr int g_ballCount = 10;
constexpr float g_speedLimit = 10;

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

const std::string g_fragShader = R"TXT(
#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform heights
uniform sampler2D texture0;
uniform vec4 colDiffuse;

const int numBalls = )TXT" + std::to_string(g_ballCount) + R"TXT(;
uniform vec2 p[numBalls];
uniform float r[numBalls];
const vec3 colorEdge = vec3(129,21,206)/255;
const vec3 colorCore = vec3(29,25,40)/255;
const vec3 colorInner = vec3(38,144,252)/255;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
uniform vec2 resolution = vec2(720, 720);

void main()
{
	float activity = 0.0;
	for (int i = 0; i < numBalls; ++i)
	{
		activity += ((r[i] / resolution.x) / distance(fragTexCoord, p[i] / resolution));
	}

    float amount = clamp(pow(activity - 1.0, 0.125), 0.0, 1.0);
    float height = clamp(fragTexCoord.y, 0.0, 1.0);
    vec3 outerColor = mix(colorInner, colorEdge, height);
    vec3 innerColor = mix(mix(1.0-colorCore, mix(colorInner, colorEdge, height), height), mix(mix(colorEdge, colorInner, height), colorCore, height), height);
    vec3 color = mix(outerColor, innerColor, amount);
    if (activity >= 1.0) finalColor = vec4(color, 1.0);
    else discard;
}
)TXT";

int main()
{
    int windowWidth = 720;
    int windowHeight = 720;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
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

    int metaCursorLoc = GetShaderLocation(metaballShader, "cursor");
    int metaPtLoc = GetShaderLocation(metaballShader, "p");
    int metaRadLoc = GetShaderLocation(metaballShader, "r");

    srand(time(nullptr));

    Vector2 points[g_ballCount];
    for (int i = 0; i < g_ballCount; ++i)
        points[i] = { RandomFloatInRange(0,(float)windowWidth), RandomFloatInRange(0,(float)windowHeight) };
    
    float radius[g_ballCount];
    for (int i = 0; i < g_ballCount; ++i)
        radius[i] = RandomFloatInRange(100,300) / g_ballCount;
    
    Vector2 velocity[g_ballCount];
    for (int i = 0; i < g_ballCount; ++i)
        velocity[i] = Vector2Normalize({ RandomFloatInRange(-1,1), RandomFloatInRange(-1,1) }) * RandomFloatInRange(0, 5);
    
    SetShaderValueV(metaballShader, metaRadLoc, radius, UNIFORM_FLOAT, g_ballCount);

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        float data[2] = { (float)GetMouseX(), (float)GetMouseY() };

        SetShaderValueV(metaballShader, metaCursorLoc, data, UNIFORM_VEC2, g_ballCount);

        SetShaderValueV(metaballShader, metaPtLoc, points, UNIFORM_VEC2, g_ballCount);

        for (int step = 0; step < 1; ++step)
        {
            for (int i = 0; i < g_ballCount; ++i)
            {
                points[i] += velocity[i];

                bool tooLeft = points[i].x - radius[i] < 0;
                bool tooHigh = points[i].y - radius[i] < 0;
                bool tooRight = points[i].x + radius[i] > windowWidth;
                bool tooLow = points[i].y + radius[i] > windowHeight;

                if (tooLeft || tooRight) velocity[i].x *= -1;
                if (tooHigh || tooLow)   velocity[i].y *= -1;

                if (tooLeft)  points[i].x = radius[i];
                if (tooHigh)  points[i].y = radius[i];
                if (tooRight) points[i].x = windowWidth - radius[i];
                if (tooLow)   points[i].y = windowHeight - radius[i];

                if (Vector2Length(velocity[i]) > g_speedLimit) velocity[i] = Vector2Normalize(velocity[i]) * g_speedLimit;

                // Air resistance
                //velocity[i] *= 0.98f;
            }
            for (int i = 0; i < g_ballCount; ++i)
            {
                for (int j = 0; j < g_ballCount; ++j)
                {
                    if (j == i)
                        continue;

                    float dist = Vector2Distance(points[j], points[i]);

                    Vector2 towards = Vector2Normalize(points[j] - points[i]);
                    float newSpeed = ((radius[j] * radius[i] * 2.0f) / (dist * dist));

                    float touchDistance = (radius[j] + radius[i]); // Distance in order to touch

                    // Gravity
                    if (dist > touchDistance)
                        velocity[i] += towards * newSpeed;

                    // Repulsion
                    else if (dist > 0.0f)
                        velocity[i] += towards * -newSpeed * (radius[i] * 0.1f);
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
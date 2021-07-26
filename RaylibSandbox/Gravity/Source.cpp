#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

#define sign(x) (((x) > (decltype(x))(0)) - ((x) < (decltype(x))(0)))

#pragma region Vector math

inline Vector2  operator- (Vector2  vec) { return Vector2Negate(vec); }
inline Vector2  operator+ (Vector2  a, Vector2 b) { return      Vector2Add(a, b); }
inline Vector2& operator+=(Vector2& a, Vector2 b) { return (a = Vector2Add(a, b)); }
inline Vector2  operator+ (Vector2  a, float val) { return      Vector2AddValue(a, val); }
inline Vector2& operator+=(Vector2& a, float val) { return (a = Vector2AddValue(a, val)); }
inline Vector2  operator- (Vector2  a, Vector2 b) { return      Vector2Subtract(a, b); }
inline Vector2& operator-=(Vector2& a, Vector2 b) { return (a = Vector2Subtract(a, b)); }
inline Vector2  operator- (Vector2  a, float val) { return      Vector2SubtractValue(a, val); }
inline Vector2& operator-=(Vector2& a, float val) { return (a = Vector2SubtractValue(a, val)); }
inline Vector2  operator* (Vector2  a, Vector2 b) { return      Vector2Multiply(a, b); }
inline Vector2& operator*=(Vector2& a, Vector2 b) { return (a = Vector2Multiply(a, b)); }
inline Vector2  operator* (Vector2  a, float scl) { return      Vector2Scale(a, scl); }
inline Vector2& operator*=(Vector2& a, float scl) { return (a = Vector2Scale(a, scl)); }
inline Vector2  operator/ (Vector2  a, Vector2 b) { return      Vector2Divide(a, b); }
inline Vector2& operator/=(Vector2& a, Vector2 b) { return (a = Vector2Divide(a, b)); }
inline Vector2  operator/ (Vector2  a, float div) { return      Vector2Scale(a, 1.0f / div); }
inline Vector2& operator/=(Vector2& a, float div) { return (a = Vector2Scale(a, 1.0f / div)); }

inline Vector3  operator- (Vector3  vec) { return Vector3Negate(vec); }
inline Vector3  operator+ (Vector3  a, Vector3 b) { return      Vector3Add(a, b); }
inline Vector3& operator+=(Vector3& a, Vector3 b) { return (a = Vector3Add(a, b)); }
inline Vector3  operator+ (Vector3  a, float val) { return      Vector3AddValue(a, val); }
inline Vector3& operator+=(Vector3& a, float val) { return (a = Vector3AddValue(a, val)); }
inline Vector3  operator- (Vector3  a, Vector3 b) { return      Vector3Subtract(a, b); }
inline Vector3& operator-=(Vector3& a, Vector3 b) { return (a = Vector3Subtract(a, b)); }
inline Vector3  operator- (Vector3  a, float val) { return      Vector3SubtractValue(a, val); }
inline Vector3& operator-=(Vector3& a, float val) { return (a = Vector3SubtractValue(a, val)); }
inline Vector3  operator* (Vector3  a, Vector3 b) { return      Vector3Multiply(a, b); }
inline Vector3& operator*=(Vector3& a, Vector3 b) { return (a = Vector3Multiply(a, b)); }
inline Vector3  operator* (Vector3  a, float scl) { return      Vector3Scale(a, scl); }
inline Vector3& operator*=(Vector3& a, float scl) { return (a = Vector3Scale(a, scl)); }
inline Vector3  operator/ (Vector3  a, Vector3 b) { return      Vector3Divide(a, b); }
inline Vector3& operator/=(Vector3& a, Vector3 b) { return (a = Vector3Divide(a, b)); }
inline Vector3  operator/ (Vector3  a, float div) { return      Vector3Scale(a, 1.0f / div); }
inline Vector3& operator/=(Vector3& a, float div) { return (a = Vector3Scale(a, 1.0f / div)); }

#pragma endregion

Color ColorRand()
{
    return Color{ (unsigned char)(rand() % 256), (unsigned char)(rand() % 256), (unsigned char)(rand() % 256), (unsigned char)255};
}

struct RigidBody
{
    Vector2 x; // Position
    Vector2 v; // Velocity
    Vector2 F; // Force
    float m; // Mass
    float r; // Radius

    Color color = ColorRand();
};

const float G = 0.00000000006674f;

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "Physics simulator 2");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes
    ******************************************/

    RigidBody bodies[]
    {
        //{ { (windowWidth / 2), (windowWidth / 2) + windowHeight }, Vector2Zero(), Vector2Zero(), 10000.0f, windowWidth / 3 * 2 },

        { { (windowWidth / 2), windowHeight / 2}, Vector2Zero(), Vector2Zero(), 1024.0f, 32.0f },
        { { (windowWidth / 2) + 42.0f, 15.0f}, { 1.0f, 0.0f }, Vector2Zero(), 8.0f, 8.0f },
    };

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables
        ******************************************/

        float Dt = GetFrameTime();

        for (RigidBody& body : bodies)
        {
            body.F = Vector2Zero();
            for (RigidBody& other : bodies)
            {
                if (&other == &body) continue;
                
                float d = Vector2Distance(body.x, other.x);
                Vector2 normalizedDirection = Vector2Normalize(other.x - body.x);
                body.F += normalizedDirection * ((100.0f * body.m * other.m) / (d * d));

                if (Vector2Distance(body.x, other.x) <= (body.r + other.r))
                {
                    body.F *= -normalizedDirection;
                }
            }
            body.v += body.F * Dt;
            body.x += body.v;
        }

        /******************************************
        *   Draw the frame
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            for (RigidBody& body : bodies)
            {
                for (RigidBody& other : bodies)
                {
                    if (&other == &body) continue;
                    if (Vector2Distance(body.x, other.x) <= (body.r + other.r)) DrawCircleV(body.x, body.r, RED);
                    else DrawCircleV(body.x, body.r, body.color);
                }
            }

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory
    ******************************************/

    // TODO: Unload variables

    CloseWindow();

    return 0;
}
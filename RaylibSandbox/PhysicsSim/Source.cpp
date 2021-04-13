#include <vector>
#include <raylib.h>
#include <raymath.h>

#pragma region Vector math

inline Vector2  operator- (Vector2  vec) { return      Vector2Negate(vec); }
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

inline Vector3  operator- (Vector3  vec) { return      Vector3Negate(vec); }
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

template<typename Vec_t>
struct MassPoint
{
    MassPoint(Vector2 pos, float mass) : x(pos), v{ 0,0 }, F{ 0,0 }, m(mass) {}

	Vec_t x; // Position
	Vec_t v; // Velocity
	Vec_t F; // Force
	float m; // Mass
};
using MassPoint1D = MassPoint<float>;
using MassPoint2D = MassPoint<Vector2>;
using MassPoint3D = MassPoint<Vector3>;

template<typename Vec_t>
struct Spring
{
    Spring(MassPoint<Vec_t> *pt1, MassPoint<Vec_t> *pt2, float stiffness, float restLength, float damping) : a(pt1), b(pt2), ks(stiffness), L0(restLength), kd(damping) {}
    MassPoint<Vec_t> *a;
    MassPoint<Vec_t> *b;
    float ks; // Stiffness
    float L0; // Rest length
    float kd; // Damping factor
};
using Spring1D = Spring<float>;
using Spring2D = Spring<Vector2>;
using Spring3D = Spring<Vector3>;

float SpringForce(Spring2D *sp)
{
    float x; // Distance (?)
    x = (Vector2Distance(sp->b->x, sp->a->x) - sp->L0);
    float Fs; // Spring force
    Fs = sp->ks * x;
    return Fs;
}

void FrameUpdateMassPt2(MassPoint2D *pt, float Δt, Vector2 gravity, std::vector<Vector2> forces)
{
    pt->F = { 0, 0 };
    pt->F += (gravity * pt->m); // Gravity
    for (Vector2 force : forces)
    {
        pt->F += (force); // Forces
    }
    pt->v += ((pt->F * Δt) / pt->m); // Velocity
    pt->x += (pt->v * Δt); // Update position
}

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "Physics Sim");
    SetTargetFPS(30);

    /******************************************
    *   Load textures, shaders, and meshes
    ******************************************/

    // TODO: Load persistent assets & variables

    MassPoint2D points[]{ MassPoint2D({ 32.0f, 14.0f }, 1.0f), MassPoint2D({ 5.0f, 10.0f }, 1.0f) };
    Spring2D springs[]{ Spring2D(points + 0, points + 1, 0.0f, 1.0f, 0.5f) };

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables
        ******************************************/

        // TODO: simulate frame

        /******************************************
        *   Draw the frame
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            // TODO: Draw frame

            for (Spring2D& sp : springs)
            {
                DrawLineV(sp.a->x, sp.b->x, WHITE);
            }
            for (MassPoint2D& pt : points)
            {
                DrawCircleV(pt.x, 4.0f, RED);
            }

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory
    ******************************************/

    // TODO: Unload variables

    CloseWindow();
}
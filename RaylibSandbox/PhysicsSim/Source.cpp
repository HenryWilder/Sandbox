#include <vector>
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

template<typename Vec_t>
struct Math // Does not actually store anything; just a wrapper for our templates
{
    struct MassPoint
    {
        MassPoint(Vector2 pos, float mass) : x(pos), v(Vector2Zero()), F(Vector2Zero()), m(mass) {}

        Vec_t x; // Position
        Vec_t v; // Velocity
        Vec_t F; // Force
        float m; // Mass
    };

    struct SpringProperties
    {
        SpringProperties(float stiffness, float restLength, float damping) : ks(stiffness), L0(restLength), kd(damping) {}

        float ks; // Stiffness
        float L0; // Rest length
        float kd; // Damping factor
    };

    struct Spring
    {
        Spring(MassPoint* connection, SpringProperties& properties) : b(connection), p(properties) {}

        MassPoint* a;
        MassPoint* b;
        SpringProperties& p;
    };

    Vec_t NullVector(void);
    float VectorDistance(Vec_t, Vec_t);
    float VectorDotProduct(Vec_t, Vec_t);
    Vec_t VectorNormalize(Vec_t);

    float SpringForce(Spring* sp)
    {
        // Base force
        float x; // Distance (?)
        float Fs; // Spring force
        x = (VectorDistance(sp->b->x, sp->a->x) - sp->p.L0);
        Fs = sp->p.ks * x;

        // Damping
        Vec_t normalizedDirection;
        Vec_t velocityDifference;
        float Fd;
        normalizedDirection = VectorNormalize(sp->b->x - sp->a->x);
        velocityDifference = (sp->a->v - sp->b->v);
        Fd = VectorDotProduct(normalizedDirection, velocityDifference) * sp->p.kd;

        // Final
        float Ft; // Total force
        Ft = Fs + Fd;

        return Ft;
    }

    void FrameUpdateMassPt2(MassPoint* pt, float Δt, Vec_t gravity, Spring* sp)
    {
        pt->F = NullVector();
        pt->F += (gravity * pt->m); // Gravity
        pt->F += SpringForce(sp); // Forces
        pt->v += ((pt->F * Δt) / pt->m); // Velocity
        pt->x += (pt->v * Δt); // Update position
    }

    void Collision()
    {
        // todo
    }
};
using Math1D = Math<float>;
using Math2D = Math<Vector2>;
using Math3D = Math<Vector3>;

inline float   Math1D::NullVector() { return 0.0f; }
inline Vector2 Math2D::NullVector() { return Vector2Zero(); }
inline Vector3 Math3D::NullVector() { return Vector3Zero(); }

inline float Math1D::VectorDistance(float   pt1, float pt2)   { return abs(pt2 - pt1); }
inline float Math2D::VectorDistance(Vector2 pt1, Vector2 pt2) { return Vector2Distance(pt1, pt2); }
inline float Math3D::VectorDistance(Vector3 pt1, Vector3 pt2) { return Vector3Distance(pt1, pt2); }

inline float Math1D::VectorDotProduct(float   pt1, float pt2)   { return pt2 * pt1; }
inline float Math2D::VectorDotProduct(Vector2 pt1, Vector2 pt2) { return Vector2DotProduct(pt1, pt2); }
inline float Math3D::VectorDotProduct(Vector3 pt1, Vector3 pt2) { return Vector3DotProduct(pt1, pt2); }

inline float   Math1D::VectorNormalize(float   vec) { return (float)sign(vec); }
inline Vector2 Math2D::VectorNormalize(Vector2 vec) { return Vector2Normalize(vec); }
inline Vector3 Math3D::VectorNormalize(Vector3 vec) { return Vector3Normalize(vec); }


using MassPoint1D = Math1D::MassPoint;
using MassPoint2D = Math2D::MassPoint;
using MassPoint3D = Math3D::MassPoint;

using Spring1D = Math1D::Spring;
using Spring2D = Math2D::Spring;
using Spring3D = Math3D::Spring;

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
    Spring2D::Properties properties[]{ Spring2D::Properties(0.0f, 1.0f, 0.5f) };
    Spring2D springs[]{ Spring2D(points + 0, points + 1, properties[0]), Spring2D(points + 1, points + 0, properties[0]) };

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables
        ******************************************/

        // TODO: simulate frame
        for (size_t i = 0;)
        {

        }

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
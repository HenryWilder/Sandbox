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
    static Vec_t NullVector(void);
    static float VectorDistance(Vec_t, Vec_t);
    static float VectorDotProduct(Vec_t, Vec_t);
    static Vec_t VectorNormalize(Vec_t);
    // Returns the closest point on a line
    static Vec_t ClosestPoint(Vec_t edgeStart, Vec_t edgeEnd, Vec_t pt);

    struct Triangle
    {
        Vec_t pt1;
        Vec_t pt2;
        Vec_t pt3;
    };

    struct MassPoint
    {
        MassPoint(Vector2 pos, float mass, float bounce) : x(pos), v(NullVector()), F(NullVector()), Fc(NullVector()), m(mass), b(bounce) {}

        Vec_t x; // Position
        Vec_t v; // Velocity
        Vec_t F; // Force
        Vec_t Fc; // Cumulative spring force
        float m; // Mass
        float b; // Bounciness
    };

    struct Spring
    {
        Spring(MassPoint* _a, MassPoint* _b, float stiffness, float restLength, float damping) : a(_a), b(_b), ks(stiffness), L0(restLength), kd(damping) {}

        MassPoint* a;
        MassPoint* b;
        float ks; // Stiffness
        float L0; // Rest length
        float kd; // Damping factor
    };

    static Vec_t SpringForce(Spring* sp)
    {
        // Base force
        float x; // Distance (?)
        float Fc; // Spring force
        x = (VectorDistance(sp->b->x, sp->a->x) - sp->L0);
        Fc = sp->ks * x;

        // Damping
        Vec_t normalizedDirection;
        Vec_t velocityDifference;
        float Fd;
        normalizedDirection = VectorNormalize(sp->b->x - sp->a->x);
        velocityDifference = (sp->a->v - sp->b->v);
        Fd = VectorDotProduct(normalizedDirection, velocityDifference) * sp->kd;

        // Final
        float Ft; // Total force
        Ft = Fc + Fd;

        return normalizedDirection * Ft;
    }

    static void FrameUpdateMassPt(MassPoint* pt, float Δt, Vec_t gravity)
    {
        pt->F = NullVector();
        pt->F += (gravity * pt->m); // Gravity
        pt->F += pt->Fc; // Forces
        pt->Fc = NullVector(); // Clear spring force for recalculation next frame
        pt->v += ((pt->F * Δt) / pt->m); // Velocity
        pt->x += (pt->v * Δt); // Update position
    }

    static void UpdateSpringForce(Spring* sp)
    {
        Vec_t vec = SpringForce(sp);
        sp->a->Fc = vec;
        sp->b->Fc = -vec;
    }

    static void UpdateCollision(MassPoint* pt, const Triangle* tri);
};

using Vector1 = float;

using Math1D = Math<Vector1>;
using Math2D = Math<Vector2>;
using Math3D = Math<Vector3>;

inline Vector1 Math1D::NullVector() { return 0.0f; }
inline Vector2 Math2D::NullVector() { return Vector2Zero(); }
inline Vector3 Math3D::NullVector() { return Vector3Zero(); }

inline float   Math1D::VectorDistance(Vector1 pt1, Vector1 pt2) { return abs(pt2 - pt1); }
inline float   Math2D::VectorDistance(Vector2 pt1, Vector2 pt2) { return Vector2Distance(pt1, pt2); }
inline float   Math3D::VectorDistance(Vector3 pt1, Vector3 pt2) { return Vector3Distance(pt1, pt2); }

inline float   Math1D::VectorDotProduct(Vector1 pt1, Vector1 pt2) { return pt2 * pt1; }
inline float   Math2D::VectorDotProduct(Vector2 pt1, Vector2 pt2) { return Vector2DotProduct(pt1, pt2); }
inline float   Math3D::VectorDotProduct(Vector3 pt1, Vector3 pt2) { return Vector3DotProduct(pt1, pt2); }

inline Vector1 Math1D::VectorNormalize(Vector1 vec) { return (Vector1)sign(vec); }
inline Vector2 Math2D::VectorNormalize(Vector2 vec) { return Vector2Normalize(vec); }
inline Vector3 Math3D::VectorNormalize(Vector3 vec) { return Vector3Normalize(vec); }

Vector1 Math1D::ClosestPoint(Vector1 edgeStart, Vector1 edgeEnd, Vector1 pt)
{
    if (edgeStart == edgeEnd) return edgeStart;
    if (edgeStart > edgeEnd) // Swap if start is larger than end
    {
        *(int*)&edgeStart ^= *(int*)&edgeEnd;
        *(int*)&edgeEnd ^= *(int*)&edgeStart;
        *(int*)&edgeStart ^= *(int*)&edgeEnd;
    }
    return std::min(std::max(pt, edgeStart), edgeEnd);
}

Vector2 Math2D::ClosestPoint(Vector2 edgeStart, Vector2 edgeEnd, Vector2 pt)
{
    if (edgeStart.x == edgeEnd.x && edgeStart.y == edgeEnd.y) return edgeStart;
    if (edgeStart.x == edgeEnd.x) return { edgeStart.x, pt.y };
    if (edgeStart.y == edgeEnd.y) return { pt.x, edgeStart.y };
    float m1 = (edgeEnd.y - edgeStart.y) / (edgeEnd.x - edgeStart.x);
    float m2 = -1.0f / m1;
    float x = (m1 * edgeStart.x - m2 * pt.x + pt.y - edgeStart.y) / (m1 - m2);
    float y = m2 * (x - pt.x) + pt.y;
    return { x, y };
}
Vector3 Math3D::ClosestPoint(Vector3 edgeStart, Vector3 edgeEnd, Vector3 pt)
{
    _ASSERT_EXPR(false, "3D collision math incomplete!");
    // TODO
    return NullVector();
}

void Math2D::UpdateCollision(MassPoint* pt, const Triangle* tri)
{
    if (CheckCollisionPointTriangle(pt->x, tri->pt1, tri->pt2, tri->pt3))
    {
        Vector2 closest;
        float shortestDist;
        Vector2 temp;
        float tempDist;
        Vector2 normalPts[2];

        auto Edge = [&](Vector2 start, Vector2 end)
        {
            temp = ClosestPoint(start, end, pt->x);
            tempDist = Vector2Distance(pt->x, temp);
            if (tempDist < shortestDist)
            {
                closest = temp;
                shortestDist = tempDist;
                normalPts[0] = start;
                normalPts[1] = end;
            }
        };

        closest = ClosestPoint(tri->pt1, tri->pt2, pt->x);
        shortestDist = Vector2Distance(pt->x, closest);

        Edge(tri->pt2, tri->pt3);
        Edge(tri->pt1, tri->pt3);

        Vector2 normal = Vector2Normalize(normalPts[1] - normalPts[0]);
        normal = Vector2Rotate(normal, 90);
        Vector2 bounceVector = Vector2Reflect(closest - pt->x, normal);

        pt->Fc += bounceVector * pt->b;
        pt->x = closest;
    }
}

using MassPoint2D = Math2D::MassPoint;
using Spring2D = Math2D::Spring;
using Triangle2D = Math2D::Triangle;


struct RigidBody
{
    RigidBody(Vector2 pos, float mass, float bounce) : x(pos), v(Vector2Zero()), F(Vector2Zero()), Fc(Vector2Zero()), m(mass), b(bounce) {}

    Vector2 x; // Position
    Vector2 com; // Center of mass
    Vector2 v; // Velocity
    Vector2 F; // Force
    Vector2 Fc; // Cumulative spring force
    float m; // Mass
    float r; // Rotation
    float b; // Bounciness
};

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "Physics Sim");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes
    ******************************************/

    // TODO: Load persistent assets & variables

    //MassPoint2D points[]
    //{
    //    MassPoint2D({ 32.0f, 16.0f }, 1.0f, 0.2f),
    //    MassPoint2D({ 32.0f, 32.0f }, 1.0f, 0.2f),
    //    MassPoint2D({ 16.0f, 32.0f }, 1.0f, 0.2f),
    //    MassPoint2D({ 16.0f, 16.0f }, 1.0f, 0.2f),
    //};
    //Spring2D springs[]
    //{
    //    Spring2D(points + 0, points + 1, 0.9f, 3.0f, 0.5f),
    //    Spring2D(points + 1, points + 2, 0.9f, 3.0f, 0.5f),
    //    Spring2D(points + 2, points + 3, 0.9f, 3.0f, 0.5f),
    //    Spring2D(points + 3, points + 0, 0.9f, 3.0f, 0.5f),
    //
    //    Spring2D(points + 0, points + 2, 0.9f, 5.0f, 0.5f),
    //    Spring2D(points + 1, points + 3, 0.9f, 5.0f, 0.5f),
    //};
    Triangle2D world[]
    {
        Triangle2D{ { 3.0f, 54.0f }, { 64.0f, 54.0f }, { 3.0f, 32.0f } },
        Triangle2D{ { 48.0f, 72.0f }, { 64.0f, 75.0f }, { 66.0f, 32.0f } },
    };

    Vector2 gravity = { 0.0f, 9.8f };

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables
        ******************************************/

        // TODO: simulate frame
        float Δt = GetFrameTime();
        // Collisions
        for (MassPoint2D& pt : points)
        {
            for (Triangle2D& tri : world)
            {
                Math2D::UpdateCollision(&pt, &tri);
            }
        }
        // Spring force
        for (Spring2D& sp : springs)
        {
            Math2D::UpdateSpringForce(&sp);
        }
        // Point movement
        for (MassPoint2D& pt : points)
        {
            Math2D::FrameUpdateMassPt(&pt, Δt, gravity);
        }

        /******************************************
        *   Draw the frame
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            // TODO: Draw frame

            for (Triangle2D& tri : world)
            {
                DrawTriangle(tri.pt1, tri.pt2, tri.pt3, GRAY);
            }
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
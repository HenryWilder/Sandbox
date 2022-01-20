#include <raylib.h>
#include <raymath.h>
#include <vector>

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

struct MassPoint;

struct Spring
{
    // From scratch
    Spring(MassPoint* _a, MassPoint* _b, float _stiffness, float _restLength, float _dampingFactor) :
        a(_a), b(_b), stiffness(_stiffness), restLength(_restLength), dampingFactor(_dampingFactor) {}

    // Base
    Spring(float _stiffness, float _restLength, float _dampingFactor) :
        a(nullptr), b(nullptr), stiffness(_stiffness), restLength(_restLength), dampingFactor(_dampingFactor) {}

    // From base
    Spring(MassPoint* _a, MassPoint* _b, Spring* base) :
        a(_a), b(_b), stiffness(base->stiffness), restLength(base->restLength), dampingFactor(base->dampingFactor) {}

    MassPoint* a;
    MassPoint* b;
    float stiffness;
    float restLength;
    float dampingFactor;

    void SetCurrentStateAsRestLength();
    float SpringForce() const;
    void ApplySpringForce();
};

struct MassPoint
{
    // From scratch
    MassPoint(Vector2 _position, float _mass, float _selfCollisionRadius) :
        position(_position), velocity(Vector2Zero()), force(Vector2Zero()), mass(_mass), selfCollisionRadius(_selfCollisionRadius), springs{} {}

    // Base
    MassPoint(float _mass, float _selfCollisionRadius) :
        position(Vector2Zero()), velocity(Vector2Zero()), force(Vector2Zero()), mass(_mass), selfCollisionRadius(_selfCollisionRadius), springs{} {}

    // From Base
    MassPoint(Vector2 _position, MassPoint* base) :
        position(_position), velocity(Vector2Zero()), force(Vector2Zero()), mass(base->mass), selfCollisionRadius(base->selfCollisionRadius), springs{} {}

    Vector2 position;
    Vector2 velocity;
    Vector2 force;
    float mass;
    float selfCollisionRadius;
    std::vector<Spring*> springs;

    static constexpr Vector2 gravity = { 0.0f, 9.8f };
};

void Spring::SetCurrentStateAsRestLength()
{
    restLength = Vector2Distance(a->position, b->position);
}
float Spring::SpringForce() const
{
    float springForce = (Vector2Distance(a->position, b->position) - restLength) * stiffness;

    Vector2 direction = Vector2Normalize(b->position - a->position);
    Vector2 velocityDifference = b->velocity - a->velocity;
    float dampForce = Vector2DotProduct(direction, velocityDifference) * dampingFactor;

    float totalSpringForce = springForce + dampForce;
    return totalSpringForce;
}
void Spring::ApplySpringForce()
{
    float force = SpringForce();
    a->force += Vector2Normalize(b->position - a->position) * force;
    b->force += Vector2Normalize(a->position - b->position) * force;
}

struct Body_Base
{
    Rectangle boundingBox;

    virtual size_t GetPointsCount() const = 0;
    virtual Vector2 GetPoint(size_t index) const = 0;

    void UpdateBoundingBox()
    {
        Vector2 min = { FLT_MAX, FLT_MAX };
        Vector2 max = { -FLT_MAX, -FLT_MAX };
        for (size_t i = 0; i < GetPointsCount(); ++i)
        {
            Vector2 pt = GetPoint(i);
            if (pt.x < min.x) min.x = pt.x;
            if (pt.y < min.y) min.y = pt.y;
            if (pt.x > max.x) max.x = pt.x;
            if (pt.y > max.y) max.y = pt.y;
        }
        boundingBox.x = min.x;
        boundingBox.y = min.y;
        boundingBox.width = max.x - min.x;
        boundingBox.height = max.y - min.y;
    }
};
struct Rigidbody : public Body_Base
{
    Rigidbody(std::vector<Vector2> _points) : Body_Base(), points(_points) {}

    std::vector<Vector2> points;

    size_t GetPointsCount() const override
    {
        return points.size();
    }
    Vector2 GetPoint(size_t index) const override
    {
        return points[index % GetPointsCount()];
    }
};
bool CheckCollisionPointBody(Vector2 point, const Rigidbody& body, Vector2* closestPoint)
{
#define DEBUG_COLLISION_POINT_BODY 1

#if DEBUG_COLLISION_POINT_BODY
    DrawRectangleLinesEx(body.boundingBox, 1, RED);
#endif
    if (!CheckCollisionPointRec(point, body.boundingBox))
        return false;

    float distanceToSide = FLT_MAX;
    size_t linesIntersected = 0;
    for (size_t i = 0; i < body.GetPointsCount(); ++i)
    {
        Vector2 p0 = body.GetPoint(i);
        Vector2 p1 = body.GetPoint(i + 1); // Assumes GetPoint function wraps index
        Vector2 collisionPoint;
#if DEBUG_COLLISION_POINT_BODY
        DrawLineV({ point.x, body.boundingBox.y - 1.0f }, point, RED);
#endif
        if (CheckCollisionLines(p0, p1, { point.x, body.boundingBox.y - 1.0f }, point, &collisionPoint))
        {
#if DEBUG_COLLISION_POINT_BODY
            DrawLineV(point, collisionPoint, GREEN);
#endif
            ++linesIntersected;
        }

        Vector2 directionAB = Vector2Normalize(p1 - p0);
        Vector2 nearestPointOnSide = p0 + directionAB * std::min(std::max(Vector2DotProduct(directionAB, point - p0), 0.0f), Vector2Distance(p0, p1));
        float distance = Vector2Distance(point, nearestPointOnSide);
        if (distance < distanceToSide)
        {
            distanceToSide = distance;
            *closestPoint = nearestPointOnSide;
        }
#if DEBUG_COLLISION_POINT_BODY
        DrawCircleV(nearestPointOnSide, 2, BLUE);
#endif
    }
#if DEBUG_COLLISION_POINT_BODY
    DrawCircleV(*closestPoint, 2, GREEN);
#endif
    return linesIntersected & 1;

#undef DEBUG_COLLISION_POINT_BODY
}

struct Softbody : public Body_Base
{
    Softbody(std::vector<MassPoint> _points) : Body_Base(), points(_points) {}

    std::vector<MassPoint> points;

    size_t GetPointsCount() const override
    {
        return points.size();
    }
    Vector2 GetPoint(size_t index) const override
    {
        return points[index % GetPointsCount()].position;
    }
};

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    std::vector<Softbody> softBodies = {
        Softbody({
            MassPoint({ 30, 20 }, 1, 2),
            //MassPoint({ 40, 20 }, 1, 2),
            //MassPoint({ 40, 10 }, 1, 2),
            //MassPoint({ 30, 10 }, 1, 2),
            }),
    };
    std::vector<Rigidbody> rigidBodies{
        Rigidbody({
            { 10, 10 },
            { 40, 40 },
            { 10, 40 },
            }),
    };

    std::vector<Spring> springs;
    //{
    //    Spring springBase(0.5f, 5.0f, 0.5f);
    //
    //    springs = {
    //            Spring(&(softBodies[0].points[0]), &(softBodies[0].points[1]), &springBase),
    //            Spring(&(softBodies[0].points[0]), &(softBodies[0].points[2]), &springBase),
    //            Spring(&(softBodies[0].points[0]), &(softBodies[0].points[3]), &springBase),
    //
    //            Spring(&(softBodies[0].points[1]), &(softBodies[0].points[2]), &springBase),
    //            Spring(&(softBodies[0].points[1]), &(softBodies[0].points[3]), &springBase),
    //
    //            Spring(&(softBodies[0].points[2]), &(softBodies[0].points[3]), &springBase),
    //    };
    //}
    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        for (Rigidbody& body : rigidBodies)
        {
            body.UpdateBoundingBox();
        }

        for (Softbody& body : softBodies)
        {
            for (MassPoint& point : body.points)
            {
                point.force = { 0.0f, 0.0f };

                // Gravity
                point.force += (point.gravity * point.mass);

                // Soft Collision
                for (const Softbody& body2 : softBodies)
                {
                    for (const MassPoint& point2 : body.points)
                    {
                        float distance = Vector2Distance(point.position, point2.position);
                        float idealDistance = point.selfCollisionRadius + point2.selfCollisionRadius;

                        if (distance > 0.0f && distance < idealDistance)
                        {
                            Vector2 direction = Vector2Normalize(point2.position - point.position);
                            Vector2 newPosition = direction * idealDistance;
                            point.position = newPosition;
                            point.velocity = Vector2Reflect(point.velocity, direction);
                        }
                    }
                }

                // Rigid Collision
                for (const Rigidbody& body : rigidBodies)
                {
                    Vector2 newPosition;
                    if (CheckCollisionPointBody(point.position, body, &newPosition))
                    {
                        point.velocity = Vector2Reflect(point.velocity, Vector2Normalize(newPosition - point.position));
                        point.position = newPosition;
                    }
                }

                point.velocity += (point.force * GetFrameTime()) / point.mass;

                point.position += point.velocity * GetFrameTime();
            }
        }

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            for (const Rigidbody& body : rigidBodies)
            {
                for (size_t i = 0; i < body.GetPointsCount(); ++i)
                {
                    DrawLineV(body.GetPoint(i), body.GetPoint(i + 1), WHITE); // Assumes GetPoint function wraps index
                }
            }

            for (const Spring& spring : springs)
            {
                DrawLineV(spring.a->position, spring.b->position, RAYWHITE); // Assumes GetPoint function wraps index
            }

            for (const Softbody& body : softBodies)
            {
                for (size_t i = 0; i < body.GetPointsCount(); ++i)
                {
                    DrawCircleV(body.points[i].position, body.points[i].selfCollisionRadius, RED);
                }
            }

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    // @TODO: Unload variables

    CloseWindow();

	return 0;
}
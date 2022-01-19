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
    MassPoint* a;
    MassPoint* b;
    float stiffness;
    float restLength;
    float dampingFactor;

    float SpringForce() const;
    void ApplySpringForce();
};

struct MassPoint
{
    Vector2 position;
    Vector2 velocity;
    Vector2 force;
    float mass;

    float selfCollisionRadius;

    std::vector<Spring*> springs;

    static constexpr Vector2 gravity = { 0.0f, -9.8f };

    void Tick(float deltaTime)
    {
        force = { 0.0f, 0.0f };

        force += (gravity * mass);
        // TODO: Add other forces here

        velocity += (force * deltaTime) / mass;

        position += velocity * deltaTime;
    }
};

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
bool CheckCollisionPointBody(Vector2 point, Rigidbody body, Vector2* closestPoint)
{
    if (!CheckCollisionPointRec(point, body.boundingBox))
        return false;

    float distanceToSide = INFINITY;
    size_t linesIntersected = 0;
    for (size_t i = 0; i < body.GetPointsCount(); ++i)
    {
        Vector2 p0 = body.GetPoint(i);
        Vector2 p1 = body.GetPoint(i + 1); // Assumes GetPoint function wraps index
        Vector2 collisionPoint;
        linesIntersected += (size_t)(CheckCollisionLines({ point.x, body.boundingBox.y - 1.0f }, point, p0, p1, &collisionPoint));

        Vector2 directionAB = Vector2Normalize(p1 - p0);
        Vector2 nearestPointOnSide = p0 + directionAB * std::min(std::max(Vector2DotProduct(directionAB, point - p0), 0.0f), Vector2Distance(p0, p1));
        float distance = Vector2Distance(point, nearestPointOnSide);
        if (distance < distanceToSide)
        {
            distanceToSide = distance;
            *closestPoint = nearestPointOnSide;
        }
    }
    return linesIntersected & 1;
}

struct Softbody : public Body_Base
{
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

void UpdateCollision(std::vector<Rigidbody>* rigid, std::vector<Softbody>* soft)
{
    for (Rigidbody& body : *rigid)
    {
        body.UpdateBoundingBox();
    }

    for (Softbody& body : *soft)
    {
        for (MassPoint& point : body.points)
        {
            for (Rigidbody& body : *rigid)
            {
                Vector2 newPosition;
                if (CheckCollisionPointBody(point.position, body, &newPosition))
                {
                    point.velocity = Vector2Reflect(point.velocity, Vector2Normalize(newPosition - point.position));
                    point.position = newPosition;
                }
            }

            for (Softbody& body2 : *soft)
            {
                for (MassPoint& point2 : body.points)
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
        }
    }
}

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    std::vector<Softbody> softBodies;
    std::vector<Rigidbody> rigidBodies;

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        UpdateCollision(&rigidBodies, &softBodies);

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            for (Rigidbody body : rigidBodies)
            {
                for (size_t i = 0; i < body.GetPointsCount(); ++i)
                {
                    DrawLineV(body.GetPoint(i), body.GetPoint(i + 1), WHITE); // Assumes GetPoint function wraps index
                }
            }

            for (Softbody body : softBodies)
            {
                for (size_t i = 0; i < body.GetPointsCount(); ++i)
                {
                    DrawPixelV(body.GetPoint(i), RED);
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
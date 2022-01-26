#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <functional>

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

struct CollisionPoints
{
    Vector2 a;
    Vector2 b;
    Vector2 normal; // b - a normalized
    float depth;    // length of b - a
    bool hasCollision;
};

struct Transform2D
{
    Vector2 position;
    Vector2 scale;
    Quaternion rotation;
};

struct Collider;
struct SphereCollider;
struct PlaneCollider;

struct Collider
{
    virtual CollisionPoints TestCollision(
        const Transform2D* transform,
        const Collider* collider,
        const Transform2D* colliderTransform) const = 0;

    virtual CollisionPoints TestCollision(
        const Transform2D* transform,
        const SphereCollider* sphere,
        const Transform2D* sphereTransform) const = 0;

    virtual CollisionPoints TestCollision(
        const Transform2D* transform,
        const PlaneCollider* plane,
        const Transform2D* planeTransform) const = 0;
};

namespace algo
{
    CollisionPoints FindSphereSphereCollisionPoints(
        const SphereCollider* a, const Transform2D* ta,
        const SphereCollider* b, const Transform2D* tb);

    CollisionPoints FindSpherePlaneCollisionPoints(
        const SphereCollider* a, const Transform2D* ta,
        const PlaneCollider* b, const Transform2D* tb);

    CollisionPoints FindPlaneSphereCollisionPoints(
        const PlaneCollider* a, const Transform2D* ta,
        const SphereCollider* b, const Transform2D* tb);
}

struct SphereCollider : Collider
{
    Vector2 center;
    float radius;

    CollisionPoints TestCollision(
        const Transform2D* transform,
        const Collider* collider,
        const Transform2D* colliderTransform) const override
    {
        return collider->TestCollision(colliderTransform, this, transform);
    }

    CollisionPoints TestCollision(
        const Transform2D* transform,
        const SphereCollider* sphere,
        const Transform2D* sphereTransform) const override
    {
        return algo::FindSphereSphereCollisionPoints(
            this, transform, sphere, sphereTransform);
    }

    CollisionPoints TestCollision(
        const Transform2D* transform,
        const PlaneCollider* plane,
        const Transform2D* planeTransform) const override
    {
        return algo::FindSpherePlaneCollisionPoints(
            this, transform, plane, planeTransform);
    }
};
struct PlaneCollider : Collider
{
    Vector2 plane;
    float distance;

    CollisionPoints TestCollision(
        const Transform2D* transform,
        const Collider* collider,
        const Transform2D* colliderTransform) const override
    {
        return collider->TestCollision(colliderTransform, this, transform);
    }

    CollisionPoints TestCollision(
        const Transform2D* transform,
        const SphereCollider* sphere,
        const Transform2D* sphereTransform) const override
    {
        return algo::FindPlaneSphereCollisionPoints(
            this, transform, sphere, sphereTransform);
    }

    CollisionPoints TestCollision(
        const Transform2D* transform,
        const PlaneCollider* plane,
        const Transform2D* planeTransform) const override
    {
        return {}; // No plane v plane
    }
};

struct CollisionObject
{
protected:
    Collider* collider;
    Transform2D* transform;

    bool isDynamic;
    bool isTrigger;

    std::function<void(Collision&, float)> onCollision;

public:
    const Collider* Col() const
    {
        return collider;
    }
    Transform2D Trans() const
    {
        return *transform;
    }
    void SetPosition(Vector2 position)
    {
        transform->position = position;
    }
    bool IsDynamic() const
    {
        return isDynamic;
    }
    bool IsTrigger() const
    {
        return isTrigger;
    }

    const std::function<void(Collision&, float)> OnCollision() const
    {
        return onCollision;
    }
};
struct Rigidbody : CollisionObject
{
private:
    Vector2 velocity;
    Vector2 force;
    float mass;

    Vector2 m_gravity;
    bool m_takesGravity; // Whether to take gravity from the world

    float m_staticFriction;
    float m_dynamicFriction;
    float m_restitution;

public:
    Vector2 Velocity() const
    {
        return velocity;
    }
    void SetVelocity(Vector2 velocity)
    {
        this->velocity = velocity;
    }
    Vector2 Force() const
    {
        return force;
    }
    void SetForce(Vector2 force)
    {
        this->force = force;
    }
    void ApplyForce(Vector2 force)
    {
        this->force += force;
    }
    float Mass() const
    {
        return mass;
    }
    void SetMass(float mass)
    {
        this->mass = mass;
    }

    bool TakesGravity() const
    {
        return m_takesGravity;
    }
    Vector2 Gravity() const
    {
        return m_gravity;
    }
    void SetGravity(Vector2 gravity)
    {
        m_gravity = gravity;
    }
};

struct Collision
{
    Rigidbody* objA;
    Rigidbody* objB;
    CollisionPoints points;
};

class Solver
{
public:
    virtual void Solve(
        std::vector<Collision>& collisions,
        float dt) = 0;
};

class CollisionWorld {
protected:
    std::vector<CollisionObject*> m_objects;
    std::vector<Solver*> m_solvers;

    std::function<void(Collision&, float)> m_onCollision;

public:
    void AddCollisionObject   (CollisionObject* object)
    {
        m_objects.push_back(object);
    }
    void RemoveCollisionObject(CollisionObject* object)
    {
        if (!object) return;
        auto it = std::find(m_objects.begin(), m_objects.end(), object);
        if (it == m_objects.end()) return;
        m_objects.erase(it);
    }

    void AddSolver(Solver* solver)
    {
        m_solvers.push_back(solver);
    }
    void RemoveSolver(Solver* solver)
    {
        if (!solver) return;
        auto it = std::find(m_solvers.begin(), m_solvers.end(), solver);
        if (it == m_solvers.end()) return;
        m_solvers.erase(it);
    }

    void SetCollisionCallback(std::function<void(Collision&, float)>& callback)
    {
        m_onCollision = callback;
    }

    void SolveCollisions(
        std::vector<Collision>& collisions,
        float dt)
    {
        for (Solver* solver : m_solvers)
        {
            solver->Solve(collisions, dt);
        }
    }

    void SendCollisionCallbacks(
        std::vector<Collision>& collisions,
        float dt)
    {
        for (Collision& collision : collisions)
        {
            m_onCollision(collision, dt);

            auto& a = collision.objA->OnCollision();
            auto& b = collision.objB->OnCollision();

            if (a) a(collision, dt);
            if (b) b(collision, dt);
        }
    }

    void ResolveCollisions(float dt)
    {
        std::vector<Collision> collisions;
        std::vector<Collision> triggers;
        for (CollisionObject* a : m_objects)
        {
            for (CollisionObject* b : m_objects)
            {
                if (a == b) break; // Unique pairs

                if (!a->Col() || !b->Col()) continue; // Both have colliders

                CollisionPoints points = a->Col()->TestCollision(a->Trans(), b->Col(), b->Trans());

                if (points.hasCollision)
                {
                    bool trigger = a->IsTrigger() || b->IsTrigger();
                    
                    if (trigger) triggers  .emplace_back(a, b, points);
                    else         collisions.emplace_back(a, b, points);
                }
            }
        }

        SolveCollisions(collisions, dt); // Don't solve triggers

        SendCollisionCallbacks(collisions, dt);
        SendCollisionCallbacks(triggers, dt);
    }
};

class DynamicsWorld : CollisionWorld {
private:
    Vector2 m_gravity = Vector2{ 0.0f, 9.81f };

public:
    void AddRigidbody(Rigidbody* rigidbody)
    {
        if (rigidbody->TakesGravity())
            rigidbody->SetGravity(m_gravity);

        AddCollisionObject(rigidbody);
    }

    void ApplyGravity()
    {
        for (CollisionObject* object : m_objects)
        {
            if (!object->IsDynamic()) continue;

            Rigidbody* rigidbody = (Rigidbody*)object;
            rigidbody->ApplyForce(rigidbody->Gravity() * rigidbody->Mass());
        }
    }

    void MoveObjects(float dt)
    {
        for (CollisionObject* object : m_objects)
        {
            if (!object->IsDynamic()) continue;

            Rigidbody* rigidbody = (Rigidbody*)object;

            Vector2 vel = rigidbody->Velocity()
                        + rigidbody->Force() / rigidbody->Mass()
                        * dt;

            Vector2 pos = rigidbody->Trans().position
                        + rigidbody->Velocity()
                        * dt;

            rigidbody->SetVelocity(vel);
            rigidbody->SetPosition(pos);

            rigidbody->SetForce(Vector2Zero());
        }
    }

    void Step(float dt)
    {
        ApplyGravity();
        ResolveCollisions(dt);
        MoveObjects(dt);
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


    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/


        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);


        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    // @TODO: Unload variables

    CloseWindow();

	return 0;
}
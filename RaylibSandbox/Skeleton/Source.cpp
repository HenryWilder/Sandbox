#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <string>
#include "hankorithms.h"

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

class Bone
{
private:
    std::string m_name;
    Vector2 m_offset;
    Vector2 m_direction;
    float m_length;
    Bone* m_parent;
    std::vector<Bone*> m_children;

public:
    Bone(const std::string& name,
        Vector2 offset,
        Vector2 direction,
        float length,
        Bone* parent = nullptr) :
        m_name(name),
        m_offset(offset),
        m_direction(direction),
        m_length(length),
        m_parent(parent),
        m_children{}
    {}

    Bone(const Bone& src) = default;

    const std::string& Name() const { return m_name; }
    Vector2 Offset() const { return m_offset; }
    Vector2 StartPoint() const { return (m_parent ? m_parent->EndPoint() + m_offset : m_offset); }
    Vector2 EndPoint() const { return StartPoint() + m_direction * m_length; }
    Vector2 Direction() const { return m_direction; }
    Vector2 WorldDirection() const { return (m_parent ? m_parent->WorldDirection() + m_direction : m_direction); }
    float Length() const { return m_length; }
    Bone* Parent() const { return m_parent; }
    size_t ChildCount() const { return m_children.size(); }
    Bone* Child(size_t i) const { return m_children[i]; }
    const std::vector<Bone*>& Children() const { return m_children; }


    void SetName(const std::string& name) { m_name = name; }
    void SetOffset(Vector2 offset) { m_offset = offset; }
    void SetDirection(Vector2 direction) { m_direction = direction; }
    void SetLength(float length) { m_length = length; }
    void RemoveParent() { m_parent = nullptr; }
    void SetParent(Bone* parent) { m_parent = parent; }

    void AddChild(Bone* child)
    {
        if (child == nullptr)
            return;

        if (std::find(m_children.begin(), m_children.end(), child) == m_children.end())
            m_children.push_back(child);
    }
    void RemoveChild(Bone* child)
    {
        if (child == nullptr)
            return;

        std::vector<Bone*>::const_iterator it = std::find(m_children.begin(), m_children.end(), child);
        if (it != m_children.end())
            m_children.erase(it);
    }
    void AddChildren(const std::vector<Bone*>& children)
    {
        hw::multi_push(m_children, children);
    }
    void RemoveChildren(const std::unordered_set<Bone*>& children)
    {
        hw::multi_erase(m_children, children);
    }
    void RemoveChildren(const std::vector<Bone*>& children)
    {
        hw::multi_erase(m_children, children);
    }
};

// Uses DFS
template<class _Pr>
void TraverseSkeleton(Bone* root, _Pr pred)
{
    pred(root);
    for (Bone* bone : root->Children())
    {
        TraverseSkeleton<_Pr>(bone, pred);
    }
}
// Uses DFS
template<class _Pr>
Bone* SearchSkeleton(Bone* root, _Pr pred)
{
    if (pred(root))
        return root;

    for (Bone* bone : root->Children())
    {
        if (Bone* found = SearchSkeleton<_Pr>(bone, pred); found)
            return found;
    }
    return nullptr;
}

Bone* FindBoneEndingNearPosition(Bone* root, Vector2 position, float radius = 0.001f)
{
    return SearchSkeleton(root, [position, radius](Bone* root) { return Vector2Distance(root->EndPoint(), position) <= radius; });
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

    std::vector<Bone*> skeletons;
    Bone* selected = nullptr;

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        Bone* hovered;
        for (Bone* root : skeletons)
        {
            hovered = FindBoneEndingNearPosition(root, GetMousePosition(), 2.0f);
            if (!!hovered)
                break;
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            selected = new Bone("bone", Vector2Zero(), Vector2Zero(), 0.0f, hovered);
            if (!hovered)
                selected->SetOffset(GetMousePosition());
            skeletons.push_back(selected);
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && selected)
        {
            selected->SetLength(Vector2Distance(selected->StartPoint(), GetMousePosition()));
            selected->SetDirection();
        }

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            // TODO: Draw frame

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    // @TODO: Unload variables

    CloseWindow();

	return 0;
}
#include <vector>
#include <raylib.h>
#include <raymath.h>

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

struct ItemBase
{
    uint8_t width;
    uint8_t height;
    uint8_t depth; // Controls stack size
    const char* name;
    const char* description;
    Texture2D icon;
};

constexpr ItemBase g_pureItems[] =
{
    ItemBase{ 1, 1, 1, "Nut", "[@todo]" },
    ItemBase{ 1, 1, 1, "Bolt", "[@todo]" },
    ItemBase{ 1, 1, 1, "Screw", "[@todo]" },
    ItemBase{ 1, 1, 1, "Rivet", "[@todo]" },
    ItemBase{ 8, 8, 1, "Plate", "[@todo]" },
    ItemBase{ 8, 2, 2, "Pole", "[@todo]" },
    ItemBase{ 10, 4, 4, "Beam", "[@todo]" },
};

enum class ItemName : uint8_t
{
    Nut,
    Bolt,
    Screw,
    Rivet,
    Plate,
    Pole,
    Beam,
};

class ItemID
{
private:
    uint8_t m_id;

public:
    ItemID(uint8_t id) : m_id(id) { _ASSERT_EXPR(m_id < _countof(g_pureItems), "Invalid item ID"); }
    ItemID(ItemName name) : m_id((uint8_t)name) { _ASSERT_EXPR(m_id < _countof(g_pureItems), "Invalid item ID"); }
    ItemID(const ItemID& base) : m_id(base.m_id) {}

    const ItemBase& operator*() const { return g_pureItems[m_id]; }
    const ItemBase* operator->() const { return &g_pureItems[m_id]; }
};

class ItemSlot
{
private:
    ItemID m_item;
    uint16_t m_count;
    uint8_t m_x;
    uint8_t m_y;
    bool b_rotated;

public:
    ItemSlot(const ItemID& item, uint16_t count, uint8_t x, uint8_t y, bool rotated) :
        m_item(item), m_count(count), m_x(x), m_y(y), b_rotated(rotated) {}

    ItemSlot(const ItemName& name, uint16_t count, uint8_t x, uint8_t y, bool rotated) :
        m_item(name), m_count(count), m_x(x), m_y(y), b_rotated(rotated) {}

    ItemID Item() const
    {
        return m_item;
    }

    uint8_t X() const
    {
        return m_x;
    }
    void SetX(uint8_t x)
    {
        m_x = x;
    }

    uint8_t Y() const
    {
        return m_y;
    }
    void SetY(uint8_t y)
    {
        m_y = y;
    }

    void Add()
    {
        m_count++;
    }
    // Returns true if empty
    bool Subtract()
    {
        m_count--;
        return !m_count;
    }
    uint16_t Count()
    {
        return m_count;
    }
    
    void Rodate()
    {
        b_rotated = !b_rotated;
    }
    void SetRotation(bool rotation)
    {
        b_rotated = rotation;
    }
    bool Rotation()
    {
        return b_rotated;
    }
};

int main()
{
    constexpr int windowWidth = 1280;
    constexpr int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    constexpr int gridSize = 8;
    constexpr int gridWidth = 64;
    constexpr int gridHeight = 48;
    constexpr int invWidth = gridWidth * gridSize;
    constexpr int invHeight = gridHeight * gridSize;
    constexpr int invX = (windowWidth - invWidth) / 2;
    constexpr int invY = (windowHeight - invHeight) / 2;

    std::vector<ItemSlot*> slots;

    slots.push_back(new ItemSlot(ItemName::Plate, 1, 0, 0, false));

    ItemSlot*;

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        for (ItemSlot* slot : slots)
        {
            int start_x = invX + slot->X() * gridSize;
            int start_y = invY + slot->Y() * gridSize;
            int end_x = start_x + slot->Item()->width * gridSize;
            int end_y = start_y + slot->Item()->width * gridSize;
            if ();

        }

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            DrawRectangle(invX, invY, invWidth, invHeight, WHITE);
            for (int x = 0; x < gridWidth; ++x)
            {
                int _x = invX + x * gridSize + 1;
                DrawLine(_x, invY, _x, invY + invHeight, RAYWHITE);
            }
            for (int y = 0; y < gridHeight; ++y)
            {
                int _y = invY + y * gridSize;
                DrawLine(invX, _y, invX + invWidth, _y, RAYWHITE);
            }

            for (ItemSlot* slot : slots)
            {
                int x = invX + slot->X() * gridSize;
                int y = invY + slot->Y() * gridSize;
                DrawRectangle(x, y, slot->Item()->width * gridSize, slot->Item()->height * gridSize, LIGHTGRAY);
                DrawRectangleLines(x, y, slot->Item()->width * gridSize, slot->Item()->height * gridSize, GRAY);
                DrawText(TextFormat("%i", slot->Count()), x + 4, y + 4, 8, GRAY);
            }

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    for (ItemSlot* slot : slots)
    {
        delete slot;
    }

    CloseWindow();

	return 0;
}
#include <raylib.h>
#include <raymath.h>
#include <fstream>
#include <vector>
#include <deque>

template<typename F, size_t... S>
constexpr void static_for(F&& function, std::index_sequence<S...>) {
    int unpack[] =
    {
        0,
        void(function(std::integral_constant<std::size_t, S>{})), 0)...
    };

    (void)unpack;
}

template<std::size_t iterations, typename F>
constexpr void static_for(F&& function) {
    static_for(std::forward<F>(function), std::make_index_sequence<iterations>());
}

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

constexpr const char* g_IconFilenames[]
{
    "missing.png",
};
constexpr size_t g_IconCount = sizeof(g_IconFilenames) / sizeof(const char*);
Texture2D g_ItemIcons[g_IconCount];
void LoadIcons()
{
    template<>
    for (size_t i = 0; i < g_IconCount; ++i)
    {
        g_ItemIcons[i] = LoadTexture(g_IconFilenames[i]);
    }
}
void UnloadIcons()
{
    for (const Texture2D& icon : g_ItemIcons)
    {
        UnloadTexture(icon);
    }
}
// Iterator to textures in g_ItemIcons
class IconIter
{
private:
    uint16_t m_Index;

public:
    constexpr IconIter(uint16_t i) : m_Index(i) {}
    constexpr IconIter(const Texture2D& texture) : m_Index((uint32_t)(&texture - g_ItemIcons)) {}

    const Texture2D& operator*()  { return g_ItemIcons[m_Index]; }
    const Texture2D* operator->() { return &g_ItemIcons[m_Index]; }
};
// An abstraction of an item - the source version of which all others are clones of
struct Item
{
    constexpr Item(const char* displayName, uint16_t value, float weight, IconIter icon)
        : m_DisplayName(displayName), m_Weight(weight), m_Value(value), m_Icon(icon) {}

    const char* const m_DisplayName;
    const float m_Weight;
    const uint16_t m_Value;
    const IconIter m_Icon;
};
// List of all *possible* items
constexpr Item g_ItemList[]
{
    Item("null", 0ui16, 0.0f, IconIter(0ui16)),
    Item("Foo", 3ui16, 1.5f, IconIter(0ui16)),
    Item("Bar", 3ui16, 1.5f, IconIter(0ui16)),
};
constexpr uint16_t ItemCHash(const char* name)
{
    switch ((uint64_t)*name)
    {
    case (uint64_t)*"null": return 0ui16;
    case (uint64_t)*"Foo":  return 1ui16;
    case (uint64_t)*"Bar":  return 2ui16;
    default: return UINT16_MAX;
    }
}
// Reference/pointer to a base itemclass in the ItemList global
// Has overloads for dereference operators and acts like an iterator
class ItemBase
{
private:
    const uint16_t m_Index;

public:
    constexpr ItemBase()
        : m_Index(0ui16) {}
    constexpr ItemBase(uint16_t i)
        : m_Index(i) {}
    constexpr ItemBase(const Item& item)
        : m_Index((uint32_t)(&item - g_ItemList)) {}
    constexpr ItemBase(const char* itemname)
        : m_Index(ItemCHash(itemname)) {}

    constexpr const Item& operator*()  { return g_ItemList[m_Index]; }
    constexpr const Item* operator->() { return &g_ItemList[m_Index]; }
};
// An item in the inventory - points to the source version and has a quantity and position.
struct ItemSlot
{
    ItemSlot()
        : m_Base(), m_Quantity(0ui16) {}
    ItemSlot(ItemBase item)
        : m_Base(item), m_Quantity(0ui16) {}
    ItemSlot(ItemBase item, uint16_t quantity)
        : m_Base(item), m_Quantity(quantity) {}

    ItemBase m_Base; // Iterator to base item
    uint16_t m_Quantity; // How many of the item the player has in the slot
};
// Handler for item slots
// Give one of these to anything which may hold items
class Inventory
{
    std::vector<ItemSlot> m_Items;
    template<class ... Types>
    Inventory(Types ... args) : m_Items(args) {}
};

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "Inventory Menu Test");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    // @TODO: Load persistent assets & variables

    printf("Value: %i\n", ItemBase("Foo")->m_Value);

    return 0;
    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        // @TODO: simulate frame

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

    UnloadIcons();

    CloseWindow();

	return 0;
}
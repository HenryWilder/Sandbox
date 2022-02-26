#include <raylib.h>
#include <raymath.h>
#include <algorithm>
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

class Button
{
public:
    enum class State : unsigned char
    {
        NONE     = 0,
        ACTIVE   = 0b1,
        HOVERED  = 0b10,
        DISABLED = 0b100,
    };

private:
    static bool GetFlag(State flags, State bit)
    {
        return !!((unsigned char)flags & (unsigned char)bit);
    }
    static void SetFlag(State& flags, State bit, bool value)
    {
        flags = (State)((unsigned char)flags ^ ((-(unsigned char)value ^ (unsigned char)flags) & (unsigned char)bit));
    }

private:
    Rectangle m_rect;
    bool m_isToggle;
    State m_state;
    std::string m_displayName;

public:
    static constexpr bool Type_Toggle = true;
    static constexpr bool Type_Hold = false;

    Button(const std::string displayName, float x, float y, float width, float height, bool isToggle, bool startActive, bool startDisabled) :
        m_rect{ x, y, width, height },
        m_isToggle(isToggle),
        m_state((State)(((unsigned char)State::ACTIVE * (unsigned char)startActive) | ((unsigned char)State::DISABLED * (unsigned char)startDisabled))),
        m_displayName(displayName) {}

    Button(const std::string displayName, Rectangle rect, bool isToggle, bool startActive, bool startDisabled) :
        m_rect(rect),
        m_isToggle(isToggle),
        m_state((State)(((unsigned char)State::ACTIVE* (unsigned char)startActive) | ((unsigned char)State::DISABLED * (unsigned char)startDisabled))),
        m_displayName(displayName) {}

    Rectangle GetRect() const
    {
        return m_rect;
    }
    void SetRect(Rectangle rect)
    {
        m_rect = rect;
    }
    bool IsOverlapping(Vector2 pt) const
    {
        return CheckCollisionPointRec(pt, m_rect);
    }

    bool IsToggle() const
    {
        return m_isToggle;
    }
    void SetToggle(bool isToggle)
    {
        m_isToggle = isToggle;
    }

    bool IsActive() const
    {
        return GetFlag(m_state, State::ACTIVE);
    }
    void SetActive(bool isActive)
    {
        SetFlag(m_state, State::ACTIVE, isActive);
    }

    bool IsHovered() const
    {
        return GetFlag(m_state, State::HOVERED);
    }
    void SetHovered(bool hovered)
    {
        SetFlag(m_state, State::HOVERED, hovered);
    }

    bool IsDisabled() const
    {
        return GetFlag(m_state, State::DISABLED);
    }
    void SetDisabled(bool disabled)
    {
        SetFlag(m_state, State::DISABLED, disabled);
    }

    const std::string& GetDisplayName() const
    {
        return m_displayName;
    }
    void SetDisplayName(const std::string& displayName)
    {
        m_displayName = displayName;
    }
};
Button::State operator|(Button::State lValue, Button::State rValue)
{
    return (Button::State)((unsigned char)lValue | (unsigned char)rValue);
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

    std::vector<Button> buttons = {
        { "Hold button", 20, 20, 60, 20, Button::Type_Hold, false, false },
        { "Toggle button", 20, 50, 60, 20, Button::Type_Toggle, false, false },
        { "Draggable button", 20, 80, 60, 20, Button::Type_Hold, false, false },
    };

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

            Vector2 cursor = GetMousePosition();
            for (Button& button : buttons)
            {
                DrawRectangleRec(button.GetRect(), button.IsOverlapping(cursor) ? SKYBLUE : BLUE);
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
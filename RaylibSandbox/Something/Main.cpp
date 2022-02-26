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

struct RadioButtonHandler;

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
    std::string m_displayName;
    std::string m_tooltip;
    Rectangle m_rect;
    bool m_isToggle;
    State m_state;
    bool m_callbackDirty;
    RadioButtonHandler* m_group;

public:
    static constexpr bool Type_Toggle = true;
    static constexpr bool Type_Hold = false;

    Button(
        const std::string& displayName,
        const std::string& toolTip,
        float x,
        float y,
        float width,
        float height,
        bool isToggle,
        bool startActive,
        bool startDisabled,
        RadioButtonHandler* group = nullptr) :
        m_displayName(displayName),
        m_tooltip(toolTip),
        m_rect{ x, y, width, height },
        m_isToggle(isToggle),
        m_state((State)(((unsigned char)State::ACTIVE * (unsigned char)startActive) | ((unsigned char)State::DISABLED * (unsigned char)startDisabled))),
        m_callbackDirty(false),
        m_group(group)
    {}

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

    float GetX() const
    {
        return m_rect.x;
    }
    void SetX(float x)
    {
        m_rect.x = x;
    }

    float GetY() const
    {
        return m_rect.y;
    }
    void SetY(float y)
    {
        m_rect.y = y;
    }

    float GetWidth() const
    {
        return m_rect.width;
    }
    void SetWidth(float width)
    {
        m_rect.width = width;
    }

    float GetHeight() const
    {
        return m_rect.height;
    }
    void SetHeight(float height)
    {
        m_rect.height = height;
    }

    bool IsToggle() const
    {
        return m_isToggle;
    }
    bool IsHold() const
    {
        return !m_isToggle;
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

    const std::string& GetToolTip() const
    {
        return m_tooltip;
    }
    void SetToolTip(const std::string& toolTip)
    {
        m_tooltip = toolTip;
    }

    const std::string& GetDisplayName() const
    {
        return m_displayName;
    }
    void SetDisplayName(const std::string& displayName)
    {
        m_displayName = displayName;
    }

    bool IsChanged() const
    {
        return m_callbackDirty;
    }
    void MarkChanged()
    {
        m_callbackDirty = true;
    }
    void ResetChanged()
    {
        m_callbackDirty = true;
    }

    RadioButtonHandler* GetGroup() const
    {
        return m_group;
    }
    void SetGroup(RadioButtonHandler* group)
    {
        m_group = group;
    }
};

// Buttons which only allow one to be active at a time
struct RadioButtonHandler
{
    std::vector<Button*> m_children;

    void Activate(Button* selection)
    {
        for (Button* button : m_children)
        {
            button->MarkChanged();
            button->SetActive(false);
        }
        selection->SetActive(true);
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

    std::vector<RadioButtonHandler> radios = {
        {}
    };

    std::vector<Button> buttons = {
        { "Hold", "Stays active only while the mouse is pressed.",
            20, 20, 60, 20, Button::Type_Hold, false, false },
        { "Toggle", "Changes states when the mouse is pressed.",
            20, 50, 60, 20, Button::Type_Toggle, false, false },
        { "DragV", "Shows the ability to drag a button while it is held.",
            20, 80, 60, 20, Button::Type_Hold, false, false },
        { "DragH", "Shows the ability to drag a button while it is held.",
            90, 110, 60, 20, Button::Type_Hold, false, false },

        { "A", "Only one of these two can be active at a time.",
            90, 20, 20, 20, Button::Type_Toggle, false, false, &(radios[0]) },
        { "B", "Only one of these two can be active at a time.",
            90, 50, 20, 20, Button::Type_Toggle, false, false, &(radios[0]) },
        { "C'", "Only one of these two can be active at a time. Also, this one moves with the one on its left!",
            90, 80, 20, 20, Button::Type_Toggle, false, false, &(radios[0]) },
    };

    radios[0].m_children.reserve(3);
    radios[0].m_children.push_back(&(buttons[4]));
    radios[0].m_children.push_back(&(buttons[5]));
    radios[0].m_children.push_back(&(buttons[6]));

    constexpr float toolTipHoverTime = 0.5f;
    float timeSinceMouseMove = 0.0f;
    Vector2 cursor{ 0,0 };

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        // Update mouse hold still time
        if (cursor.x != GetMousePosition().x || cursor.y != GetMousePosition().y || IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            timeSinceMouseMove = 0.0f;
        else if (timeSinceMouseMove <= toolTipHoverTime) // I am irrationally afraid of floating point imprecision
            timeSinceMouseMove += GetFrameTime();

        cursor = GetMousePosition();

        // Find state
        for (Button& button : buttons)
        {
            bool hovered = button.IsOverlapping(cursor);
            button.SetHovered(hovered);
            if (hovered && button.IsToggle() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if (!!button.GetGroup())
                {
                    button.GetGroup()->Activate(&button);
                }
                else
                {
                    button.MarkChanged();
                    button.SetActive(!button.IsActive());
                }
            }
            else if (!button.IsToggle())
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hovered)
                {
                    button.MarkChanged();
                    button.SetActive(true);
                }
                else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                {
                    button.MarkChanged();
                    button.SetActive(false);
                }
            }
        }

        // Highlight grouped buttons together
        for (RadioButtonHandler& radio : radios)
        {
            bool hovered = false;
            for (Button* child : radio.m_children)
            {
                if (child->IsHovered())
                {
                    hovered = true;
                    break;
                }
            }
            if (hovered)
            {
                for (Button* child : radio.m_children)
                {
                    child->SetHovered(true);
                }
            }
        }

        // Handle 3rd button dragging
        if (buttons[2].IsActive())
        {
            Rectangle rec = buttons[2].GetRect();
            rec.y = Clamp(cursor.y - rec.height * 0.5f, 80.0f, 280.0f);

            buttons[2].SetRect(rec);

            Rectangle rec1 = buttons[3].GetRect();
            rec1.y = rec.y;
            buttons[3].SetRect(rec1);

            rec1 = buttons[6].GetRect();
            rec1.y = rec.y;
            buttons[6].SetRect(rec1);
        }
        // Handle 3rd button dragging
        if (buttons[3].IsActive())
        {
            Rectangle rec = buttons[3].GetRect();
            rec.y = Clamp(cursor.y - rec.height * 0.5f, 80.0f, 280.0f);

            buttons[3].SetRect(rec);

            Rectangle rec1 = buttons[6].GetRect();
            rec1.y = rec.y;
            buttons[6].SetRect(rec1);
        }

        // Mark handled
        for (Button& button : buttons)
        {
            button.ResetChanged();
        }

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            // Draw buttons
            for (const Button& button : buttons)
            {
                Color color;

                // Determine color of state
                if (button.IsDisabled())
                    color = GRAY;
                else
                {
                    if (button.IsHovered())
                    {
                        if (button.IsActive())
                            color = WHITE;
                        else
                            color = BLUE;
                    }
                    else
                    {
                        if (button.IsActive())
                            color = SKYBLUE;
                        else
                            color = DARKBLUE;
                    }
                }

                DrawRectangleRec(button.GetRect(), color); // Rectangle
                DrawText(button.GetDisplayName().c_str(), button.GetRect().x + 2, button.GetRect().y + 2, 8, BLACK); // Name
            }

            // Draw tooltip
            if (timeSinceMouseMove > toolTipHoverTime)
            {
                for (const Button& button : buttons)
                {
                    if (button.IsHovered())
                    {
                        Rectangle rec = { cursor.x, cursor.y + 20, 200, 200 };
                        Vector2 size = MeasureTextEx(GetFontDefault(), button.GetToolTip().c_str(), 10, 1);
                        float lines = (fmodf(size.x, rec.width - 4) / size.y);
                        rec.height = (lines) * size.y + 4.0f;

                        DrawRectangleRec(rec, WHITE);
                        rec.x += 2;
                        rec.y += 2;
                        rec.width -= 4;
                        rec.height -= 4;
                        DrawTextRec(GetFontDefault(), button.GetToolTip().c_str(), rec, 10, 1, true, BLACK);

                        break;
                    }
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
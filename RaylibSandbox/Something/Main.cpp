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

Rectangle ExpandRec(Rectangle rec, float amt)
{
    rec.x -= amt;
    rec.y -= amt;
    rec.width += amt;
    rec.height += amt;
    return rec;
}
void ExpandRec(Rectangle* rec, float amt)
{
    rec->x -= amt;
    rec->y -= amt;
    rec->width += amt;
    rec->height += amt;
}

struct RadioButtonHandler;

enum class Spacing
{
    OVERLAP, // Offset from the center of the base
    PAD, // Space between the edges
};

class Button
{
public:
    enum class State : unsigned char
    {
        NONE = 0,
        ACTIVE = 0b1,
        HOVERED = 0b10,
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

public:
    struct ColorScheme
    {
        Color disabledColor = GRAY;
        Color inactiveColor = BLUE;
        Color activeColor = SKYBLUE;
        Color inactiveColor_hovered = DARKBLUE;
        Color activeColor_hovered = WHITE;

        // Determine color of state
        Color GetStateColor(Button::State state) const
        {
            if (GetFlag(state, State::DISABLED))
                return disabledColor;
            else
            {
                if (GetFlag(state, State::HOVERED))
                {
                    if (GetFlag(state, State::ACTIVE))
                        return activeColor_hovered;
                    else
                        return inactiveColor_hovered;
                }
                else
                {
                    if (GetFlag(state, State::ACTIVE))
                        return activeColor;
                    else
                        return inactiveColor;
                }
            }
        }
    };

private:
    std::string m_displayName;
    std::string m_tooltip;
    Rectangle m_rect;
    bool m_isToggle;
    State m_state;
    bool m_callbackDirty;
    RadioButtonHandler* m_group;
    ColorScheme m_colors;
    Color m_stateColor; // Precalculated color for current state

public:
    static constexpr bool Type_Toggle = true;
    static constexpr bool Type_Hold = false;

    Button() :
        m_displayName(),
        m_tooltip(),
        m_rect{ 0, 0, 0, 0 },
        m_isToggle(false),
        m_state(State::NONE),
        m_callbackDirty(false),
        m_group(nullptr),
        m_colors(ColorScheme()),
        m_stateColor(m_colors.inactiveColor)
    {}

    Button(
        const std::string& displayName,
        const std::string& toolTip,
        Rectangle shape,
        bool isToggle,
        bool startActive,
        bool startDisabled,
        ColorScheme colors = ColorScheme(),
        RadioButtonHandler* group = nullptr) :
        m_displayName(displayName),
        m_tooltip(toolTip),
        m_rect{ shape.x, shape.y, shape.width, shape.height },
        m_isToggle(isToggle),
        m_state((State)(((unsigned char)State::ACTIVE * (unsigned char)startActive) | ((unsigned char)State::DISABLED * (unsigned char)startDisabled))),
        m_callbackDirty(false),
        m_group(group),
        m_colors(m_colors),
        m_stateColor(m_colors.inactiveColor)
    {}

    Rectangle GetRect() const { return m_rect; }
    void SetRect(Rectangle rect) { m_rect = rect; }
    bool IsOverlapping(Vector2 pt) const { return CheckCollisionPointRec(pt, m_rect); }

    float GetX() const { return m_rect.x; }
    float GetY() const { return m_rect.y; }
    float GetWidth() const { return m_rect.width; }
    float GetHeight() const { return m_rect.height; }

    void SetX(float x) { m_rect.x = x; }
    void SetY(float y) { m_rect.y = y; }
    void SetWidth(float width) { m_rect.width = width; }
    void SetHeight(float height) { m_rect.height = height; }

    void SetPosition(float x, float y)
    {
        m_rect.x = x;
        m_rect.y = y;
    }
    void SetShape(float width, float height)
    {
        m_rect.width = width;
        m_rect.height = height; 
    }

    // Offset is treated as padding. Negatives put the button left of base, while positives put it right.
    void OffsetFrom(const Button& base, Spacing xType, float x, Spacing yType, float y)
    {
        switch (xType)
        {
        case Spacing::OVERLAP:
            SetX(base.GetX() + (base.GetWidth() - GetWidth()) * 0.5f + x);
            break;

        case Spacing::PAD:
            if (x < 0.0) SetX(base.GetX() - (     GetWidth() - x));
            else         SetX(base.GetX() + (base.GetWidth() + x));
            break;
        }

        switch (yType)
        {
        case Spacing::OVERLAP:
            SetY(base.GetY() + (base.GetHeight() - GetHeight()) * 0.5f + y);
            break;

        case Spacing::PAD:
            if (y < 0.0) SetY(base.GetY() - (     GetHeight() - y));
            else         SetY(base.GetY() + (base.GetHeight() + y));
            break;
        }
    }
    
    // Takes the width/height of the base without taking the position
    void CopyShape(const Button& base)
    {
        SetWidth(base.GetWidth());
        SetHeight(base.GetHeight());
    }

    bool IsToggle() const { return m_isToggle; }
    bool IsHold() const { return !m_isToggle; }
    void SetToggle(bool isToggle) { m_isToggle = isToggle; }

    bool IsActive() const { return GetFlag(m_state, State::ACTIVE); }
    bool IsHovered() const { return GetFlag(m_state, State::HOVERED); }
    bool IsDisabled() const { return GetFlag(m_state, State::DISABLED); }
    void SetActive(bool isActive) { SetFlag(m_state, State::ACTIVE, isActive); }
    void SetHovered(bool hovered) { SetFlag(m_state, State::HOVERED, hovered); }
    void SetDisabled(bool disabled) { SetFlag(m_state, State::DISABLED, disabled); }

    const std::string& GetToolTip() const { return m_tooltip; }
    const std::string& GetDisplayName() const { return m_displayName; }
    void SetToolTip(const std::string& toolTip) { m_tooltip = toolTip; }
    void SetDisplayName(const std::string& displayName) { m_displayName = displayName; }

    bool IsChanged() const { return m_callbackDirty; }
    void MarkChanged() { m_callbackDirty = true; }
    void ResetChanged() { m_callbackDirty = false; }

    RadioButtonHandler* GetGroup() const { return m_group; }
    void SetGroup(RadioButtonHandler* group) { m_group = group; }

    Color GetColor() const { return m_stateColor; }
    void SetColorScheme(ColorScheme scheme) { m_colors = scheme; }
    void UpdateStateColor() { m_stateColor = m_colors.GetStateColor(m_state); }

    Color GetColor_Disabled() { return m_colors.disabledColor; }
    Color GetColor_Inactive() { return m_colors.inactiveColor; }
    Color GetColor_Active() { return m_colors.activeColor; }
    Color GetColor_Inactive_Hovered() { return m_colors.inactiveColor_hovered; }
    Color GetColor_Active_Hovered() { return m_colors.activeColor_hovered; }

    void SetColor_Disabled(Color color) { m_colors.disabledColor = color; }
    void SetColor_Inactive(Color color) { m_colors.inactiveColor = color; }
    void SetColor_Active(Color color) { m_colors.activeColor = color; }
    void SetColor_Inactive_Hovered(Color color) { m_colors.inactiveColor_hovered = color; }
    void SetColor_Active_Hovered(Color color) { m_colors.activeColor_hovered = color; }
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

class UIHandler
{
private:
    UIHandler() : m_toolTipHoverTime(0.5f), m_timeSinceMouseMove(0.0f), m_cursor(Vector2Zero()), m_tooltipButton(nullptr), m_buttons{}, m_groups{} {}

    float m_toolTipHoverTime;
    float m_timeSinceMouseMove;
    Vector2 m_cursor;
    Button* m_tooltipButton;
    std::vector<Button*> m_buttons;
    std::vector<RadioButtonHandler*> m_groups;

public:
    static UIHandler& Global()
    {
        static UIHandler object;
        return object;
    }

    const std::vector<Button*>& GetButtons() const
    {
        return m_buttons;
    }

    // How many buttons to reserve space for (accounts for existing space, you don't need to add that)
    void Expect(size_t newButtons)
    {
        m_buttons.reserve(m_buttons.size() + newButtons);
    }

    // For when you're certain the button being added is new
    void AddButton_New(Button* button)
    {
        if (!!button)
            m_buttons.push_back(button);
    }
    void AddButton(Button* button)
    {
        if (!!button && std::find(m_buttons.begin(), m_buttons.end(), button) == m_buttons.end())
            m_buttons.push_back(button);
    }

    // All buttons being supplied are already in the handler
    void CreateButtonGroup_FromExisting(RadioButtonHandler* group, const std::vector<Button*>& buttons)
    {
        if (!group) return;
        m_groups.push_back(group);
        m_groups.back()->m_children.reserve(buttons.size());
        for (Button* button : buttons)
        {
            button->SetGroup(m_groups.back());
            m_groups.back()->m_children.push_back(button);
        }
    }
    // All buttons being supplied are NOT already in the handler
    void CreateButtonGroup_FromNew(RadioButtonHandler* group, const std::vector<Button*>& buttons)
    {
        if (!group) return;
        m_groups.push_back(group);
        m_groups.back()->m_children.reserve(buttons.size());
        for (Button* button : buttons)
        {
            AddButton_New(button);
            button->SetGroup(m_groups.back());
            m_groups.back()->m_children.push_back(button);
        }
    }
    void CreateButtonGroup(RadioButtonHandler* group, const std::vector<Button*>& buttons)
    {
        if (!group) return;
        m_groups.push_back(group);
        m_groups.back()->m_children.reserve(buttons.size());
        for (Button* button : buttons)
        {
            AddButton(button);
            button->SetGroup(m_groups.back());
            m_groups.back()->m_children.push_back(button);
        }
    }

    void UpdateCursorPos(Vector2 newPosition)
    {
        // Update mouse hold still time
        if (m_cursor.x != newPosition.x || m_cursor.y != newPosition.y || IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            m_timeSinceMouseMove = 0.0f;
        else if (m_timeSinceMouseMove <= m_toolTipHoverTime) // I am irrationally afraid of floating point imprecision
            m_timeSinceMouseMove += GetFrameTime();

        m_cursor = newPosition;
    }
    void UpdateButtons()
    {
        // Cleanup from last frame
        for (Button* button : m_buttons)
        {
            button->ResetChanged();
        }

        // Find state
        for (Button* button : m_buttons)
        {
            bool hovered = button->IsOverlapping(GetMousePosition());
            button->SetHovered(hovered);
            if (hovered && button->IsToggle() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if (!!button->GetGroup())
                {
                    button->GetGroup()->Activate(button);
                }
                else
                {
                    button->MarkChanged();
                    button->SetActive(!button->IsActive());
                }
            }
            else if (!button->IsToggle())
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hovered)
                {
                    button->MarkChanged();
                    button->SetActive(true);
                }
                else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                {
                    button->MarkChanged();
                    button->SetActive(false);
                }
            }
        }

        // Change cursor
        {
            bool hovering = false;
            for (Button* button : m_buttons)
            {
                if (!button->IsDisabled() && button->IsHovered())
                {
                    hovering = true;
                    break;
                }
            }

            SetMouseCursor(hovering ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
        }

        // Get tooltip
        {
            m_tooltipButton = nullptr;
            for (Button* button : m_buttons)
            {
                if (!button->GetToolTip().empty() && button->IsOverlapping(m_cursor))
                {
                    m_tooltipButton = button;
                    break;
                }
            }
        }

        // Highlight grouped buttons together
        for (RadioButtonHandler* group : m_groups)
        {
            bool hovered = false;
            for (Button* child : group->m_children)
            {
                if (child->IsHovered())
                {
                    hovered = true;
                    break;
                }
            }
            if (hovered)
            {
                for (Button* child : group->m_children)
                {
                    child->SetHovered(true);
                }
            }
        }

        for (Button* button : m_buttons)
        {
            button->UpdateStateColor();
        }
    }
    void Draw()
    {
        // Draw buttons
        for (const Button* button : m_buttons)
        {
            DrawRectangleRec(button->GetRect(), button->GetColor()); // Rectangle
            DrawText(button->GetDisplayName().c_str(), button->GetRect().x + 2, button->GetRect().y + 2, 8, BLACK); // Name
        }

        // Draw tooltip
        if (m_timeSinceMouseMove > m_toolTipHoverTime)
        {
            if (!!m_tooltipButton)
            {
                Rectangle rec = { m_tooltipButton->GetX(), m_tooltipButton->GetY() + m_tooltipButton->GetHeight(), 200, 100 };
                DrawRectangleRec(rec, WHITE);
                DrawTextRec(GetFontDefault(), m_tooltipButton->GetToolTip().c_str(), ExpandRec(rec, -2.0f), 10, 1, true, BLACK);
            }
        }
    }

    void Unload()
    {
        m_buttons.clear();
        m_groups.clear();
    }
};


// Returns true when program should exit
bool MenuScreen()
{
    Button button_start;
    button_start.SetDisplayName("Start");
    button_start.SetRect({ 20, 20, 60, 20 });

    Button button_close;
    button_close.SetDisplayName("Close");
    button_close.CopyShape(button_start);
    button_close.OffsetFrom(button_start, Spacing::OVERLAP, 0.0f, Spacing::PAD, 10.0f);

    UIHandler::Global().Expect(2);
    UIHandler::Global().AddButton_New(&button_start);
    UIHandler::Global().AddButton_New(&button_close);

    while (true)
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        UIHandler::Global().UpdateCursorPos(GetMousePosition());
        UIHandler::Global().UpdateButtons();

        if (button_start.IsActive())
            break;

        if (button_close.IsActive())
            return true;

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            UIHandler::Global().Draw();

        } EndDrawing();

        if (WindowShouldClose())
            return true;
    }

    UIHandler::Global().Unload();
    return false;
}


// Returns true when program should exit
bool GameScreen()
{
    Button button_Hold;
    button_Hold.SetDisplayName("Hold");
    button_Hold.SetToolTip("Stays active only while the mouse is pressed.");
    button_Hold.SetShape(60,20);
    button_Hold.SetPosition(20,20);

    Button button_Toggle;
    button_Toggle.SetDisplayName("Toggle");
    button_Toggle.SetToolTip("Changes states when the mouse is pressed.");
    button_Toggle.SetToggle(true);
    button_Toggle.CopyShape(button_Hold);
    button_Toggle.OffsetFrom(button_Hold, Spacing::OVERLAP, 0, Spacing::PAD, 10);

    Button button_DragV;
    button_DragV.SetDisplayName("DragV");
    button_DragV.SetToolTip("Shows the ability to drag a button while it is held.");
    button_DragV.CopyShape(button_Hold);
    button_DragV.OffsetFrom(button_Toggle, Spacing::OVERLAP, 0, Spacing::PAD, 10);

    Button button_DragH;
    button_DragH.SetDisplayName("DragH");
    button_DragH.SetToolTip(button_DragH.GetToolTip());
    button_DragH.CopyShape(button_Hold);
    button_DragH.OffsetFrom(button_DragV, Spacing::OVERLAP, 0, Spacing::PAD, 10);

    RadioButtonHandler group0;

    Button button_A;
    button_A.SetDisplayName("A");
    button_A.SetToolTip("Only one of these two can be active at a time.");
    button_A.SetToggle(true);
    button_A.SetShape(20,20);
    button_A.OffsetFrom(button_Hold, Spacing::PAD, 10, Spacing::OVERLAP, 0);

    Button button_B;
    button_B.SetDisplayName("B");
    button_B.SetToolTip(button_A.GetToolTip());
    button_B.SetToggle(true);
    button_B.CopyShape(button_A);
    button_B.OffsetFrom(button_A, Spacing::OVERLAP, 0, Spacing::PAD, 10);

    Button button_C;
    button_C.SetDisplayName("C");
    button_C.SetToolTip(button_A.GetToolTip() + " Also, this one moves with the one on its left!");
    button_C.SetToggle(true);
    button_C.CopyShape(button_A);
    button_C.OffsetFrom(button_B, Spacing::OVERLAP, 0, Spacing::PAD, 10);

    UIHandler::Global().Expect(7);
    UIHandler::Global().AddButton_New(&button_Hold);
    UIHandler::Global().AddButton_New(&button_Toggle);
    UIHandler::Global().AddButton_New(&button_DragV);
    UIHandler::Global().AddButton_New(&button_DragH);
    UIHandler::Global().CreateButtonGroup_FromNew(&group0, { &button_A, &button_B, &button_C });

    while (true)
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        UIHandler::Global().UpdateCursorPos(GetMousePosition());
        UIHandler::Global().UpdateButtons();

        // Handle 3rd button dragging
        if (button_DragV.IsActive())
        {
            float y = Clamp(GetMousePosition().y - button_DragV.GetHeight() * 0.5f, 80.0f, 280.0f);

            button_DragV.SetY(y);
            button_DragH.SetY(y + 30);
            button_C.SetY(y);
        }
        // Handle 3rd button dragging
        if (button_DragH.IsActive())
        {
            float x = Clamp(GetMousePosition().x - button_DragH.GetWidth() * 0.5f, 20.0f, 220.0f);

            button_DragH.SetX(x);
            button_C.SetX(x + 70);
        }

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            UIHandler::Global().Draw();

        } EndDrawing();

        if (WindowShouldClose())
            return true;
    }

    UIHandler::Global().Unload();
    return true;
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

    enum class GameMode
    {
        Menu,
        Game,
    } mode = GameMode::Menu;

    bool requestedClose = false;
    while (!requestedClose)
    {
        switch (mode)
        {
        case GameMode::Menu: requestedClose = MenuScreen(); mode = GameMode::Game; break;
        case GameMode::Game: requestedClose = GameScreen(); mode = GameMode::Menu; break;
        }
    }

    CloseWindow();

	return 0;
}
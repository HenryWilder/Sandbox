#pragma once
#include <vector>

Rectangle ExpandRec(Rectangle rec, float amt);
void ExpandRec(Rectangle* rec, float amt);

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
        NONE     = 0,
        ACTIVE   = 0b1,
        HOVERED  = 0b10,
        DISABLED = 0b100,
    };

private:
    static bool GetFlag(State flags, State bit);
    static void SetFlag(State& flags, State bit, bool value);

public:
    struct ColorScheme
    {
        Color disabledColor = GRAY;
        Color inactiveColor = BLUE;
        Color activeColor = SKYBLUE;
        Color inactiveColor_hovered = DARKBLUE;

        // Determine color of state
        Color GetStateColor(Button::State state) const;
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

    Button();

    Button(
        const std::string& displayName, const std::string& toolTip,
        Rectangle shape,
        bool isToggle, bool startActive, bool startDisabled,
        ColorScheme colors = ColorScheme(),
        RadioButtonHandler* group = nullptr);

    Rectangle GetRect() const;
    void SetRect(Rectangle rect);
    bool IsOverlapping(Vector2 pt) const;

    float GetX() const;
    float GetY() const;
    float GetWidth() const;
    float GetHeight() const;

    void SetX(float x);
    void SetY(float y);
    void SetWidth(float width);
    void SetHeight(float height);

    void SetPosition(float x, float y);
    void SetShape(float width, float height);

    // Offset is treated as padding. Negatives put the button left of base, while positives put it right.
    void OffsetFrom(const Button& base, Spacing xType, float x, Spacing yType, float y);

    // Takes the width/height of the base without taking the position
    void CopyShape(const Button& base);

    bool IsToggle() const;
    bool IsHold() const;
    void SetToggle(bool isToggle);

    bool IsActive() const;
    bool IsHovered() const;
    bool IsDisabled() const;
    void SetActive(bool isActive);
    void SetHovered(bool hovered);
    void SetDisabled(bool disabled);

    const std::string& GetToolTip() const;
    const std::string& GetDisplayName() const;
    void SetToolTip(const std::string& toolTip);
    void SetDisplayName(const std::string& displayName);

    bool IsChanged() const;
    void MarkChanged();
    void ResetChanged();

    RadioButtonHandler* GetGroup() const;
    void SetGroup(RadioButtonHandler* group);

    Color GetColor() const;
    void SetColorScheme(ColorScheme scheme);
    void UpdateStateColor();

    Color GetColor_Disabled();
    Color GetColor_Inactive();
    Color GetColor_Active();
    Color GetColor_Inactive_Hovered();
    Color GetColor_Active_Hovered();

    void SetColor_Disabled(Color color);
    void SetColor_Inactive(Color color);
    void SetColor_Active(Color color);
    void SetColor_Inactive_Hovered(Color color);
    void SetColor_Active_Hovered(Color color);
};

// Buttons which only allow one to be active at a time
struct RadioButtonHandler
{
    std::vector<Button*> m_children;
    void Activate(Button* selection);
};

class UIHandler
{
private:
    UIHandler();

    float m_toolTipHoverTime;
    float m_timeSinceMouseMove;
    Vector2 m_cursor;
    Button* m_tooltipButton;
    std::vector<Button*> m_buttons;
    std::vector<RadioButtonHandler*> m_groups;

public:
    static UIHandler& Global();

    const std::vector<Button*>& GetButtons() const;

    // How many buttons to reserve space for (accounts for existing space, you don't need to add that)
    void Expect(size_t newButtons);

    // For when you're certain the button being added is not already in the handler
    // (skips check; faster but might create duplicate references if careless)
    void AddButton_New(Button* button);
    void AddButton(Button* button);

    // All buttons being supplied are already in the handler
    void CreateButtonGroup_FromExisting(RadioButtonHandler* group, const std::vector<Button*>& buttons);
    // All buttons being supplied are NOT already in the handler
    void CreateButtonGroup_FromNew(RadioButtonHandler* group, const std::vector<Button*>& buttons);
    void CreateButtonGroup(RadioButtonHandler* group, const std::vector<Button*>& buttons);

    void UpdateCursorPos(Vector2 newPosition);

    void UpdateButtons();

    void Draw();

    void Unload();
};

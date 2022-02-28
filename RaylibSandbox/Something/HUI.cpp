#include <raylib.h>
#include <raymath.h>
#include <algorithm>
#include <functional>
#include <vector>
#include "HUI.h"

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
    _ASSERT_EXPR(!!rec, "ExpandRec(*) was passed a nullptr.");
    rec->x -= amt;
    rec->y -= amt;
    rec->width += amt;
    rec->height += amt;
}

#pragma region Button

bool Button::GetFlag(State flags, State bit)
{
    return !!((unsigned char)flags & (unsigned char)bit);
}
void Button::SetFlag(State& flags, State bit, bool value)
{
    flags = (State)((unsigned char)flags ^ ((-(unsigned char)value ^ (unsigned char)flags) & (unsigned char)bit));
}

#pragma region ColorScheme

// Determine color of state
Color Button::ColorScheme::GetStateColor(Button::State state) const
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

#pragma endregion

Button::Button()
    :
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

Button::Button(
    const std::string& displayName, const std::string& toolTip,
    Rectangle shape,
    bool isToggle, bool startActive, bool startDisabled,
    ColorScheme colors = ColorScheme(),
    RadioButtonHandler* group = nullptr)
    :
    m_displayName(displayName), m_tooltip(toolTip),
    m_rect{ shape.x, shape.y, shape.width, shape.height },
    m_isToggle(isToggle),
    m_state((State)(((unsigned char)State::ACTIVE * (unsigned char)startActive) | ((unsigned char)State::DISABLED * (unsigned char)startDisabled))),
    m_callbackDirty(false),
    m_group(group),
    m_colors(m_colors),
    m_stateColor(m_colors.inactiveColor)
{}

Rectangle Button::GetRect() const { return m_rect; }
void Button::SetRect(Rectangle rect) { m_rect = rect; }
bool Button::IsOverlapping(Vector2 pt) const { return CheckCollisionPointRec(pt, m_rect); }

float Button::GetX() const { return m_rect.x; }
float Button::GetY() const { return m_rect.y; }
float Button::GetWidth () const { return m_rect.width;  }
float Button::GetHeight() const { return m_rect.height; }

void Button::SetX(float x) { m_rect.x = x; }
void Button::SetY(float y) { m_rect.y = y; }
void Button::SetWidth (float width ) { m_rect.width  = width;  }
void Button::SetHeight(float height) { m_rect.height = height; }

void Button::SetPosition(float x, float y) { m_rect.x = x; m_rect.y = y; }
void Button::SetShape(float width, float height)
{
    m_rect.width  = width;
    m_rect.height = height; 
}

// Offset is treated as padding. Negatives put the button left of base, while positives put it right.
void Button::OffsetFrom(const Button& base, Spacing xType, float x, Spacing yType, float y)
{
    _ASSERT_EXPR((xType == Spacing::OVERLAP) || (xType == Spacing::PAD), "OffsetFrom requires that xType be either OVERLAP or PAD.");
    _ASSERT_EXPR((yType == Spacing::OVERLAP) || (yType == Spacing::PAD), "OffsetFrom requires that yType be either OVERLAP or PAD.");

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
void Button::CopyShape(const Button & base)
{
    SetWidth (base.GetWidth ());
    SetHeight(base.GetHeight());
}

bool Button::IsToggle() const { return  m_isToggle; }
bool Button::IsHold()   const { return !m_isToggle; }
void Button::SetToggle(bool isToggle) { m_isToggle = isToggle; }

bool Button::IsActive()     const { return GetFlag(m_state, State::ACTIVE); }
bool Button::IsHovered()    const { return GetFlag(m_state, State::HOVERED); }
bool Button::IsDisabled()   const { return GetFlag(m_state, State::DISABLED); }
void Button::SetActive(bool isActive)   { SetFlag(m_state, State::ACTIVE, isActive); }
void Button::SetHovered(bool hovered)   { SetFlag(m_state, State::HOVERED, hovered); }
void Button::SetDisabled(bool disabled) { SetFlag(m_state, State::DISABLED, disabled); }

const std::string& Button::GetToolTip() const { return m_tooltip; }
const std::string& Button::GetDisplayName() const { return m_displayName; }
void Button::SetToolTip(const std::string & toolTip) { m_tooltip = toolTip; }
void Button::SetDisplayName(const std::string & displayName) { m_displayName = displayName; }

bool Button::IsChanged() const { return m_callbackDirty; }
void Button::MarkChanged() { m_callbackDirty = true; }
void Button::ResetChanged() { m_callbackDirty = false; }

RadioButtonHandler* Button::GetGroup() const { return m_group; }
void Button::SetGroup(RadioButtonHandler* group) { m_group = group; }

Color Button::GetColor() const { return m_stateColor; }
void Button::SetColorScheme(ColorScheme scheme) { m_colors = scheme; }
void Button::UpdateStateColor() { m_stateColor = m_colors.GetStateColor(m_state); }

Color Button::GetColor_Disabled() { return m_colors.disabledColor; }
Color Button::GetColor_Inactive() { return m_colors.inactiveColor; }
Color Button::GetColor_Active() { return m_colors.activeColor; }
Color Button::GetColor_Inactive_Hovered() { return m_colors.inactiveColor_hovered; }
Color Button::GetColor_Active_Hovered() { return m_colors.activeColor_hovered; }

void Button::SetColor_Disabled(Color color) { m_colors.disabledColor = color; }
void Button::SetColor_Inactive(Color color) { m_colors.inactiveColor = color; }
void Button::SetColor_Active(Color color) { m_colors.activeColor = color; }
void Button::SetColor_Inactive_Hovered(Color color) { m_colors.inactiveColor_hovered = color; }
void Button::SetColor_Active_Hovered(Color color) { m_colors.activeColor_hovered = color; }

#pragma endregion

#pragma region RadioButtonHandler

void RadioButtonHandler::Activate(Button* selection)
{
    for (Button* button : m_children)
    {
        button->MarkChanged();
        button->SetActive(false);
    }
    selection->SetActive(true);
}

#pragma endregion

#pragma region UIHandler

UIHandler::UIHandler()
    :
    m_toolTipHoverTime(0.5f),
    m_timeSinceMouseMove(0.0f),
    m_cursor(Vector2Zero()),
    m_tooltipButton(nullptr),
    m_buttons{},
    m_groups{}
{}

UIHandler& UIHandler::Global()
{
    static UIHandler object;
    return object;
}

const std::vector<Button*>& UIHandler::GetButtons() const { return m_buttons; }

// How many buttons to reserve space for (accounts for existing space, you don't need to add that)
void UIHandler::Expect(size_t newButtons)
{
    m_buttons.reserve(m_buttons.size() + newButtons);
}

// For when you're certain the button being added is new
void UIHandler::AddButton_New(Button* button)
{
    _ASSERT_EXPR(!!button, "AddButton_New() was passed a nullptr.");
    _ASSERT_EXPR(std::find(m_buttons.begin(), m_buttons.end(), button) == m_buttons.end(), "AddButton_New() was passed a button already existing in the UIHandler.");
    if (!!button)
        m_buttons.push_back(button);
}
void UIHandler::AddButton(Button* button)
{
    _ASSERT_EXPR(!!button, "AddButton() was passed a nullptr.");
    if (std::find(m_buttons.begin(), m_buttons.end(), button) == m_buttons.end())
        m_buttons.push_back(button);
}

// All buttons being supplied are already in the handler
void UIHandler::CreateButtonGroup_FromExisting(RadioButtonHandler* group, const std::vector<Button*>& buttons)
{
    _ASSERT_EXPR(!!group, "The RadioButtonHandler* parameter passed to CreateButtonGroup_FromExisting() cannot be nullptr.");
    _ASSERT_EXPR(std::find(m_groups.begin(), m_groups.end(), group) == m_groups.end(), "The RadioButtonHandler* parameter passed to CreateButtonGroup_FromExisting() is expected not to pre-exist in the UIHandler.");
    m_groups.push_back(group);
    m_groups.back()->m_children.reserve(buttons.size());
    for (Button* button : buttons)
    {
        _ASSERT_EXPR(std::find(m_buttons.begin(), m_buttons.end(), button) != m_buttons.end(), "CreateButtonGroup_FromExisting() was passed a button that did not exist in the UIHandler.");
        button->SetGroup(m_groups.back());
        m_groups.back()->m_children.push_back(button);
    }
}
// All buttons being supplied are NOT already in the handler
void UIHandler::CreateButtonGroup_FromNew(RadioButtonHandler* group, const std::vector<Button*>& buttons)
{
    _ASSERT_EXPR(!!group, "The RadioButtonHandler* parameter passed to CreateButtonGroup_FromNew() cannot be nullptr.");
    _ASSERT_EXPR(std::find(m_groups.begin(), m_groups.end(), group) == m_groups.end(), "The RadioButtonHandler* parameter passed to CreateButtonGroup_FromNew() is expected not to pre-exist in the UIHandler.");
    m_groups.push_back(group);
    m_groups.back()->m_children.reserve(buttons.size());
    for (Button* button : buttons)
    {
        _ASSERT_EXPR(std::find(m_buttons.begin(), m_buttons.end(), button) == m_buttons.end(), "CreateButtonGroup_FromNew() was passed a button already existing in the UIHandler.");
        AddButton_New(button);
        button->SetGroup(m_groups.back());
        m_groups.back()->m_children.push_back(button);
    }
}
void UIHandler::CreateButtonGroup(RadioButtonHandler* group, const std::vector<Button*>& buttons)
{
    _ASSERT_EXPR(!!group, "The RadioButtonHandler* parameter passed to CreateButtonGroup() cannot be nullptr.");
    _ASSERT_EXPR(std::find(m_groups.begin(), m_groups.end(), group) == m_groups.end(), "The RadioButtonHandler* parameter passed to CreateButtonGroup() is expected not to pre-exist in the UIHandler.");
    m_groups.push_back(group);
    m_groups.back()->m_children.reserve(buttons.size());
    for (Button* button : buttons)
    {
        AddButton(button);
        button->SetGroup(m_groups.back());
        m_groups.back()->m_children.push_back(button);
    }
}

void UIHandler::UpdateCursorPos(Vector2 newPosition)
{
    // Update mouse hold still time
    if (m_cursor.x != newPosition.x || m_cursor.y != newPosition.y || IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        m_timeSinceMouseMove = 0.0f;
    else if (m_timeSinceMouseMove <= m_toolTipHoverTime) // I am irrationally afraid of floating point imprecision
        m_timeSinceMouseMove += GetFrameTime();

    m_cursor = newPosition;
}

void UIHandler::UpdateButtons()
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
    m_tooltipButton = nullptr;
    if (m_timeSinceMouseMove > m_toolTipHoverTime)
    {
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

    // Update state color
    for (Button* button : m_buttons)
    {
        button->UpdateStateColor();
    }
}

void UIHandler::Draw()
{
    // Draw buttons
    for (const Button* button : m_buttons)
    {
        DrawRectangleRec(button->GetRect(), button->GetColor()); // Rectangle
        DrawText(button->GetDisplayName().c_str(), button->GetRect().x + 2, button->GetRect().y + 2, 8, BLACK); // Name
    }

    // Draw tooltip
    if (!!m_tooltipButton)
    {
        Rectangle rec = { m_cursor.x, m_cursor.y + 25, 200, 100 };
        DrawRectangleRec(rec, WHITE);
        DrawTextRec(GetFontDefault(), m_tooltipButton->GetToolTip().c_str(), ExpandRec(rec, -2.0f), 10, 1, true, BLACK);
    }
}

void UIHandler::Unload()
{
    m_buttons.clear();
    m_groups.clear();
}

#pragma endregion
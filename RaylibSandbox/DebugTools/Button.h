#pragma once
#include <raylib.h>
#include <cmath>

namespace std {
    // Declaration
    template<class _Ty, class _Alloc = _Ty>
    class vector;
}

struct ButtonData {
    Vector2 position;
    bool defaultState = false;

    bool enabled = true;
    bool visible = true;

    Color buttonTrueColor = LIGHTGRAY;
};

class Button {
public:

    Button() = default;
    Button(ButtonData data);

    bool GetValue() const;
    void ToggleValue();

    static constexpr float GetButtonWidth();
    static constexpr float GetButtonHeight();
    static constexpr float GetButtonHalfWidth();
    static constexpr float GetButtonHalfHeight();

    float GetButtonX() const;
    float GetButtonY() const;

    Vector2 GetButtonPosition() const;
    Vector2 GetButtonCenterPosition() const;

    Rectangle GetButtonRect() const;

    bool IsHovered() const;

    Color GetButtonColor() const;

    void Draw() const;
    static void Update(std::vector<Button>& vec);
    static void Draw(std::vector<Button>& vec);

private:

    static constexpr int s_buttonWidth = 16;
    static constexpr int s_buttonHeight = 16;
    static constexpr Color s_buttonFalseColor = BLACK;
    Color m_buttonTrueColor;

    Vector2 m_position;
    bool b_state;

    bool b_enabled;
    bool b_visible;
};
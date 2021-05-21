#pragma once
#include <raylib.h>
#include <cmath>

namespace std {
    // Declaration
    template<class _Ty, class _Alloc = _Ty>
    class vector;
}

class Picker;

struct PickerData {
    Vector2 position;
    Picker* set;

    bool enabled = true;
    bool visible = true;

    Color activeColor = LIGHTGRAY;
};

class Picker {
public:

    Picker() = default;
    Picker(PickerData data);

    bool GetValue() const;
    void ToggleValue();

    static constexpr float GetPickerRadius();

    float GetPickerX() const;
    float GetPickerY() const;

    Vector2 GetPickerPosition() const;

    bool IsHovered() const;

    Color GetPickerColor() const;

    void Draw() const;
    static void Update(std::vector<Picker>& vec);
    static void Draw(std::vector<Picker>& vec);

private:

    static constexpr int s_buttonWidth = 16;
    static constexpr int s_buttonHeight = 16;
    static constexpr Color s_pickerFalseColor = BLACK;
    Color m_pickerTrueColor;

    Vector2 m_position;
    bool b_state;

    bool b_enabled;
    bool b_visible;
};

class SelectorSet {
public:

    SelectorSet() = default;
    SelectorSet(std::vector<Picker> pickers);

    void Draw() const;
    static void Update(std::vector<SelectorSet>& vec);
    static void Draw(std::vector<SelectorSet>& vec);

private:
};
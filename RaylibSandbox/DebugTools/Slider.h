#pragma once
#include <raylib.h>
#include <cmath>

namespace std {
    // Declaration
    template<class _Ty, class _Alloc = _Ty>
    class vector;
}

struct SliderData {
    // Display
    Vector2 position; // The position of the slider on the screen (start of the track)
    float length = 100.0f; // Length of the track on the screen

    // Range
    float min; // The minimum value when clamping
    float max; // The maximum value when clamping

    float initValue = NAN; // Starts at midpoint when constructed if both this and defaultValue are NaN -- defaultValue takes priority as the construction value if applicable

    float increment = 0.0f; // 0.0f for continuous
    float defaultValue = NAN; // NaN to disable default value. Uses initValue if this is the case.

    // Flags
    bool horizontal = true; // Whether the slider is horizontal
    bool visible = true; // Whether the slider is displayed (also disables interaction with the slider when false)
    bool enabled = true; // Whether the slider can be interacted with

    Color handleColor = LIGHTGRAY;
};

class Slider {
public:
    Slider() = default;
    Slider(SliderData data);

    // Controller

    static constexpr float GetHandleWidth(); // The width of the slider handle -- parallel to the track
    static constexpr float GetHandleLength(); // The length of the slider handle -- perpendicular to the track

    static constexpr float GetHandleHalfWidth(); // The width of the slider handle -- parallel to the track
    static constexpr float GetHandleHalfLength(); // The length of the slider handle -- perpendicular to the track

    static Vector2 GetHandleExtents();
    static Vector2 GetHandleHalfSize();

    float GetHandleX() const;
    float GetHandleY() const;

    Vector2 GetHandlePosition() const;

    Rectangle GetHandleRect() const;
    Rectangle GetHandleInsetRect() const;

    bool IsHandleHovered() const;

    // Flags

    bool IsHorizontal() const;
    bool IsVisible() const;
    bool IsBeingHeld() const;
    bool IsEnabled() const;

    // Range

    float GetValue() const;
    float GetRangeMin() const;
    float GetRangeMax() const;
    float GetDefaultValueTrackValue() const;
    Vector2 GetDefaultValueTrackPosition() const;
    float GetIncrementValue() const;
    float SnapToIncrement(float value) const;

    bool HasDefaultValue() const;
    bool HasIncrement() const;

    // Display

    float GetTrackLength() const;
    float GetTrackStartX() const;
    float GetTrackStartY() const;
    float GetTrackStart() const; // Dependent on b_horizontal

    float GetTrackEndX() const;
    float GetTrackEndY() const;
    float GetTrackEnd() const; // Dependent on b_horizontal

    Vector2 GetTrackStartPosition() const;
    Vector2 GetTrackEndPosition() const;

    Color GetHandleColor() const;
    Color GetHandleOutlineColor() const;

public:

    void UpdateDrag(); // Update the position/value of a slider being dragged
    static void Update(Slider* array, size_t count = 1); // Update an array of sliders
    static void Update(std::vector<Slider>& array); // Update an array of sliders
    static void Update(std::vector<Slider*>& array); // Update an array of sliders

    void Draw() const; // Draw a slider
    static void Draw(Slider* array, size_t count = 1); // Draw an array of sliders
    static void Draw(std::vector<Slider>& array); // Draw an array of sliders
    static void Draw(std::vector<Slider*>& array); // Draw an array of sliders

private:

    // Controller

    static constexpr int s_handleWidth = 6; // The width of the slider handle -- parallel to the track
    static constexpr int s_handleLength = 16; // The length of the slider handle -- perpendicular to the track
    static constexpr Color s_trackColor = GRAY; // Color of the track
    Color m_handleColor; // Color of the handle

    // Flags

    bool b_horizontal; // Whether the slider is horizontal
    bool b_visible; // Whether the slider is displayed (also disables interaction with the slider when false)
    bool b_beingHeld; // Whether the slider is currently being held
    bool b_enabled; // Whether the slider can be interacted with

    // Range

    float m_value; // Also controls the handle's position
    float m_default; // NaN to disable default value
    float m_min, m_max; // Permited range for value
    float m_incr; // 0.0f for continuous

    // Display

    Vector2 m_position; // Start position of the slider track
    float m_length; // Length of the track on the screen
};

class ColorSlider {
public:

    ColorSlider(Vector2 position, Slider* _r, Slider* _g, Slider* _b);
    ColorSlider(Vector2 position, std::vector<Slider>& dest);

    Color GetValue() const;

private:
    Slider *r, *g, *b;
};
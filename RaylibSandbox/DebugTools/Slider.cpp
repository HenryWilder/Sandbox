#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "Math.h"
#include "Slider.h"

Slider::Slider(SliderData data) :
    b_horizontal(data.horizontal),
    b_visible(data.visible),
    b_beingHeld(false),
    b_enabled(data.enabled),

    m_value(((isnan(data.defaultValue)) ? ((data.min + data.max) / 2.0f) : (data.defaultValue))),
    m_default(data.defaultValue),
    m_min(data.min),
    m_max(data.max),
    m_incr(data.increment),

    m_position(data.position),
    m_length(data.length)
{}

constexpr float Slider::GetHandleWidth() {
    return (float)s_handleWidth;
}
constexpr float Slider::GetHandleLength() {
    return (float)s_handleLength;
}

constexpr float Slider::GetHandleHalfWidth() {
    return (float)(s_handleWidth / 2);
}
constexpr float Slider::GetHandleHalfLength() {
    return (float)(s_handleLength / 2);
}

Vector2 Slider::GetHandleExtents() {
    return {
        GetHandleWidth(),
        GetHandleLength()
    };
}
Vector2 Slider::GetHandleHalfSize() {
    return {
        GetHandleHalfWidth(),
        GetHandleHalfLength()
    };
}

float Slider::GetHandleX() const {
    return GetTrackStartX() + RangeRemap(GetValue(), GetRangeMin(), GetRangeMax(), 0.0f, GetTrackLength());
}
float Slider::GetHandleY() const {
    return GetTrackStartY();
}
Vector2 Slider::GetHandlePosition() const {
    return {
        GetHandleX(),
        GetHandleY()
    };
}

Rectangle Slider::GetHandleRect() const {
    return Rectangle{
        GetHandleX() - GetHandleHalfWidth(),
        GetHandleY() - GetHandleHalfLength(),
        GetHandleWidth(),
        GetHandleLength(),
    };
}
Rectangle Slider::GetHandleInsetRect() const {
    return Rectangle{
        GetHandleX() - GetHandleHalfWidth() + 1.0f,
        GetHandleY() - GetHandleHalfLength() + 1.0f,
        GetHandleWidth() - 2.0f,
        GetHandleLength() - 2.0f,
    };
}

bool Slider::IsHandleHovered() const {
    return CheckCollisionPointRec(
        GetMousePosition(),
        GetHandleRect()
    );
}

// Flags

bool Slider::IsHorizontal() const {
    return b_horizontal;
}
bool Slider::IsVisible() const {
    return b_visible;
}
bool Slider::IsBeingHeld() const {
    return b_beingHeld;
}
bool Slider::IsEnabled() const {
    return b_enabled;
}

bool Slider::HasDefaultValue() const {
    return !isnan(m_default);
}
bool Slider::HasIncrement() const {
    return m_incr > 0.0f;
}

// Range

float Slider::GetValue() const {
    return m_value;
}
float Slider::GetRangeMin() const {
    return m_min;
}
float Slider::GetRangeMax() const {
    return m_max;
}
float Slider::GetDefaultValueTrackValue() const {
    return RangeRemap(m_default, GetRangeMin(), GetRangeMax(), GetTrackStart(), GetTrackEnd());
}
Vector2 Slider::GetDefaultValueTrackPosition() const {
    if (IsHorizontal())
        return { RangeRemap(m_default, GetRangeMin(), GetRangeMax(), GetTrackStartX(), GetTrackEndX()), GetTrackStartY() };
    else
        return { GetTrackStartX(), RangeRemap(m_default, GetRangeMin(), GetRangeMax(), GetTrackStartY(), GetTrackEndY()) };
}
float Slider::GetIncrementValue() const {
    return m_incr;
}
float Slider::SnapToIncrement(float value) const {
    float r = roundf(value / GetIncrementValue());
    return r * GetIncrementValue();
}

// Display

float Slider::GetTrackLength() const {
    return m_length;
}
float Slider::GetTrackStartX() const {
    return m_position.x;
}
float Slider::GetTrackStartY() const {
    return m_position.y;
}
float Slider::GetTrackStart() const {
    if (IsHorizontal())
        return GetTrackStartX();
    else
        return GetTrackStartY();
}

float Slider::GetTrackEndX() const {
    if (IsHorizontal())
        return GetTrackStartX() + GetTrackLength();
    else
        return GetTrackStartX();
}
float Slider::GetTrackEndY() const {
    if (IsHorizontal())
        return GetTrackStartY();
    else
        return GetTrackStartY() + GetTrackLength();
}
float Slider::GetTrackEnd() const {
    if (IsHorizontal())
        return GetTrackEndX();
    else
        return GetTrackEndY();
}

Vector2 Slider::GetTrackStartPosition() const {
    return m_position;
}
Vector2 Slider::GetTrackEndPosition() const {
    if (IsHorizontal())
        return { GetTrackStartX() + GetTrackLength(), GetTrackStartY() };
    else
        return { GetTrackStartX(), GetTrackStartY() + GetTrackLength() };
}

Color Slider::GetHandleColor() const {
    if (b_enabled)
    {
        if (b_beingHeld)
            return s_heldColor;
        else
            return s_enabledColor;
    }
    else
        return s_disabledColor;
}


void Slider::UpdateDrag() {
    float mousePos = (IsHorizontal() ? GetMouseX() : GetMouseY());

    // Snap to increment
    if (HasIncrement()) {
        float val = RangeRemap(mousePos, GetTrackStart(), GetTrackEnd(), GetRangeMin(), GetRangeMax());
        val = SnapToIncrement(val);
        mousePos = RangeRemap(val, GetRangeMin(), GetRangeMax(), GetTrackStart(), GetTrackEnd());
    }
    // Snap to default position
    if (HasDefaultValue()) {
        float defPos = GetDefaultValueTrackValue();
        if (abs(defPos - mousePos) < GetHandleHalfWidth())
            mousePos = defPos;
    }

    // Clamp to bounds
    float screenMin = RangeRemap(GetRangeMin(), GetRangeMin(), GetRangeMax(), GetTrackStart(), GetTrackEnd());
    float screenMax = RangeRemap(GetRangeMax(), GetRangeMin(), GetRangeMax(), GetTrackStart(), GetTrackEnd());
    mousePos = Clamp(mousePos, screenMin, screenMax);

    m_value = RangeRemap(mousePos, GetTrackStart(), GetTrackEnd(), GetRangeMin(), GetRangeMax());
}
void Slider::Update(Slider* array, size_t count) {
    static Slider* held = nullptr;

    if (!held && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (size_t i = 0; i < count; ++i)
        {
            if (!array[i].b_enabled || !array[i].b_visible)
                continue;

            if (array[i].IsHandleHovered()) {
                held = array + i;
                break;
            }
        }
    }
    else if (held && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        held->b_beingHeld = false;
        held = nullptr;
    }
    if (held) {
        held->b_beingHeld = true;
        held->UpdateDrag();
    }
}
void Slider::Update(std::vector<Slider>& array) {
    static Slider* held = nullptr;

    if (!held && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (size_t i = 0; i < array.size(); ++i)
        {
            if (!array[i].b_enabled || !array[i].b_visible)
                continue;

            if (array[i].IsHandleHovered()) {
                held = &(array[i]);
                break;
            }
        }
    }
    else if (held && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        held->b_beingHeld = false;
        held = nullptr;
    }
    if (held) {
        held->b_beingHeld = true;
        held->UpdateDrag();
    }
}
void Slider::Update(std::vector<Slider*>& array) {
    static Slider* held = nullptr;

    if (!held && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (size_t i = 0; i < array.size(); ++i)
        {
            if (!array[i]->b_enabled || !array[i]->b_visible)
                continue;

            if (array[i]->IsHandleHovered()) {
                held = array[i];
                break;
            }
        }
    }
    else if (held && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        held->b_beingHeld = false;
        held = nullptr;
    }
    if (held) {
        held->b_beingHeld = true;
        held->UpdateDrag();
    }
}

void Slider::Draw() const {
    // Track
    DrawLineV(GetTrackStartPosition(), GetTrackEndPosition(), s_trackColor);

    Vector2 offsetStart;
    Vector2 offsetEnd;
    if (IsHorizontal()) {
        offsetStart = { 0.0f, 2.0f };
        offsetEnd = { 0.0f, GetHandleHalfLength() * (2.0f / 3.0f) };
    }
    else {
        offsetStart = { 2.0f, 0.0f };
        offsetEnd = { GetHandleHalfLength() * (2.0f / 3.0f), 0.0f };
    }

    // Boundary markers
#if 1
    DrawLineV(GetTrackStartPosition() - offsetEnd, GetTrackStartPosition() + offsetEnd, s_markerColor);
    DrawLineV(GetTrackEndPosition() - offsetEnd, GetTrackEndPosition() + offsetEnd, s_markerColor);
#endif

    // Increment markers
    if (HasIncrement()) {
        Vector2 p = GetTrackStartPosition();
        if (IsHorizontal()) {
            for (float f = GetRangeMin() + GetIncrementValue(); f < GetRangeMax(); f += GetIncrementValue()) {
                p.x = RangeRemap(f, GetRangeMin(), GetRangeMax(), GetTrackStartX(), GetTrackEndX());
                DrawLineV(p - offsetStart, p - offsetEnd, s_markerColor);
            }
        }
        else {
            for (float f = GetRangeMin() + GetIncrementValue(); f < GetRangeMax(); f += GetIncrementValue()) {
                p.y = RangeRemap(f, GetRangeMin(), GetRangeMax(), GetTrackStartY(), GetTrackEndY());
                DrawLineV(p - offsetStart, p - offsetEnd, s_markerColor);
            }
        }
    }

    // Default marker
    if (HasDefaultValue()) {
        Vector2 pos = GetDefaultValueTrackPosition();
        DrawLineV(pos + offsetStart, pos + offsetEnd, s_markerColor);
    }

    DrawRectangleRec(GetHandleRect(), s_disabledColor); // Outline
    DrawRectangleRec(GetHandleInsetRect(), GetHandleColor()); // Inside
}
void Slider::Draw(Slider* array, size_t count) {
    for (size_t i = 0; i < count; ++i) {

        if (!array[i].b_visible)
            continue;

        array[i].Draw();
    }
}
void Slider::Draw(std::vector<Slider>& array) {
    for (Slider& slider : array) {

        if (!slider.b_visible)
            continue;

        slider.Draw();
    }
}
void Slider::Draw(std::vector<Slider*>& array) {
    for (Slider* slider : array) {

        if (!slider->b_visible)
            continue;

        slider->Draw();
    }
}

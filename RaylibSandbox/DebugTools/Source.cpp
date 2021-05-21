#include <raylib.h>
#include <raymath.h>
#include <assert.h>
#include <type_traits>

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

#pragma region Range math

float ScaleFromRange(float min, float max) { return max - min; }
float RangeToScalar(float value, float from_min, float from_max) { return ((value - from_min) / ScaleFromRange(from_min, from_max)); }
float RangeFromScalar(float value, float to_min, float to_max) { return (value * ScaleFromRange(to_min, to_max)) + to_min; }
float RangeRemap(float value, float from_min, float from_max, float to_min, float to_max) { return (((value - from_min) / ScaleFromRange(from_min, from_max)) * ScaleFromRange(to_min, to_max)) + to_min; }

Vector2 ScaleFromRangeVector2(Vector2 min, Vector2 max) { return max - min; }
Vector2 RangeToScalarVector2(Vector2 value, Vector2 from_min, Vector2 from_max) { return ((value - from_min) / ScaleFromRangeVector2(from_min, from_max)); }
Vector2 RangeFromScalarVector2(Vector2 value, Vector2 to_min, Vector2 to_max) { return (value * ScaleFromRangeVector2(to_min, to_max)) + to_min; }
Vector2 RangeRemapVector2(Vector2 value, Vector2 from_min, Vector2 from_max, Vector2 to_min, Vector2 to_max) { return (((value - from_min) / ScaleFromRangeVector2(from_min, from_max)) * ScaleFromRangeVector2(to_min, to_max)) + to_min; }

Vector3 ScaleFromRangeVector3(Vector3 min, Vector3 max) { return max - min; }
Vector3 RangeToScalarVector3(Vector3 value, Vector3 from_min, Vector3 from_max) { return ((value - from_min) / ScaleFromRangeVector3(from_min, from_max)); }
Vector3 RangeFromScalarVector3(Vector3 value, Vector3 to_min, Vector3 to_max) { return (value * ScaleFromRangeVector3(to_min, to_max)) + to_min; }
Vector3 RangeRemapVector3(Vector3 value, Vector3 from_min, Vector3 from_max, Vector3 to_min, Vector3 to_max) { return (((value - from_min) / ScaleFromRangeVector3(from_min, from_max)) * ScaleFromRangeVector3(to_min, to_max)) + to_min; }

struct Range { float min, max; };
float ScaleFromRangeR(Range range) { return range.max - range.min; }
float RangeToScalarR(float value, Range from) { return ((value - from.min) / ScaleFromRangeR(from)); }
float RangeFromScalarR(float value, Range to) { return (value * ScaleFromRangeR(to)) + to.min; }
float RangeRemapR(float value, Range from, Range to) { return (((value - from.min) / ScaleFromRangeR(from)) * ScaleFromRangeR(to)) + to.min; }

#pragma endregion

#pragma region Buttons

enum ButtonStatus {
    status_disabled,
    status_normal,
    status_hovered,
    status_held,

    status_hidden,
};
#define BUTTON_UPDATABLE_FLAGS 0b11
enum ButtonType {
    button_hold,
    button_toggle,
};

struct Slider;

struct Button {
    Rectangle shape;
    Color color[4];
    ButtonStatus status = status_normal;
    ButtonType type = button_hold;
    Slider* parent = nullptr;
    bool value = false; // Toggles
    bool lastValue = false; // Value before change
};
void CreateButton(
    Button* button,
    Rectangle shape,
    Color disabledColor, Color normalColor, Color hoveredColor, Color heldColor,
    ButtonType type,
    bool defaultValue, bool startDisabled)
{
    *button = { shape, { disabledColor, normalColor, hoveredColor, heldColor }, (startDisabled ? status_disabled : status_normal), type, nullptr, defaultValue, defaultValue };
}

bool IsDebugButtonValueUnchanged(Button* button)
{
    return button->lastValue == button->value;
}
bool IsDebugButtonValueChanged(Button* button)
{
    return button->lastValue != button->value;
}
bool IsDebugButtonActivated(Button* button)
{
    return !button->lastValue && button->value;
}
bool IsDebugButtonDeactivated(Button* button)
{
    return button->lastValue && !button->value;
}
bool IsDebugButtonActive(Button* button)
{
    return button->value;
}
bool IsDebugButtonInactive(Button* button)
{
    return !button->value;
}

Button* g_heldButton = nullptr;

void UpdateDebugButton(Button* button)
{
    button->lastValue = button->value;

    if (!g_heldButton && (button->status & BUTTON_UPDATABLE_FLAGS) && (button->status == status_held ? IsMouseButtonReleased(MOUSE_LEFT_BUTTON) : true))
    {
        if (CheckCollisionPointRec(GetMousePosition(), button->shape))
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                button->status = status_held;

                if (button->type == button_toggle)
                    button->value = !button->value;

                g_heldButton = button;
            }
            else
                button->status = status_hovered;
        }
        else
            button->status = status_normal;

        if (button->type == button_hold)
            button->value = button->status == status_held;
    }
}
void UpdateDebugButtonArray(Button* buttonArray, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        buttonArray[i].lastValue = buttonArray[i].value;
    }

    if (g_heldButton && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    {
        g_heldButton->status = status_normal;
        g_heldButton = nullptr;
    }

    if (!g_heldButton)
    {
        bool mouseDown = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

        for (size_t i = 0; i < size; ++i)
        {
            if (!(buttonArray[i].status & BUTTON_UPDATABLE_FLAGS))
                continue;

            if (CheckCollisionPointRec(GetMousePosition(), buttonArray[i].shape))
            {
                buttonArray[i].status = status_hovered;
                if (mouseDown)
                    g_heldButton = buttonArray + i; // Yes, this will get overwritten if there are overlapping buttons.
            }
            else
                buttonArray[i].status = status_normal;

            if (buttonArray[i].type == button_hold)
                buttonArray[i].value = false;
        }
        if (g_heldButton)
        {
            g_heldButton->status = status_held;
            g_heldButton->value = (g_heldButton->type == button_toggle ? !g_heldButton->value : true);
        }
    }
}

void DrawDebugButton(Button* button)
{
    Color color = button->color[button->status];
    DrawRectangleRec(button->shape, color);
    if (button->type == button_toggle && button->value)
    {
        DrawRectangleLinesEx(button->shape, 2, button->color[status_held]);
    }
}
void DrawDebugButtonArray(Button* buttonArray, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        DrawRectangleRec(buttonArray[i].shape, buttonArray[i].color[buttonArray[i].status]);
        if (buttonArray[i].type == button_toggle && buttonArray[i].value)
        {
            DrawRectangleLinesEx(buttonArray[i].shape, 2, buttonArray[i].color[status_held]);
        }
    }
}

void DisableDebugButton(Button* button)
{
    button->status = status_disabled;
}
void EnableDebugButton(Button* button)
{
    button->status = status_normal;
}

#pragma endregion

#pragma region Sliders

//template<typename T>
//struct Slider {};


struct Slider
{
    Button handle;
    float value;
    Range valueRange;
    float trackLength;
    float lockX;
    float snapGrid = 0.0f;
};

void CreateSlider(
    Slider* slider,
    float min, float max,
    float trackLength,
    float snap,
    Vector2 position,
    Vector2 handleExtents,
    Color disabledColor, Color normalColor, Color hoveredColor, Color heldColor,
    float defaultValue, bool startDisabled)
{
    *slider = {
        {},
        defaultValue,
        { min, max },
        trackLength,
        position.x,
        snap
    };
    slider->handle = Button{
            { RangeRemapR(slider->value, slider->valueRange, { 0.0f, trackLength }), position.y, handleExtents.x, handleExtents.y },
            { disabledColor, normalColor, hoveredColor, heldColor },
            (startDisabled ? status_disabled : status_normal),
            button_hold,
            slider,
            false, false
    };
}

float Snap(float value, float grid)
{
    return roundf(value / grid) * grid;
}

void UpdateDebugSlider(Slider* slider)
{
    UpdateDebugButton(&slider->handle);
    if (slider->handle.status == status_held)
    {
        float xPos = Clamp(GetMouseX() - slider->handle.shape.width * 0.5f, slider->lockX, slider->lockX + slider->trackLength);

        if (slider->snapGrid > 0.0f)
            xPos = Snap(xPos, RangeRemap(slider->snapGrid, slider->lockX, slider->lockX + slider->trackLength, slider->valueRange.min, slider->valueRange.max));

        slider->value = DebugSliderTrackPositionToRange(slider, slider->handle.shape.x = xPos);
    }
}
void UpdateDebugSliderArray(Slider* sliderArray, size_t count)
{
    Slider* heldSlider = nullptr;
    for (size_t i = 0; i < count; ++i)
    {
        UpdateDebugButton(&sliderArray[i].handle);
        if (sliderArray[i].handle.status == status_held)
            heldSlider = sliderArray + i;
    }
    if (heldSlider)
    {
        float xPos = Clamp(GetMouseX(), heldSlider->lockX, heldSlider->lockX + heldSlider->trackLength);

        if (heldSlider->snapGrid > 0.0f)
            xPos = DebugSliderRangeToTrackPosition(heldSlider, Snap(DebugSliderTrackPositionToRange(heldSlider, xPos), heldSlider->snapGrid));

        heldSlider->handle.shape.x = xPos - heldSlider->handle.shape.width * 0.5f;

        heldSlider->value = DebugSliderTrackPositionToRange(heldSlider, xPos);
    }
}

void DrawDebugSlider(Slider* slider)
{
    Vector2 center = Vector2{ slider->handle.shape.width, slider->handle.shape.height } *0.5f;
    Vector2 start = Vector2{ slider->lockX, slider->handle.shape.y } + center;
    Vector2 end = Vector2{ (slider->lockX + slider->trackLength), slider->handle.shape.y } + center;
    DrawLineV(start, end, GRAY);
    Vector2 capOffset = { 0.0f, slider->handle.shape.height * 0.25f };
    DrawLineV(start + capOffset, start - capOffset, GRAY);
    DrawLineV(end + capOffset, end - capOffset, GRAY);

    DrawRectangleRec(slider->handle.shape, slider->handle.color[slider->handle.status]);
}
void DrawDebugSliderArray(Slider* sliderArray, size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        Vector2 center = Vector2{ sliderArray[i].handle.shape.width, sliderArray[i].handle.shape.height } *0.5f;
        Vector2 start = Vector2{ sliderArray[i].lockX, sliderArray[i].handle.shape.y } + center;
        Vector2 end = Vector2{ (sliderArray[i].lockX + sliderArray[i].trackLength), sliderArray[i].handle.shape.y } + center;
        DrawLineV(start, end, GRAY);
        Vector2 capOffset = { 0.0f, sliderArray[i].handle.shape.height * 0.25f };
        DrawLineV(start + capOffset, start - capOffset, GRAY);
        DrawLineV(end + capOffset, end - capOffset, GRAY);

        if (sliderArray[i].snapGrid > 0.0f)
        {
            Vector2 p;
            for (p = start; p.x <= end.x; p.x += DebugSliderRangeToTrackScale(sliderArray + i, sliderArray[i].snapGrid))
            {
                if (p.x < start.x || p.x > end.x)
                    continue;

                DrawPixelV(p, LIGHTGRAY);
            }
        }

        DrawRectangleRec(sliderArray[i].handle.shape, sliderArray[i].handle.color[sliderArray[i].handle.status]);
        DrawText(TextFormat("%1.3f", sliderArray[i].value), (int)end.x + 8, (int)end.y - 4, 8, WHITE);
    }
}

void DisableDebugSlider(Slider* slider)
{
    slider->handle.status = status_disabled;
}
void EnableDebugSlider(Slider* slider)
{
    slider->handle.status = status_normal;
}

#pragma endregion

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    float value = 0.0f;

    Button buttons[3];
    CreateButton(buttons + 0, {  20, 20, 64, 32 }, GRAY, DARKBLUE,  BLUE,  SKYBLUE, button_toggle, false, false);
    CreateButton(buttons + 1, { 104, 20, 64, 32 }, GRAY, DARKGREEN, LIME,   GREEN,  button_toggle, false, false);
    CreateButton(buttons + 2, { 187, 20, 64, 32 }, GRAY, RED,       ORANGE, YELLOW, button_hold,   false, true);

    Slider sliders[3];
    CreateSlider(sliders + 0,  0.0f, 1.0f, 100.0f, 0.0f, { 20, 100 }, { 6, 16 }, GRAY, DARKBLUE,  BLUE,   SKYBLUE, 0.5f,  false);
    CreateSlider(sliders + 1, -2.0f, 3.0f, 120.0f, 1.0f, { 20, 124 }, { 6, 16 }, GRAY, DARKGREEN, LIME,   GREEN,   0.25f, false);
    CreateSlider(sliders + 2, -5.0f, 5.0f, 120.0f, 1.0f, { 20, 148 }, { 6, 16 }, GRAY, RED,       ORANGE, YELLOW,  0.75f, true);

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        UpdateDebugButtonArray(buttons, _countof(buttons));

        if (IsDebugButtonActivated(buttons + 0))
        {
            EnableDebugButton(buttons + 2);
        }
        else if (IsDebugButtonDeactivated(buttons + 0))
        {
            DisableDebugButton(buttons + 2);
        }
        if (IsDebugButtonActivated(buttons + 1))
        {
            EnableDebugSlider(sliders + 2);
        }
        else if (IsDebugButtonDeactivated(buttons + 1))
        {
            DisableDebugSlider(sliders + 2);
        }
        
        UpdateDebugSliderArray(sliders, _countof(sliders));

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            DrawDebugButtonArray(buttons, _countof(buttons));
            DrawDebugSliderArray(sliders, _countof(sliders));

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    // @TODO: Unload variables

    CloseWindow();

	return 0;
}
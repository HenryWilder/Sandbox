#include <raylib.h>
#include <raymath.h>
#include <assert.h>
#include <type_traits>
#include <vector>
#include "Slider.h"

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

    std::vector<Slider> sliders;
    sliders.reserve(3);

    SliderData slider;

    slider.position = { 20, 20 };
    slider.min = -3.0f;
    slider.max = 3.0f;
    slider.increment = 0.0f;
    sliders.push_back(Slider(slider));

    slider.position.y = 40;
    slider.min = -3.0f;
    slider.max = 10.0f;
    slider.increment = 1.0f;
    sliders.push_back(Slider(slider));

    slider.position.y = 60;
    slider.min = 1.0f;
    slider.max = 8.0f;
    slider.increment = 0.0f;
    slider.defaultValue = 2.0f;
    sliders.push_back(Slider(slider));

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        Slider::Update(sliders);

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            Slider::Draw(sliders);

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    // @TODO: Unload variables

    CloseWindow();

	return 0;
}
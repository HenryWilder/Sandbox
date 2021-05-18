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

enum ButtonStatus {
    status_disabled,
    status_normal,
    status_hovered,
    status_held,
};

struct Button {
    Rectangle shape;
    Color color[4];
    ButtonStatus status = status_normal;
    bool value = false; // Toggles
};
Button* g_heldButton = nullptr;

void UpdateDebugButton(Button* button)
{
    if (!g_heldButton && button->status && (button->status == status_held ? IsMouseButtonReleased(MOUSE_LEFT_BUTTON) : true))
    {
        if (CheckCollisionPointRec(GetMousePosition(), button->shape))
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                button->status = status_held;
                button->value = !button->value;
                g_heldButton = button;
            }
            else
                button->status = status_hovered;
        }
        else
            button->status = status_normal;
    }
}

void DrawDebugButton(Button* button)
{
    Color color = button->color[button->status];
    DrawRectangleRec(button->shape, color);
}

void SetButtonPosition(Button* button, float x, float y)
{
    button->shape.x = x;
    button->shape.y = y;
}

void UpdateDebugButtonArray(Button* buttonArray, size_t size)
{
    if (g_heldButton && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    {
        g_heldButton->status = status_normal;
        g_heldButton = nullptr;
    }
    if (!g_heldButton)
    {
        bool mouseDown = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        {
            for (size_t i = 0; i < size; ++i)
            {
                if (buttonArray[i].status == status_disabled)
                    continue;

                if (CheckCollisionPointRec(GetMousePosition(), buttonArray[i].shape))
                {
                    buttonArray[i].status = status_hovered;
                    if (mouseDown)
                        g_heldButton = buttonArray + i; // Yes, this will get overwritten if there are overlapping buttons.
                }
                else
                    buttonArray[i].status = status_normal;
            }
            if (g_heldButton)
            {
                g_heldButton->status = status_held;
                g_heldButton->value = !g_heldButton->value;
            }
        }
    }
}

void DrawDebugButtonArray(Button* buttonArray, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        DrawRectangleRec(buttonArray[i].shape, buttonArray[i].color[buttonArray[i].status]);
    }
}

struct Slider
{
    Button handle;
    float value;
    float min, max;
    float trackLength;
    float lockX;
};

float DebugSliderTrackToRange(Slider* slider, float position)
{
    return (((position - slider->lockX) / slider->trackLength) * (slider->max - slider->min)) + slider->min;
}
float DebugSliderRangeToTrack(Slider* slider, float value)
{
    return (((value - slider->min) / (slider->max - slider->min)) * slider->trackLength) + slider->lockX;
}

void InitDebugSlider(Slider* slider)
{
    SetButtonPosition(&slider->handle, DebugSliderRangeToTrack(slider, slider->value), slider->handle.shape.y);
}
void UpdateDebugSlider(Slider* slider)
{
    UpdateDebugButton(&slider->handle);
    if (slider->handle.status == status_held)
    {
        float xPos = Clamp(GetMouseX() - slider->handle.shape.width * 0.5f, slider->lockX, slider->lockX + slider->trackLength);
        SetButtonPosition(&slider->handle, xPos, slider->handle.shape.y);
        slider->value = DebugSliderTrackToRange(slider, xPos);
    }
}
void DrawDebugSlider(Slider* slider)
{
    Vector2 center = Vector2{ slider->handle.shape.width, slider->handle.shape.height } * 0.5f;
    Vector2 start = Vector2{ slider->lockX, slider->handle.shape.y } + center;
    Vector2 end = Vector2{ (slider->lockX + slider->trackLength), slider->handle.shape.y } + center;
    DrawLineV(start, end, GRAY);
    Vector2 capOffset = { 0.0f, slider->handle.shape.height * 0.25f };
    DrawLineV(start + capOffset, start - capOffset, GRAY);
    DrawLineV(end + capOffset, end - capOffset, GRAY);
    DrawDebugButton(&slider->handle);
}

void InitDebugSliderArray(Slider* sliderArray, size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        InitDebugSlider(sliderArray + i);
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
        float xPos = Clamp(GetMouseX() - heldSlider->handle.shape.width * 0.5f, heldSlider->lockX, heldSlider->lockX + heldSlider->trackLength);
        SetButtonPosition(&heldSlider->handle, xPos, heldSlider->handle.shape.y);
        heldSlider->value = DebugSliderTrackToRange(heldSlider, xPos);
    }
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
        DrawDebugButton(&sliderArray[i].handle);
        DrawText(TextFormat("%1.3f", sliderArray[i].value), (int)end.x + 8, (int)end.y - 4, 8, WHITE);
    }
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

    float value = 0.0f;

    Button buttons[] =
    {
        Button{ {  20, 20, 64, 32 }, { GRAY, DARKBLUE, BLUE, SKYBLUE }, status_normal },
        Button{ { 104, 20, 64, 32 }, { GRAY, DARKGREEN, LIME, GREEN }, status_normal },
        Button{ { 187, 20, 64, 32 }, { GRAY, RED, ORANGE, YELLOW }, status_disabled },
    };
    Slider sliders[] =
    {
        Slider{ Button{ {  20, 100, 6, 16 }, { GRAY, DARKBLUE, BLUE, SKYBLUE }, status_normal }, 0.5f, 0.0f, 1.0f, 100.0f, 20.0f },
        Slider{ Button{ {  20, 124, 6, 16 }, { GRAY, DARKGREEN, LIME, GREEN }, status_normal }, 0.25f, -2.0f, 3.0f, 120.0f, 20.0f },
        Slider{ Button{ {  20, 148, 6, 16 }, { GRAY, RED, ORANGE, YELLOW }, status_disabled }, 0.75f, -5.0f, 5.0f, 120.0f, 20.0f },
    };
    InitDebugSliderArray(sliders, _countof(sliders));

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        UpdateDebugButtonArray(buttons, _countof(buttons));
        if (buttons[0].value && !sliders[2].handle.status)
        {
            sliders[2].handle.status = status_normal;
        }
        else if (!buttons[0].value)
        {
            sliders[2].handle.status = status_disabled;
        }
        if (buttons[1].value && !buttons[2].status)
        {
            buttons[2].status = status_normal;
        }
        else if (!buttons[1].value)
        {
            buttons[2].status = status_disabled;
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
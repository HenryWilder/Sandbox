#include <raylib.h>
#include <raymath.h>

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

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    float radius = 200.0f;
    float density1 = 0.5f;
    float density2 = 0.5f;
    bool density2Active = false;
    Rectangle radiusSlider;
    Rectangle density1Slider;
    Rectangle density2Slider;
    Rectangle density2SliderCheckbox;

    density2Slider.width = density1Slider.width = radius;
    density2SliderCheckbox.width = density2SliderCheckbox.height = density2Slider.height = 25;
    (density2Slider.y = density2SliderCheckbox.y = windowHeight - ((density2SliderCheckbox.x = 10) + density2SliderCheckbox.height));
    density2Slider.x = density2SliderCheckbox.x + density2SliderCheckbox.width + 10;
    density1Slider = density2Slider;
    density1Slider.y -= (density1Slider.height + 10);
    radiusSlider = density1Slider;
    radiusSlider.y -= (radiusSlider.height + 10);

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            if (CheckCollisionPointRec(GetMousePosition(), density1Slider))
            {
                density1 = ((float)GetMouseX() - density1Slider.x) / density1Slider.width;
                if (!density2Active) density2 = density1;
            }
            else if (density2Active && CheckCollisionPointRec(GetMousePosition(), density2Slider))
            {
                density2 = ((float)GetMouseX() - density2Slider.x) / density2Slider.width;
            }
            else if (CheckCollisionPointRec(GetMousePosition(), Rectangle{ 0, radiusSlider.y, (float)windowWidth, radiusSlider.height }))
            {
                float max = ((float)windowWidth - radiusSlider.x * 2) - 10.0f * 2;

                radius = ((float)GetMouseX() - radiusSlider.x);
                if (radius < 10) radius = 10;
                else if (radius > max) radius = max;
            }
        }
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), density2SliderCheckbox))
        {
            density2Active = !density2Active;
            if (!density2Active) density2 = density1;
        }

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            DrawTextureEx(windowWidth / 2 - radius, windowHeight / 2 - radius, radius * 2, radius * 2, WHITE);

            /******************************************
            *   UI                                    *
            ******************************************/

            Rectangle fillBar;

            // Radius Slider
            fillBar = radiusSlider;
            fillBar.width = radius;
            DrawRectangleRec(fillBar, BLUE);
            DrawText("Radius", radiusSlider.x + 2, radiusSlider.y + 2, 8, WHITE);

            // Density 1 Slider
            DrawRectangleRec(density1Slider, WHITE);
            fillBar = density1Slider;
            fillBar.width = density1 * density1Slider.width;
            DrawRectangleRec(fillBar, BLUE);
            DrawText("Outside Density", density1Slider.x + 2, density1Slider.y + 2, 8, BLACK);

            // Density 2 Slider
            DrawRectangleRec(density2Slider, (density2Active ? WHITE : GRAY));
            fillBar = density2Slider;
            fillBar.width = density2 * density2Slider.width;
            DrawRectangleRec(fillBar, BLUE);
            DrawText("Inside Density", density2Slider.x + 2, density2Slider.y + 2, 8, BLACK);

            // Density 2 Checkbox
            DrawRectangleRec(density2SliderCheckbox, (density2Active ? BLUE : GRAY));

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    // @TODO: Unload variables

    CloseWindow();

	return 0;
}
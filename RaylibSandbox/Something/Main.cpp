#include <raylib.h>
#pragma warning(push)
#pragma warning(disable : 26451)
#include <raymath.h>
#pragma warning(pop)
#include <algorithm>
#include <functional>
#include "HUI.h"

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

enum class GameMode
{
    Exit,
    Menu,
    ModeA,
    ModeB,
};

// Returns true when program should exit
GameMode MenuScreen()
{
    Button button_A;
    button_A.SetDisplayName("Game");
    button_A.SetRect({ 20, 20, 60, 20 });
    
    Button button_B;
    button_B.SetDisplayName("Test");
    button_B.SetRect({ 20, 20, 60, 20 });
    button_B.OffsetFrom(button_A, Spacing::OVERLAP, 0.0f, Spacing::PAD, 10.0f);

    Button button_close;
    button_close.SetDisplayName("Close");
    button_close.CopyShape(button_A);
    button_close.OffsetFrom(button_B, Spacing::OVERLAP, 0.0f, Spacing::PAD, 10.0f);

    UIHandler::Global().Expect(3);
    UIHandler::Global().AddButton_New(&button_A);
    UIHandler::Global().AddButton_New(&button_B);
    UIHandler::Global().AddButton_New(&button_close);

    while (true)
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        UIHandler::Global().UpdateCursorPos(GetMousePosition());
        UIHandler::Global().UpdateButtons();

        if (button_A.IsActive())
        {
            UIHandler::Global().Unload();
            return GameMode::ModeA;
        }
        if (button_B.IsActive())
        {
            UIHandler::Global().Unload();
            return GameMode::ModeB;
        }

        if (button_close.IsActive())
            return GameMode::Exit;

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(RAYWHITE);

            UIHandler::Global().Draw();

        } EndDrawing();

        if (WindowShouldClose())
            return GameMode::Exit;
    }
    return GameMode::Exit; // Just in case
}


// Returns true when program should exit
GameMode GameScreen()
{
    Button button_Hold;
    button_Hold.SetDisplayName("Hold");
    button_Hold.SetToolTip("Stays active only while the mouse is pressed.");
    button_Hold.SetShape(60,20);
    button_Hold.SetPosition(20,20);

    Button button_Toggle;
    button_Toggle.SetDisplayName("Toggle");
    button_Toggle.SetToolTip("Changes states when the mouse is pressed.");
    button_Toggle.SetToggle(true);
    button_Toggle.CopyShape(button_Hold);
    button_Toggle.OffsetFrom(button_Hold, Spacing::OVERLAP, 0, Spacing::PAD, 10);

    Button button_DragV;
    button_DragV.SetDisplayName("DragV");
    button_DragV.SetToolTip("Able to be dragged while held.");
    button_DragV.CopyShape(button_Hold);
    button_DragV.OffsetFrom(button_Toggle, Spacing::OVERLAP, 0, Spacing::PAD, 10);

    Button button_DragH;
    button_DragH.SetDisplayName("DragH");
    button_DragH.SetToolTip("Able to be dragged while held.");
    button_DragH.CopyShape(button_Hold);
    button_DragH.OffsetFrom(button_DragV, Spacing::OVERLAP, 0, Spacing::PAD, 10);

    RadioButtonHandler group0;

    Button button_A;
    button_A.SetDisplayName("A");
    button_A.SetToolTip("Only one of these three can be active at a time.");
    button_A.SetToggle(true);
    button_A.SetShape(20,20);
    button_A.OffsetFrom(button_Hold, Spacing::PAD, 10, Spacing::OVERLAP, 0);

    Button button_B;
    button_B.SetDisplayName("B");
    button_B.SetToolTip(button_A.GetToolTip());
    button_B.SetToggle(true);
    button_B.CopyShape(button_A);
    button_B.OffsetFrom(button_A, Spacing::OVERLAP, 0, Spacing::PAD, 10);

    Button button_C;
    button_C.SetDisplayName("C");
    button_C.SetToolTip(button_A.GetToolTip().text + " Also, this one moves with the one on its left!");
    button_C.SetToggle(true);
    button_C.CopyShape(button_A);
    button_C.OffsetFrom(button_B, Spacing::OVERLAP, 0, Spacing::PAD, 10);

    UIHandler::Global().Expect(7);
    UIHandler::Global().AddButton_New(&button_Hold);
    UIHandler::Global().AddButton_New(&button_Toggle);
    UIHandler::Global().AddButton_New(&button_DragV);
    UIHandler::Global().AddButton_New(&button_DragH);
    UIHandler::Global().CreateButtonGroup_FromNew(&group0, { &button_A, &button_B, &button_C });

    while (true)
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        UIHandler::Global().UpdateCursorPos(GetMousePosition());
        UIHandler::Global().UpdateButtons();

        // Handle 3rd button dragging
        if (button_DragV.IsActive())
        {
            float y = Clamp(GetMousePosition().y - button_DragV.GetHeight() * 0.5f, 80.0f, 280.0f);

            button_DragV.SetY(y);
            button_DragH.SetY(y + 30);
            button_C.SetY(y);
        }
        // Handle 3rd button dragging
        if (button_DragH.IsActive())
        {
            float x = Clamp(GetMousePosition().x - button_DragH.GetWidth() * 0.5f, 20.0f, 220.0f);

            button_DragH.SetX(x);
            button_C.SetX(x + 70);
        }

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(RAYWHITE);

            UIHandler::Global().Draw();

        } EndDrawing();

        if (WindowShouldClose())
            break;
    }

    UIHandler::Global().Unload();
    return GameMode::Menu;
}

// Returns true when program should exit
GameMode TestScreen()
{
    while (true)
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        UIHandler::Global().UpdateCursorPos(GetMousePosition());
        UIHandler::Global().UpdateButtons();

        // Update button actions

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(RAYWHITE);

            UIHandler::Global().Draw();

        } EndDrawing();

        if (WindowShouldClose())
            break;
    }

    UIHandler::Global().Unload();
    return GameMode::Menu;
}

typedef GameMode(*Screen)(void);

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    GameMode mode = GameMode::Menu;

    Screen screen[] =
    {
        MenuScreen,
        GameScreen,
        TestScreen,
    };

    while (mode != GameMode::Exit)
    {
        mode = screen[(int)mode - 1]();
    }

    CloseWindow();

	return 0;
}
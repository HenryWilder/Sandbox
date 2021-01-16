#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "Transistor.h"
#include "Wire.h"

struct Transistor;
struct Wire;

Vector2 Vector2Snap(const Vector2& vector, float gridSize)
{
    return Vector2{
        ((float)((int)((vector.x / gridSize) + 0.5f)) * gridSize),
        ((float)((int)((vector.y / gridSize) + 0.5f)) * gridSize)
    };
}

struct InputMode
{
    enum class Mode : char
    {
        None = 0,
        Wire,
        Gate,
    };
    Mode mode;

    union Data
    {
        WireDirection wireDirection;
    };
    Data data;
};

struct Undo
{
    Undo() : start{ }, end{ }, direction{ }, valid{ false } { };
    Undo(Vector2 _start, Vector2 _end, WireDirection _direction) : start{ _start }, end{ _end }, direction{ _direction }, valid{ true } { };
    Undo(Wire* wire) : start{ wire->startPos }, end{ wire->endPos }, direction{ wire->direction }, valid{ true } { };

    Vector2 start, end;
    WireDirection direction;
    bool valid;
};

int main(void)
{
    const int WindowWidth = 1280;
    const int WindowHeight = 720;

    std::vector<Wire*> wires;
    std::vector<Transistor*> transistors;

    Undo undoneAction = Undo();

    auto Vector2InvertY = [&WindowHeight](Vector2 vector) {
        return Vector2{ vector.x, WindowHeight - vector.y };
    };

    InitWindow(WindowWidth, WindowHeight, "Electron Architect");

    Vector2 cursorPos = { -100.0f, -100.0f };
    Vector2 wireStart = { -100.0f, -100.0f };

    InputMode mode = {};

    while (!WindowShouldClose())
    {
        // Update vars
        //---------------------------------------------
        cursorPos = GetMousePosition();
        cursorPos = Vector2Snap(cursorPos, 8.0f);

        switch (mode.mode)
        {
        case InputMode::Mode::None:
        {
            // Pressed
            // M1
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                wireStart = cursorPos;
                mode.mode = InputMode::Mode::Wire;
            }
            // M2
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
            {
                mode.mode = InputMode::Mode::Gate;
            }
        }
        break;

        case InputMode::Mode::Wire:
        {
            // Pressed
            // M2
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
            {
                mode.data.wireDirection = (WireDirection)(((char)mode.data.wireDirection + 1) % 3); // Should % the number of wire directions
            }

            // Released
            // M1
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                Wire* newWire = new Wire(wireStart, cursorPos, mode.data.wireDirection, &transistors);
                wires.push_back(newWire);

                mode.mode = InputMode::Mode::None;
            }
        }
        break;

        case InputMode::Mode::Gate:
        {
            // Pressed
            // M1
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                for (Transistor* transistor : transistors)
                {
                    if (Vector2Equal(cursorPos, transistor->pos))
                    {
                        DrawCircleV(cursorPos, 8.0f, YELLOW);
                        transistor->type = (Transistor::Type)(((int)(transistor->type) + 1) % 2); // Should % the number of transistor types
                        break;
                    }
                }
            }
            // M2
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
            {
                mode.mode = InputMode::Mode::None;
            }
        }
        break;

        }

        if (wires.size() && IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z))
        {
            if (IsKeyDown(KEY_LEFT_SHIFT)) // Redo
            {
                if (undoneAction.valid)
                {
                    Wire* newWire = new Wire(undoneAction.start, undoneAction.end, undoneAction.direction, &transistors);
                    wires.push_back(newWire);
                    undoneAction = Undo();
                }
            }
            else if (!IsKeyDown(KEY_LEFT_SHIFT)) // Undo
            {
                undoneAction = Undo(wires[wires.size() - 1]);
                delete wires[wires.size() - 1];
                wires.pop_back();
            }
        }

        for (Transistor* transistor : transistors) { transistor->Evaluate(); }

        // Draw
        //---------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);

        // Draw cursor
        {
            Color cursorColor = WHITE;
            if (mode.mode == InputMode::Mode::Wire) cursorColor = RED;
            else if (mode.mode == InputMode::Mode::Gate) cursorColor = YELLOW;

            DrawRectangleV({ cursorPos.x - 2, cursorPos.y - 2 }, { 4,4 }, cursorColor);
        }

        if (mode.mode == InputMode::Mode::Wire)
        {
            DrawSnappedLine(wireStart, cursorPos, GRAY, mode.data.wireDirection); // Temporary wire
        }

        if (mode.mode == InputMode::Mode::Gate)
        {
            
        }

        // Draw the current architecture
        for (Wire* wire : wires) { wire->Draw(); }

        for (Transistor* transistor : transistors) { transistor->Draw(); }

        EndDrawing();

        for (Transistor* transistor : transistors) { transistor->FrameReset(); }
    }

    // Return memory to heap
    //--------------------------

    for (Transistor* transistor : transistors)
    {
        delete transistor;
    }
    transistors.clear();

    for (Wire* wire : wires)
    {
        delete wire;
    }
    wires.clear();

    return 0;
}



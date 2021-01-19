#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "Transistor.h"
#include "Wire.h"
#include "Abstraction.h"
#include "Serialization.h"

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
        Move,
    };
    Mode mode;

    union Data
    {
        struct WireData {
            Wire* draggingWire;
            WireDirection wireDirection;
        };
        WireData wire;
        struct MoveData {
            Transistor* dragingTransistor;
        };
        MoveData move;
    };
    Data data;
};

struct Undo
{
    Undo() : start{ }, end{ }, direction{ }, valid{ false } { };
    Undo(Vector2 _start, Vector2 _end, WireDirection _direction) : start{ _start }, end{ _end }, direction{ _direction }, valid{ true } { };
    Undo(Wire* wire) : start{ wire->GetStartPos() }, end{ wire->GetEndPos() }, direction{ wire->direction }, valid{ true } { };

    Vector2 start, end;
    WireDirection direction;
    bool valid;
};

Rectangle RecVec2(Vector2 start, Vector2 end)
{
    Rectangle rec;
    rec.x = __min(start.x, end.x);
    rec.width = __max(start.x, end.x) - rec.x;
    rec.y = __min(start.y, end.y);
    rec.height = __max(start.y, end.y) - rec.y;
    return rec;
}

void SolderTransistorInput(Transistor* transistor, Wire* wire)
{
    transistor->inputs.push_back(wire);
    wire->outTransistor = transistor;
}
void BreakWireOutput(Wire* wire)
{
    Transistor* transistor = wire->outTransistor;
    for (size_t index = 0; index < transistor->inputs.size(); ++ index)
    {
        if (transistor->inputs[index] == wire)
        {
            transistor->inputs.erase(transistor->inputs.begin() + index);
            break;
        }
    }
    wire->outTransistor = nullptr;
}
void SolderTransistorOutput(Transistor* transistor, Wire* wire)
{
    transistor->outputs.push_back(wire);
    wire->inTransistor = transistor;
}
void BreakWireInput(Wire* wire)
{
    Transistor* transistor = wire->inTransistor;
    for (size_t index = 0; index < transistor->outputs.size(); ++index)
    {
        if (transistor->outputs[index] == wire)
        {
            transistor->outputs.erase(transistor->outputs.begin() + index);
            break;
        }
    }
    wire->inTransistor = nullptr;
}
Wire* FormTtTConnection(Transistor* a, Transistor* b, WireDirection shape = WireDirection::XFirst)
{
    Wire* connector = new Wire();
    connector->direction = shape;
    SolderTransistorOutput(a, connector);
    SolderTransistorInput(b, connector);
    return connector;
}

int main(void)
{
    const int WindowWidth = 1280;
    const int WindowHeight = 720;
    const float gridSize = 8.0f;

    std::vector<Wire*> wires;
    std::vector<Transistor*> transistors;

    auto deleteWire = [&transistors, &wires](Wire* wire)
    {
        if (!wire->inTransistor->ConnectsExternally())
        {
            for (size_t i = 0; i < transistors.size(); ++i)
            {
                if (transistors[i] == wire->inTransistor)
                {
                    transistors.erase(transistors.begin() + i);
                    break;
                }
            }
            delete wire->inTransistor;
        }
        if (!wire->outTransistor->ConnectsExternally())
        {
            for (size_t i = 0; i < transistors.size(); ++i)
            {
                if (transistors[i] == wire->outTransistor)
                {
                    transistors.erase(transistors.begin() + i);
                    break;
                }
            }
            delete wire->outTransistor;
        }
        for (size_t i = 0; i < wires.size(); ++i)
        {
            if (wires[i] == wire)
            {
                wires.erase(wires.begin() + i);
                break;
            }
        }
        delete wire;
    };

    Undo undoneAction = Undo();

    auto Vector2InvertY = [&WindowHeight](Vector2 vector) {
        return Vector2{ vector.x, WindowHeight - vector.y };
    };

    InitWindow(WindowWidth, WindowHeight, "Electron Architect");

    Vector2 cursorPos = { -100.0f, -100.0f };
    Vector2 wireStart = { -100.0f, -100.0f };
    Vector2 selectionStart = {};
    Vector2 selectionEnd = {};

    InputMode mode = {};

    //Shader circuitPostProc = LoadShader(0, "pretty.frag");
    //RenderTexture2D wireTexture = LoadRenderTexture(WindowWidth, WindowHeight);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Update vars
        //---------------------------------------------
        cursorPos = GetMousePosition();
        cursorPos = Vector2Snap(cursorPos, gridSize);

        Transistor* transistorHovering = nullptr;
        for (Transistor* transistor : transistors) {
            if (Vector2Equal(cursorPos, transistor->pos)) {
                transistorHovering = transistor;
                break;
            }
        }
        Wire* wireHovering = nullptr;
        for (Wire* wire : wires) {
            if (wire->IsPointOnLine(cursorPos)) {
                wireHovering = wire;
                break;
            }
        }

        switch (mode.mode)
        {
        case InputMode::Mode::None:
            // Pressed
            // M1
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) // Start a wire
            {
                wireStart = cursorPos;
                mode.mode = InputMode::Mode::Wire;
                if (wireHovering && !transistorHovering) // Dragging off of an existing wire
                {
                    Transistor* endTransistor = wireHovering->outTransistor;
                    BreakWireOutput(wireHovering);
                    Transistor* splitTransistor = new Transistor();
                    splitTransistor->type = Transistor::Type::Simple;
                    splitTransistor->pos = cursorPos;
                    transistors.push_back(splitTransistor);
                    SolderTransistorInput(splitTransistor, wireHovering);
                    wires.push_back(FormTtTConnection(splitTransistor, endTransistor, wireHovering->direction));
                }
            }
            // M2
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) // Enter Gate mode
            {
                mode.mode = InputMode::Mode::Gate;
            }

            if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)) // Move transistor being hovered
            {
                mode.mode = InputMode::Mode::Move;
                mode.data.move.dragingTransistor = transistorHovering;
            }
        break;

        case InputMode::Mode::Wire:
        {
            // Pressed
            // M2
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) // Change wire direction
            {
                mode.data.wire.wireDirection = (WireDirection)(((char)mode.data.wire.wireDirection + 1) % (char)WireDirection::Size); // Should % the number of wire directions
            }

            // Released
            // M1
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) // Finish drawing wire
            {
                Wire* newWire = new Wire(wireStart, cursorPos, mode.data.wire.wireDirection, &transistors);
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
                if (transistorHovering)
                {
                    DrawCircleV(cursorPos, 8.0f, YELLOW);
                    if (transistorHovering->OutputOnly()) transistorHovering->type = (Transistor::Type)(((int)(transistorHovering->type) + 1) % 2); // If the transistor has no inputs, toggle between simple & invert
                    else transistorHovering->type = (Transistor::Type)(((int)(transistorHovering->type) + 1) % (int)Transistor::Type::Size); // Should % the number of transistor types
                }
                else if (wireHovering)
                {
                    deleteWire(wireHovering);
                    wireHovering = nullptr;
                }
                selectionStart = cursorPos;
            }
            // TODO: component Abstraction
            else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                selectionEnd = cursorPos;
                Rectangle selectionSpace = RecVec2(selectionStart, selectionEnd);

                DrawRectangleRec(selectionSpace, ColorAlpha(YELLOW, 0.2f));
                DrawRectangleLines((int)(selectionSpace.x + 0.5f), (int)(selectionSpace.y + 0.5f), (int)(selectionSpace.width + 0.5f), (int)(selectionSpace.height + 0.5f), YELLOW);

                // TODO
            }
            else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                // TODO
            }
            // M2
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
            {
                mode.mode = InputMode::Mode::None;
            }

            // Show branches
            if (transistorHovering)
            {
                transistorHovering->Highlight(YELLOW, 16);

                for (Wire* wire : transistorHovering->inputs)
                {
                    wire->Highlight(BLUE, 4);
                }
                for (Wire* wire : transistorHovering->outputs)
                {
                    wire->Highlight(GREEN, 4);
                }
            }
            // Show selectable wire
            if (wireHovering)
            {
                wireHovering->Highlight(YELLOW, 4);
            }
        }
        break;

        case InputMode::Mode::Move:
            if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON))
            {
                if (mode.data.move.dragingTransistor) mode.data.move.dragingTransistor->pos = cursorPos;
            }

            if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON))
            {
                if (mode.data.move.dragingTransistor)
                {
                    size_t index = 0;
                    for (Transistor* transistor : transistors)
                    {
                        if (Vector2Equal(transistor->pos, mode.data.move.dragingTransistor->pos))
                        {
                            if (transistor != mode.data.move.dragingTransistor)
                            {
                                // TODO: What happens when a transistor is dropped onto the same position as another?
                                break;
                            }
                        }
                    }
                }
                mode.mode = InputMode::Mode::None;
                mode.data.move.dragingTransistor = nullptr;
            }
            break;
        }

        // Key comboes
        if (wires.size() && IsKeyDown(KEY_LEFT_CONTROL))
        {
            // Save
            if (IsKeyPressed(KEY_S))
            {
                Save(&transistors);
            }
            // Load
            if (IsKeyPressed(KEY_L))
            {
                Load(&transistors, &wires);
            }

            // Undo/redo
            if (IsKeyPressed(KEY_Z))
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
                    Wire* wireToUndo = wires[wires.size() - 1];
                    undoneAction = Undo(wireToUndo);
                    if (!wireToUndo->inTransistor->ConnectsExternally())
                    {
                        for (int i = 0; i < transistors.size(); ++i)
                        {
                            if (transistors[i] == wireToUndo->inTransistor)
                            {
                                transistors.erase(transistors.begin() + i);
                                break;
                            }
                        }
                        delete wireToUndo->inTransistor;
                    }
                    if (!wireToUndo->outTransistor->ConnectsExternally())
                    {
                        for (int i = 0; i < transistors.size(); ++i)
                        {
                            if (transistors[i] == wireToUndo->outTransistor)
                            {
                                transistors.erase(transistors.begin() + i);
                                break;
                            }
                        }
                        delete wireToUndo->outTransistor;
                    }
                    delete wireToUndo;
                    wires.pop_back();
                }
            }
        }

        // Evaluation
        {
            std::vector<Transistor*> evaluationList;
            for (Transistor* transistor : transistors)
            {
                if (transistor->OutputOnly()) evaluationList.push_back(transistor); // Start of a branch
            }

            for (Transistor* transistor : evaluationList)
            {
                transistor->Evaluate();
            }
        }

        // Draw
        //---------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);

        // Columns
        for(int x = 0; x < WindowWidth; x += gridSize)
        {
            DrawLine(x, 0, x, WindowHeight, { 255,255,255,20 });
        }
        // Rows
        for (int y = 0; y < WindowHeight; y += gridSize)
        {
            DrawLine(0, y, WindowWidth, y, { 255,255,255,20 });
        }

        // Draw cursor
        {
            Color cursorColor = WHITE;
            if (mode.mode == InputMode::Mode::Wire) cursorColor = RED;
            else if (mode.mode == InputMode::Mode::Gate) cursorColor = YELLOW;

            DrawRectangleV({ cursorPos.x - 2, cursorPos.y - 2 }, { 4,4 }, cursorColor);
        }

        if (mode.mode == InputMode::Mode::Wire)
        {
            DrawSnappedLine(wireStart, cursorPos, GRAY, mode.data.wire.wireDirection); // Temporary wire
        }

        if (mode.mode == InputMode::Mode::Gate)
        {
            // TODO
        }

        // Draw the current architecture
        // Wires
        //BeginTextureMode(wireTexture);

        { // We want to draw the active wires on top of the inactive ones
            std::vector<Wire*> activeWires;
            for (Wire* wire : wires)
            {
                if (wire->active) activeWires.push_back(wire);
                else wire->Draw();
            }
            for (Wire* wire : activeWires) { wire->Draw(); }
        }

        //EndTextureMode();

        // Post processing
        //BeginShaderMode(circuitPostProc);

        //DrawTextureRec(wireTexture.texture, Rectangle{0.0f, 0.0f, (float)wireTexture.texture.width, -1.0f * (float)wireTexture.texture.height}, {0,0}, WHITE);

        //EndShaderMode();

        // Transistors
        for (Transistor* transistor : transistors) { transistor->Draw(); }

        DrawText("Apple", 0, 0, 8, WHITE);

        EndDrawing();

        for (Transistor* transistor : transistors) { transistor->FrameReset(); }
    }

    // Return memory to heap
    //--------------------------
    //UnloadShader(circuitPostProc);
    //UnloadRenderTexture(wireTexture);

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



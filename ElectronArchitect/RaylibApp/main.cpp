#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "Globals.h"
#include "Transistor.h"
#include "Wire.h"
#include "Abstraction.h"
#include "Serialization.h"

struct Transistor;
struct Wire;

Vector2 Vector2Snap(const Vector2& vector, float g_gridSize)
{
    Vector2 roundOffs = { 0.5f, 0.5f };
    if (vector.x < 0.0f) roundOffs.x *= -1.0f;
    if (vector.y < 0.0f) roundOffs.y *= -1.0f;
    return Vector2{
        ((float)((int)((vector.x / g_gridSize) + roundOffs.x)) * g_gridSize),
        ((float)((int)((vector.y / g_gridSize) + roundOffs.y)) * g_gridSize)
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
        struct WireData {
            Wire* draggingWire;
            WireDirection wireDirection;
        };
        WireData wire;
    };
    Data data;
};

struct Undo // TODO: Make more versatile
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

void DeleteWire(Wire* wire)
{
    if (!wire->inTransistor->ConnectsExternally())
    {
        for (size_t i = 0; i < Transistor::allTransistors.size(); ++i)
        {
            if (Transistor::allTransistors[i] == wire->inTransistor)
            {
                Transistor::allTransistors.erase(Transistor::allTransistors.begin() + i);
                break;
            }
        }
        delete wire->inTransistor;
    }
    if (!wire->outTransistor->ConnectsExternally())
    {
        for (size_t i = 0; i < Transistor::allTransistors.size(); ++i)
        {
            if (Transistor::allTransistors[i] == wire->outTransistor)
            {
                Transistor::allTransistors.erase(Transistor::allTransistors.begin() + i);
                break;
            }
        }
        delete wire->outTransistor;
    }
    for (size_t i = 0; i < Wire::allWires.size(); ++i)
    {
        if (Wire::allWires[i] == wire)
        {
            Wire::allWires.erase(Wire::allWires.begin() + i);
            break;
        }
    }
    wire->ClearReferences();
    delete wire;
}

/************************************************************
* 
*   START OF PROGRAM
* 
************************************************************/

int main(void)
{
    Undo undoneAction = Undo();

    auto Vector2InvertY = [](Vector2 vector) {
        return Vector2{ vector.x, g_windowHeight - vector.y };
    };

    InitWindow(g_windowWidth, g_windowHeight, "Electron Architect");

    cursorPos = { -100.0f, -100.0f };
    Vector2 cursorPosDelta = { };
    Vector2 wireStart = { -100.0f, -100.0f };
    Vector2 selectionStart = {};
    InputMode mode = {};

    bool b_moveActive = false;
    bool b_selectionIsExplicit = false; // Whether the current selection was created explicitly by the user (with a selection box) vs just an on-the-spot hover promotion to implied selection.
    std::vector<Transistor*> selection;
    Rectangle selectionSpace = {};

    auto ClearSelection = [&selection, &b_selectionIsExplicit]() {
        selection.clear();
        b_selectionIsExplicit = false;
    };

    //Shader circuitPostProc = LoadShader(0, "pretty.frag");
    //RenderTexture2D wireTexture = LoadRenderTexture(g_windowWidth, g_windowHeight);

    // TODO: Set up camera
    Camera2D camera;
    camera.offset = { (float)g_windowWidth / 2.0f, (float)g_windowHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.target = { 0,0 };
    camera.zoom = 1.0f;
    Vector2 tweenCamOffset = {0,0}; // This will store the decimal camera offset, while the true camera offset will be this but snapped to the grid.

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Update vars
        //---------------------------------------------
       
        // Zoom
        if (GetMouseWheelMove())
        {
            float delta = 0.0f;
            if (camera.zoom <= 1.0f)
            {
                if (GetMouseWheelMove() < 0.0f)
                {
                    delta = -1.0f * (camera.zoom / 2);
                }
                else if (camera.zoom < 1.0f)
                {
                    delta = camera.zoom;
                }
                else delta = GetMouseWheelMove();
            }
            else
            {
                delta = GetMouseWheelMove();
            }
            camera.zoom = camera.zoom + delta;
            camera.offset = camera.offset + (cursorPos * (delta * -1.0f));
        }

        // Pan
        camera.offset.x += ((float)((int)(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) - (int)(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))) * (g_gridSize * camera.zoom));
        camera.offset.y += ((float)((int)(IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) - (int)(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))) * (g_gridSize * camera.zoom));
        //camera.offset = Vector2Snap(tweenCamOffset, g_gridSize);

        {
            Vector2 cursorPosLast = cursorPos;
            cursorPos = (GetMousePosition() - camera.offset) * (1.0f / camera.zoom);
            cursorPos = Vector2Snap(cursorPos, g_gridSize);
            cursorPosDelta = cursorPos - cursorPosLast;
        }

        Component* componentHovering = nullptr;
        for (Component* component : Component::allComponents) {
            if (CheckCollisionPointRec(cursorPos, component->casing)) {
                componentHovering = component;
                break;
            }
        }
        Transistor* transistorHovering = nullptr;
        for (Transistor* transistor : Transistor::allTransistors) {
            if (Vector2Equal(cursorPos, transistor->pos) && !transistor->b_hidden) {
                transistorHovering = transistor;
                break;
            }
        }
        Wire* wireHovering = nullptr;
        for (Wire* wire : Wire::allWires) {
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
                    splitTransistor->type = TransistorType::Simple;
                    splitTransistor->pos = cursorPos;
                    Transistor::allTransistors.push_back(splitTransistor);
                    SolderTransistorInput(splitTransistor, wireHovering);
                    Wire::allWires.push_back(FormTtTConnection(splitTransistor, endTransistor, wireHovering->direction));
                }
            }
            // M2
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) // Enter Gate mode
            {
                mode.mode = InputMode::Mode::Gate;
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
                Wire* newWire = new Wire(wireStart, cursorPos, mode.data.wire.wireDirection);
                Wire::allWires.push_back(newWire);

                mode.mode = InputMode::Mode::None;
            }
        }
        break;

        case InputMode::Mode::Gate:
        {
            if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON) && selection.empty()) // Select a single transistor for moving
            {
                if (transistorHovering)
                {
                    selection.push_back(transistorHovering);
                }
                else if (componentHovering)
                {
                    for (size_t i = 0; i < componentHovering->internalCount; ++i)
                    {
                        selection.push_back(componentHovering->internals[i]);
                    }
                    for (size_t i = 0; i < componentHovering->inputCount; ++i)
                    {
                        selection.push_back(componentHovering->inputs[i]);
                    }
                    for (size_t i = 0; i < componentHovering->outputCount; ++i)
                    {
                        selection.push_back(componentHovering->outputs[i]);
                    }
                }
                b_moveActive = true;
            }
            if ((IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_BACKSPACE)) && !selection.empty()) // Delete selection
            {
                for (Transistor* item : selection) // For each transistor in the selection
                {
                    // Clean up extra-selectional transistors
                    for (Wire* wire : item->inputs) // For every wire on the input side of the transistor
                    {
                        if (wire->inTransistor->OutputOnly() && wire->inTransistor->outputs.size() == 1) // If this wire were deleted, the transistor on the input side of the wire would have no connections.
                        {
                            Erase(Transistor::allTransistors, wire->inTransistor); // Remove the transistor from the vector of all transistors
                            delete wire->inTransistor; // Free the memory
                            wire->inTransistor = nullptr; // Prevent the destructor in Wire from trying to access the freed memory while removing all references to itself
                        }
                        wire->ClearReferences();
                        delete wire; // Free the memory
                    }
                    item->inputs.clear();

                    for (Wire* wire : item->outputs)
                    {
                        if (wire->outTransistor->InputOnly() && wire->outTransistor->inputs.size() == 1)
                        {
                            Erase(Transistor::allTransistors, wire->outTransistor);
                            delete wire->outTransistor;
                            wire->outTransistor = nullptr;
                        }
                        wire->ClearReferences();;
                        delete wire;
                    }
                    item->outputs.clear();

                    // Delete selected transistor
                    Erase(Transistor::allTransistors, item);
                    delete item;
                }
                ClearSelection();
            }

            // Pressed
            // M1
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                ClearSelection();
                if (transistorHovering)// Change Transistor mode
                {
                    DrawCircleV(cursorPos, 8.0f, YELLOW);
                    if (transistorHovering->OutputOnly()) transistorHovering->type = (TransistorType)(((int)(transistorHovering->type) + 1) % 2); // If the transistor has no inputs, toggle between simple & invert
                    else transistorHovering->type = (TransistorType)(((int)(transistorHovering->type) + 1) % (int)TransistorType::Size); // Should % the number of transistor types
                    selectionStart = cursorPos;
                }
                else if (wireHovering) // Delete wire
                {
                    DeleteWire(wireHovering);
                    wireHovering = nullptr;
                }
                else if (componentHovering) // Delete component
                {
                    Erase(Component::allComponents, componentHovering);
                    delete componentHovering;
                }
            }
            // TODO: component Abstraction
            else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                Vector2 selectionEnd = cursorPos;
                selectionSpace = RecVec2(selectionStart, selectionEnd);
            }
            else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                selectionSpace = RecVec2(selectionStart, cursorPos);
                ClearSelection();
                if (!Vector2Equal(selectionStart, cursorPos)) // The selection is a box and not a point
                {
                    for (Transistor* check : Transistor::allTransistors)
                    {
                        if (CheckCollisionPointRec(check->pos, selectionSpace)) selection.push_back(check); // Add all transistors whose positions are within the collision
                    }
                }
                b_selectionIsExplicit = true;
                selectionSpace = {};
            }
            // M2
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
            {
                ClearSelection();
                mode.mode = InputMode::Mode::None;
            }
        }
        break;
        }

        if (b_moveActive) // Dragging
        {
            for (Transistor* elem : selection)
            {
                if (elem->containingComponent) elem->containingComponent->UpdateCasing();
            }
            if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON))
            {
                if (!b_selectionIsExplicit) ClearSelection();
                b_moveActive = false;
            }
            else if (((abs(cursorPosDelta.x) >= g_gridSize)  || (abs(cursorPosDelta.y) >= g_gridSize)) && IsMouseButtonDown(MOUSE_MIDDLE_BUTTON))
            {
                for (Transistor* item : selection)
                {
                    item->pos = item->pos + cursorPosDelta;
                }
            }
        }
        else // Not dragging
        {
            if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON))
            {
                if (IsKeyDown(KEY_LEFT_ALT))
                {
                    std::vector<Transistor*> selectionCopy;
                    for (size_t i = 0; i < selection.size(); ++i)
                    {
                        Transistor* copy = new Transistor();
                        copy->pos = selection[i]->pos;
                        copy->type = selection[i]->type;
                        selectionCopy.push_back(copy);
                        Transistor::allTransistors.push_back(copy);
                    }
                    for (size_t i = 0; i < selection.size(); ++i)
                    {
                        // Make inputs relative
                        for (Wire* inputWire : selection[i]->inputs)
                        {
                            for (size_t index = 0; index < selection.size(); ++index)
                            {
                                if (selection[index] == inputWire->inTransistor)
                                {
                                    Wire::allWires.push_back(FormTtTConnection(selectionCopy[index], selectionCopy[i], inputWire->direction));
                                    break;
                                }
                            }
                        }
                    }
                    for (size_t i = 0; i < selection.size(); ++i)
                    {
                        selection[i] = selectionCopy[i];
                    }
                }
                b_moveActive = true;
            }
        }

        if (IsKeyPressed(KEY_SPACE) && selection.size())
        {
            MakeAbstract(selection, cursorPos, g_gridSize);
            selection.clear();
        }

        // Key comboes
        if (Wire::allWires.size() && IsKeyDown(KEY_LEFT_CONTROL))
        {
            // Save
            if (IsKeyPressed(KEY_S))
            {
                Save(&Transistor::allTransistors);
            }
            // Load
            if (IsKeyPressed(KEY_L))
            {
                Load(&Transistor::allTransistors, &Wire::allWires);
            }

            // Undo/redo
            if (IsKeyPressed(KEY_Z))
            {
                if (IsKeyDown(KEY_LEFT_SHIFT)) // Redo
                {
                    if (undoneAction.valid)
                    {
                        Wire* newWire = new Wire(undoneAction.start, undoneAction.end, undoneAction.direction);
                        Wire::allWires.push_back(newWire);
                        undoneAction = Undo();
                    }
                }
                else if (!IsKeyDown(KEY_LEFT_SHIFT)) // Undo
                {
                    Wire* wireToUndo = Wire::allWires[Wire::allWires.size() - 1];
                    undoneAction = Undo(wireToUndo);
                    if (!wireToUndo->inTransistor->ConnectsExternally())
                    {
                        Erase(Transistor::allTransistors, wireToUndo->inTransistor);
                        delete wireToUndo->inTransistor;
                    }
                    if (!wireToUndo->outTransistor->ConnectsExternally())
                    {
                        Erase(Transistor::allTransistors, wireToUndo->outTransistor);
                        delete wireToUndo->outTransistor;
                    }
                    wireToUndo->ClearReferences();
                    delete wireToUndo;
                    Wire::allWires.pop_back();
                }
            }
        }

        // Evaluation
        {
            std::vector<Transistor*> evaluationList;
            for (Transistor* transistor : Transistor::allTransistors)
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
        BeginMode2D(camera);

        ClearBackground({ 8,8,8, 255 });

        DrawRectangleRec(selectionSpace, ColorAlpha(YELLOW, 0.2f));
        {
            const float gridScale = 50.0f;
            Vector2 gridSpace = Vector2{ (gridScale * g_gridSize), (gridScale * g_gridSize) };
            Vector2 gridEnd = cursorPos + gridSpace;
            Vector2 gridStart = cursorPos + (gridSpace * -1.0f);
            // Columns
            for (float x = gridStart.x; x <= gridEnd.x; x += g_gridSize)
            {
                if (x == cursorPos.x || x == 0.0f)
                {
                    unsigned char alpha = 32;
                    if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON) || IsMouseButtonDown(MOUSE_LEFT_BUTTON)) alpha = 48;
                    DrawLineEx({ x, gridStart.y }, { x, gridEnd.y }, 2, { 255,255,255, alpha });
                }
                else DrawLineV({ x, gridStart.y }, { x, gridEnd.y }, { 255,255,255,16 });
            }
            // Rows
            for (float y = gridStart.y; y <= gridEnd.y; y += g_gridSize)
            {
                if (y == cursorPos.y || y == 0.0f)
                {
                    unsigned char alpha = 32;
                    if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON) || IsMouseButtonDown(MOUSE_LEFT_BUTTON)) alpha = 48;
                    DrawLineEx({ gridStart.x, y }, { gridEnd.x, y }, 2, { 255,255,255, alpha });
                }
                else DrawLineV({ gridStart.x, y }, { gridEnd.x, y }, { 255,255,255,16 });
            }
        }
        for (Transistor* check : Transistor::allTransistors)
        {
            if (CheckCollisionPointRec(check->pos, selectionSpace))
            {
                check->Highlight(YELLOW, 8);
            }
        }
        DrawRectangleLinesEx(selectionSpace, 2, YELLOW);

        if (!selection.empty())
        {
            for (Transistor* select : selection)
            {
                select->Highlight(YELLOW, 8);
                for (Wire* wire : select->inputs) { wire->Highlight(BLUE, 4); }
                for (Wire* wire : select->outputs) { wire->Highlight(GREEN, 4); }
            }
        }
        else if (mode.mode == InputMode::Mode::Gate)
        {
            // Show branches
            if (transistorHovering)
            {
                transistorHovering->Highlight(YELLOW, 8);

                for (Wire* wire : transistorHovering->inputs) { wire->Highlight(BLUE, 4); }
                for (Wire* wire : transistorHovering->outputs) { wire->Highlight(GREEN, 4); }
            }
            else if (wireHovering) wireHovering->Highlight(YELLOW, 4); // Show selectable wire
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
            for (Wire* wire : Wire::allWires)
            {
                if (wire->active) activeWires.push_back(wire);
                else wire->Draw();
            }
            for (Wire* wire : activeWires) { wire->Draw(); }
        }

        for (Component* component : Component::allComponents) // Draw component casings
        {
            component->Draw();
        }

        if (componentHovering && !(transistorHovering || b_moveActive)) componentHovering->Highlight({32,32,0,255});
        else if (transistorHovering) transistorHovering->Highlight(YELLOW, 8.0f);

        //EndTextureMode();

        // Post processing
        //BeginShaderMode(circuitPostProc);

        //DrawTextureRec(wireTexture.texture, Rectangle{0.0f, 0.0f, (float)wireTexture.texture.width, -1.0f * (float)wireTexture.texture.height}, {0,0}, WHITE);

        //EndShaderMode();

        // Transistors
        for (Transistor* transistor : Transistor::allTransistors) { transistor->Draw(); }

        EndMode2D();

        DrawText(TextFormat("Transistors: %i\nWires: %i\n\nZoom: %f", Transistor::allTransistors.size(), Wire::allWires.size(), camera.zoom), 0, 0, 8, WHITE);

        EndDrawing();

        for (Transistor* transistor : Transistor::allTransistors) { transistor->FrameReset(); }
    }

    // Return memory to heap
    //--------------------------
    //UnloadShader(circuitPostProc);
    //UnloadRenderTexture(wireTexture);

    selection.clear();

    for (Transistor* transistor : Transistor::allTransistors) { delete transistor; }
    Transistor::allTransistors.clear();

    for (Wire* wire : Wire::allWires) { delete wire; }
    Wire::allWires.clear();

    for (Component* component : Component::allComponents) { delete component; }
    Component::allComponents.clear();

    return 0;
}



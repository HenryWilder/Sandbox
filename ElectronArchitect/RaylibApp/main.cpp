#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "Globals.h"
#include "Transistor.h"
#include "Abstraction.h"
#include "Serialization.h"

class Transistor;
class Component;

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
            Transistor* start;
            Transistor::Connection* draggingWire;
            Transistor::Connection::Shape shape;
        };
        WireData wire;
    };
    Data data;
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

/************************************************************
* 
*   START OF PROGRAM
* 
************************************************************/

int main(void)
{
    

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

        {
            Vector2 cursorPosLast = cursorPos;
            cursorPos = (GetMousePosition() - camera.offset) * (1.0f / camera.zoom);
            cursorPos = Vector2Snap(cursorPos);
            cursorPosDelta = cursorPos - cursorPosLast;
        }

        Component* componentHovering = nullptr;
        for (Component* component : Component::s_allComponents) {
            if (CheckCollisionPointRec(cursorPos, component->GetCaseRect())) {
                componentHovering = component;
                break;
            }
        }
        Transistor* transistorHovering = nullptr;
        for (Transistor* transistor : Transistor::s_allTransistors) {
            if (!transistor->IsHidden() && Vector2Equal(cursorPos, transistor->GetPos())) {
                transistorHovering = transistor;
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
                mode.data.wire.start = nullptr;
                for (Transistor* check : Transistor::s_allTransistors)
                {
                    if (Vector2Equal(check->GetPos(), cursorPos))
                    {
                        mode.data.wire.start = check;
                        break;
                    }
                }
                if (!mode.data.wire.start) mode.data.wire.start = new Transistor(cursorPos);
                mode.mode = InputMode::Mode::Wire;
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
                mode.data.wire.shape = (Transistor::Connection::Shape)(((int)mode.data.wire.shape + 1) % (int)Transistor::Connection::Shape::Size); // Should % the number of wire directions
            }

            // Released
            // M1
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) // Finish drawing wire
            {
                Transistor* endTransistor = nullptr;
                for (Transistor* check : Transistor::s_allTransistors)
                {
                    if (Vector2Equal(check->GetPos(), cursorPos))
                    {
                        endTransistor = check;
                        check->SolderInput(mode.data.wire.start, mode.data.wire.shape);
                        break;
                    }
                }
                if (!endTransistor) endTransistor = new Transistor(mode.data.wire.start, cursorPos, mode.data.wire.shape);
                if (endTransistor == mode.data.wire.start)
                {
                    delete mode.data.wire.start; // Make sure this is correct
                }
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
                else if (componentHovering) // Drag component
                {
                    selection.reserve(selection.size() + componentHovering->GetTotalCount());
                    for (size_t i = 0; i < componentHovering->GetTotalCount(); ++i)
                    {
                        selection.push_back(componentHovering->Begin()[i]);
                    }
                }
                b_moveActive = true;
            }
            if ((IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_BACKSPACE)) && !selection.empty()) // Delete selection
            {
                while (!selection.empty())
                {
                    size_t index = selection.size() - 1;
                    selection[index]->ClearReferences();
                    delete selection[index];
                    selection.pop_back();
                }
                b_selectionIsExplicit = false;
            }

            // Pressed
            // M1
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                ClearSelection();
                if (transistorHovering)// Change Transistor mode
                {
                    transistorHovering->IncrementGate();
                }
                else if (componentHovering) // Delete component
                {
                    componentHovering->ClearReferences();
                    delete componentHovering;
                    componentHovering = nullptr;
                }
                selectionStart = cursorPos;
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
                    for (Transistor* check : Transistor::s_allTransistors)
                    {
                        if (CheckCollisionPointRec(check->GetPos(), selectionSpace)) selection.push_back(check); // Add all transistors whose positions are within the collision
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
                if (elem == nullptr) continue;
                if (elem->GetComponent()) elem->GetComponent()->UpdateCasing();
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
                    item->ChangePos(cursorPosDelta);
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
                        copy->SetPos(selection[i]->GetPos());
                        copy->SetGateType(selection[i]->GetGateType());
                        selectionCopy.push_back(copy);
                        Transistor::s_allTransistors.push_back(copy);
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
            new Component(selection, cursorPos);
            selection.clear();
        }

        // Evaluation
        {
            std::vector<Transistor*> evaluationList;
            for (Transistor* transistor : Transistor::s_allTransistors)
            {
                if (transistor->InputOnly()) evaluationList.push_back(transistor); // Start of a branch
            }

            for (Transistor* transistor : evaluationList)
            {
                transistor->GetEvaluation();
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
        for (Transistor* check : Transistor::s_allTransistors)
        {
            if (CheckCollisionPointRec(check->GetPos(), selectionSpace))
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
            }
        }
        else if (mode.mode == InputMode::Mode::Gate)
        {
            // Show branches
            if (transistorHovering)
            {
                transistorHovering->Highlight(YELLOW, 8);
                // TODO: Highlight wires
            }
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
            Transistor::DrawSnappedLine(wireStart, cursorPos, GRAY, mode.data.wire.shape); // Temporary wire
        }

        for (Component* component : Component::s_allComponents) // Draw component casings
        {
            component->Draw();
        }

        if (componentHovering && !(transistorHovering || b_moveActive)) componentHovering->Highlight({32,32,0,255});
        else if (transistorHovering) transistorHovering->Highlight(YELLOW, 8.0f);

        // Transistors
        for (Transistor* transistor : Transistor::s_allTransistors) { transistor->Draw(); }

        EndMode2D();

        DrawText(TextFormat("Transistors: %i\nZoom: %f", Transistor::s_allTransistors.size(), camera.zoom), 0, 0, 8, WHITE);

        EndDrawing();

        for (Transistor* transistor : Transistor::s_allTransistors) { transistor->FrameReset(); }
    }

    // Return memory to heap
    //--------------------------

    selection.clear();

    for (Component* component : Component::s_allComponents) { delete component; } // Must come before transistors
    Component::s_allComponents.clear();

    for (Transistor* transistor : Transistor::s_allTransistors) { delete transistor; }
    Transistor::s_allTransistors.clear();

    return 0;
}



#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "CustomTypes.h"
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

    cursorPos;
    Int2 cursorPosDelta(0);
    Int2 wireStart(0);
    Int2 selectionStart(0);
    InputMode mode = {};

    bool b_moveActive = false;
    bool b_selectionIsExplicit = false; // Whether the current selection was created explicitly by the user (with a selection box) vs just an on-the-spot hover promotion to implied selection.
    std::vector<Transistor*> selection;
    IntRect selectionSpace = {};

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
                    delta = -(camera.zoom / 2);
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
            camera.offset = Vector2Add(camera.offset, Vector2Multiply((Vector2)cursorPos, (Vector2)(Int2(-Round(delta)))));
        }

        // Pan
        camera.offset.x += ((float)((int)(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) - (int)(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))) * (g_gridSize * camera.zoom));
        camera.offset.y += ((float)((int)(IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) - (int)(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))) * (g_gridSize * camera.zoom));

        {
            Int2 cursorPosLast = cursorPos;
            cursorPos = Int2(Vector2Snap(Vector2Divide(Vector2Subtract(GetMousePosition(), camera.offset), Vector2{ camera.zoom, camera.zoom })));
            cursorPosDelta = cursorPos - cursorPosLast;
        }

        Component* componentHovering = nullptr;
        for (Component* component : Component::s_allComponents) {
            if (component->GetCaseRect().Contains(cursorPos)) {
                componentHovering = component;
                break;
            }
        }
        Transistor* transistorHovering = nullptr;
        for (Transistor* transistor : Transistor::s_allTransistors) {
            if (!transistor->IsHidden() && cursorPos == transistor->GetPos()) {
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
                    if (check->GetPos() == cursorPos)
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
                    if (check->GetPos() == cursorPos)
                    {
                        endTransistor = check;
                        check->SolderInput(mode.data.wire.start, mode.data.wire.shape);
                        break;
                    }
                }
                if (!endTransistor) endTransistor = new Transistor(mode.data.wire.start, cursorPos, mode.data.wire.shape);
                if (endTransistor == mode.data.wire.start)
                {
                    delete mode.data.wire.start; // TODO: Make sure this is correct
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
                    componentHovering->SelectTransistors(selection);
                }
                b_moveActive = true;
            }
            if ((IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_BACKSPACE)) && !selection.empty()) // Delete selection
            {
                for (Transistor*& elem : selection)
                {
                    elem->ClearReferences();
                    delete elem;
                    elem = nullptr;
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
            else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                Int2 selectionEnd = cursorPos;
                selectionSpace = IntRect(selectionStart, selectionEnd);
            }
            else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                selectionSpace = IntRect(selectionStart, cursorPos);
                ClearSelection();
                if (selectionStart != cursorPos) // The selection is a box and not a point
                {
                    size_t reservation = 0;
                    for (Transistor* check : Transistor::s_allTransistors)
                    {
                        if (selectionSpace.Contains(check->GetPos())) reservation++; // Add all transistors whose positions are within the collision
                    }
                    selection.reserve(reservation);
                    for (Transistor* check : Transistor::s_allTransistors)
                    {
                        if (selectionSpace.Contains(check->GetPos())) selection.push_back(check); // Add all transistors whose positions are within the collision
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
            if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON))
            {
                if (!b_selectionIsExplicit) ClearSelection();
                b_moveActive = false;
            }
            else if (((abs(cursorPosDelta.x) >= g_gridSize)  || (abs(cursorPosDelta.y) >= g_gridSize)) && IsMouseButtonDown(MOUSE_MIDDLE_BUTTON))
            {
                for (Transistor* elem : selection)
                {
                    elem->ChangePos(cursorPosDelta);
                }
            }

            for (Transistor* elem : selection) // Update all component casings
            {
                if (elem == nullptr) continue;
                if (elem->GetComponent()) elem->GetComponent()->UpdateCasing();
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

        DrawRectangleRec((Rectangle)selectionSpace, ColorAlpha(YELLOW, 0.2f));
        {
            Vector2 screenMin = GetScreenToWorld2D(Vector2{ 0.0f,0.0f }, camera);
            Vector2 screenMax = GetScreenToWorld2D(Vector2{ (float)g_windowWidth, (float)g_windowHeight }, camera);

            for (Int2 pt = Int2(screenMin); !(pt.x > screenMax.x && pt.y > screenMax.y); pt += Int2(1))
            {
                DrawLine(pt.x, screenMin.y, pt.x, screenMax.y, { 255,255,255,16 });
                DrawLine(screenMin.x, pt.y, screenMax.x, pt.y, { 255,255,255,16 });
            }
            unsigned char alpha = 32;
            if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON) || IsMouseButtonDown(MOUSE_LEFT_BUTTON)) alpha = 48;

            float width = (3.0f) / camera.zoom;
            float halfWidth = (width / 2.0f);

            // Mouse cross
            // x-axis
            DrawLineEx(
                (Vector2{ screenMin.x, (float)cursorPos.y + 1.0f - halfWidth }),
                (Vector2{ screenMax.x, (float)cursorPos.y + 1.0f - halfWidth }),
                width, { 255,255,255, alpha }
            );
            // y-axis
            DrawLineEx(
                (Vector2{ (float)cursorPos.x - 1.0f + halfWidth, screenMin.y }),
                (Vector2{ (float)cursorPos.x - 1.0f + halfWidth, screenMax.y }),
                width, { 255,255,255, alpha }
            );

            // (0,0) cross
            // x-axis
            DrawLineEx(
                (Vector2{ screenMin.x, -halfWidth }),
                (Vector2{ screenMax.x, -halfWidth }),
                width, { 255,255,255, 48 }
            );
            // y-axis
            DrawLineEx(
                (Vector2{ halfWidth, screenMin.y }),
                (Vector2{ halfWidth, screenMax.y }),
                width, { 255,255,255, 48 }
            );
        }
        for (Transistor* check : Transistor::s_allTransistors)
        {
            if (selectionSpace.Contains(check->GetPos()))
            {
                check->Highlight(YELLOW, 2.0f);
            }
        }
        DrawRectangleLinesEx((Rectangle)selectionSpace, 2.0f, YELLOW);

        if (!selection.empty()) // Highlight selection
        {
            for (Transistor* select : selection)
            {
                select->Highlight(YELLOW, 2.0f);
            }
        }
        else if (mode.mode == InputMode::Mode::Gate)
        {
            // Show branches
            if (transistorHovering)
            {
                transistorHovering->Highlight(YELLOW, 2.0f);
                // TODO: Highlight wires
            }
        }

        // Draw cursor
        {
            Color cursorColor = WHITE;
            if (mode.mode == InputMode::Mode::Wire) cursorColor = RED;
            else if (mode.mode == InputMode::Mode::Gate) cursorColor = YELLOW;

            DrawRectangleV(Vector2Subtract((Vector2)cursorPos, Vector2{ 0.5f,0.5f }), (Vector2)Int2(1), cursorColor);
        }

        if (mode.mode == InputMode::Mode::Wire)
        {
            Transistor::DrawSnappedLine((Vector2)wireStart, (Vector2)cursorPos, GRAY, mode.data.wire.shape); // Temporary wire
        }

        for (Component* component : Component::s_allComponents) // Draw component casings
        {
            component->Draw();
        }

        if (componentHovering && !(transistorHovering || b_moveActive)) componentHovering->Highlight({32,32,0,255});
        else if (transistorHovering) transistorHovering->Highlight(YELLOW, 2.0f);

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

    for (Transistor* transistor : Transistor::s_allTransistors) { delete transistor; }

    return 0;
}


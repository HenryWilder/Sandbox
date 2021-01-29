#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "CustomTypes.h"
#include "Globals.h"
#include "Transistor.h"
#include "Abstraction.h"
#include "Serialization.h"

__interface IInterface
{

};

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

    Font font = LoadFont("resources/fonts/alpha_beta.png");

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

    //Shader gridShader = LoadShader(0, "grid.frag");
    //int cameraOffsetSHD = GetShaderLocation(gridShader, "cameraPosition");
    //int cameraZoomSHD = GetShaderLocation(gridShader, "cameraZoom");

    Camera2D camera;
    camera.offset = { (float)g_windowWidth / 2.0f, (float)g_windowHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.target = { 0,0 };
    camera.zoom = 1.0f;

    //float cameraOffset[2] = { 0.0f,0.0f };
    //float cameraZoom = 0.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Update vars
        //---------------------------------------------

        // Zoom
        if (GetMouseWheelMove())
        {
            float delta = 0.0f;
            if (GetMouseWheelMove() < 0.0f) // Zoom out
            {
                delta = -(camera.zoom / 2);
            }
            else // Zoom in
            {
                delta = camera.zoom;
            }
            camera.zoom = camera.zoom + delta;
            camera.offset = Vector2Add(camera.offset, Vector2Multiply((Vector2)cursorPos, Vector2{ -delta, -delta }));
        }

        // Pan
        camera.offset.x += ((float)((int)(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) - (int)(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))) * (g_gridSize * camera.zoom));
        camera.offset.y += ((float)((int)(IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) - (int)(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))) * (g_gridSize * camera.zoom));

        Vector2 wsScreenPix = Vector2Subtract(GetScreenToWorld2D((Vector2)(Int2(1)), camera), GetScreenToWorld2D((Vector2)(Int2(0)), camera)); // The size of a screenspace pixel in worldspace

        //cameraOffset[0] = camera.offset.x;
        //cameraOffset[1] = camera.offset.y;
        //cameraZoom = camera.zoom;

        //SetShaderValue(gridShader, cameraOffsetSHD, cameraOffset, UNIFORM_VEC2);
        //SetShaderValue(gridShader, cameraZoomSHD, &cameraZoom, UNIFORM_FLOAT);
    
        // Mouse position
        {
            Int2 cursorPosLast = cursorPos; // Where the cursor was last frame
            cursorPos = Int2(GetScreenToWorld2D(GetMousePosition(), camera));
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

        //BeginShaderMode(gridShader);
        //{
        //    Vector2 start = GetScreenToWorld2D(Vector2{ 0.0f,0.0f }, camera);
        //    DrawRectangleV(
        //        start,
        //        Vector2Subtract(GetScreenToWorld2D(Vector2{ (float)g_windowWidth, -(float)g_windowHeight }, camera), start),
        //        WHITE
        //    );
        //}
        //EndShaderMode();

        // Draw grid
        {
            Vector2 screenMin = GetScreenToWorld2D(Vector2{ 0.0f,0.0f }, camera);
            Vector2 screenMax = GetScreenToWorld2D(Vector2{ (float)g_windowWidth, (float)g_windowHeight }, camera);

            if (camera.zoom >= 1) // 1x gridlines
            {
                // Gridlines
                for (int x = screenMin.x; x <= screenMax.x; ++x) { DrawLine(x, (int)screenMin.y, x, (int)screenMax.y, { 63,255,63,32 }); }
                for (int y = screenMin.y; y <= screenMax.y; ++y) { DrawLine((int)screenMin.x, y, (int)screenMax.x, y, { 63,255,63,32 }); }
                // Coords
                if (camera.zoom >= 32) {
                    for (int x = screenMin.x; x <= screenMax.x; ++x) { DrawTextEx(font, TextFormat("%i", x), Vector2{ (float)x + 0.5f, 0.05f }, ((float)font.baseSize / camera.zoom), 1.0f / camera.zoom, WHITE); }
                    for (int y = screenMin.y; y <= screenMax.y; ++y) { DrawTextEx(font, TextFormat("%i", y), Vector2{ 0.05f, (float)y + 0.5f }, (float)font.baseSize / camera.zoom, 1.0f / camera.zoom, WHITE); }
                }
            }
            else // Average
            {
                constexpr Color meld{ 63 / 4, 255 / 4, 63 / 4, 255 };
                ClearBackground(meld);
            }
            if (camera.zoom >= 0.5) // 8x gridlines
            {
                for (int x = screenMin.x; x <= screenMax.x; x += 8) { DrawLine(x, (int)screenMin.y, x, (int)screenMax.y, { 63,255,255,24 }); }
                for (int y = screenMin.y; y <= screenMax.y; y += 8) { DrawLine((int)screenMin.x, y, (int)screenMax.x, y, { 63,255,255,24 }); }
            }
            else // Average
            {
                constexpr Color meld{ (63 / 16) * 3, (255 / 16) * 3, (255 / 16) * 3, 255 };
                ClearBackground(meld);
            }
            if (camera.zoom >= 0.015625) // 64x gridlines
            {
                for (int x = screenMin.x; x <= screenMax.x; x += 64) DrawLine(x, (int)screenMin.y, x, (int)screenMax.y, { 63,63,255,24 });
                for (int y = screenMin.y; y <= screenMax.y; y += 64) DrawLine((int)screenMin.x, y, (int)screenMax.x, y, { 63,63,255,24 });
            }
            else
            {
                constexpr Color meld{ (63 / 16) * 3, (63 / 16) * 3, (255 / 16) * 3, 255 };
                ClearBackground(meld);
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
                (Vector2{ screenMin.x, -halfWidth + 1.0f }),
                (Vector2{ screenMax.x, -halfWidth + 1.0f }),
                width, { 255,255,255, 48 }
            );
            // y-axis
            DrawLineEx(
                (Vector2{ halfWidth - 1.0f, screenMin.y }),
                (Vector2{ halfWidth - 1.0f, screenMax.y }),
                width, { 255,255,255, 48 }
            );
        }

        DrawRectangleRec((Rectangle)selectionSpace, ColorAlpha(YELLOW, 0.2f));

        for (Transistor* check : Transistor::s_allTransistors)
        {
            if (selectionSpace.Contains(check->GetPos()))
            {
                check->Highlight(YELLOW, 2.0f);
            }
        }

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

        // Draw cursor
        {
            Color cursorColor = WHITE;
            if (mode.mode == InputMode::Mode::Wire) cursorColor = RED;
            else if (mode.mode == InputMode::Mode::Gate) cursorColor = YELLOW;
            Vector2 width = Vector2Scale(wsScreenPix, 4.0f);
            DrawRectangleV(Vector2Subtract((Vector2)cursorPos, Vector2Scale(width, 0.5f)), width, cursorColor);
        }

        EndMode2D();

        DrawText(TextFormat("Transistors: %i\nZoom: %f\nCursor position: (%i, %i)", Transistor::s_allTransistors.size(), camera.zoom, cursorPos.x, cursorPos.y), 0, 0, 8, WHITE);

        EndDrawing();

        for (Transistor* transistor : Transistor::s_allTransistors) { transistor->FrameReset(); }
    }

    // Return memory to heap
    //--------------------------

    //UnloadShader(gridShader);

    selection.clear();

    for (Component* component : Component::s_allComponents) { delete component; } // Must come before transistors

    for (Transistor* transistor : Transistor::s_allTransistors) { delete transistor; }

    return 0;
}


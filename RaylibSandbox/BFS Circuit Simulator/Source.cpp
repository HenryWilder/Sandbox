#include <raylib.h>
#pragma warning(push)
#pragma warning(disable : 26451)
#include <raymath.h>
#pragma warning(pop)
#include <vector>
#include <fstream>
#include <stack>
#include <queue>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include "Node.h"
#include "Component.h"
#include "Graph.h"
#include "HUI.h"

int Snap(int x, int gridsize)
{
    x /= gridsize;
    return x * gridsize;
}

Vector2 Snap(Vector2 pt, float gridsize)
{
    return {
        roundf(pt.x / gridsize) * gridsize,
        roundf(pt.y / gridsize) * gridsize
    };
}

float Signf(float value)
{
    return (value > 0.01f ? 1.0f : (value < -0.01f ? -1.0f : 0.0f));
}

struct GateSelection
{
    static constexpr Gate gates[4] = {
        Gate::OR,
        Gate::AND,
        Gate::NOR,
        Gate::XOR,
    };
    static constexpr Vector2 sectors[_countof(gates)] =
    {
        Vector2{ 0, -1 },
        Vector2{ +1, 0 },
        Vector2{ 0, +1 },
        Vector2{ -1, 0 },
    };

    size_t index;
    bool b_toggle;
};

GateSelection ClosestHoveredGateOption(Vector2 cursor, Vector2 origin)
{
    if (Vector2Distance(cursor, origin) > 0.01f)
    {
        Vector2 direction = Vector2Normalize(Vector2Subtract(cursor, origin));
        float cloesetDot = -1;
        size_t closestIndex = -1;
        for (size_t i = 0; i < 4; ++i)
        {
            float dot = Vector2DotProduct(direction, Vector2Normalize(GateSelection::sectors[i]));
            if (dot > cloesetDot)
            {
                cloesetDot = dot;
                closestIndex = i;
            }
        }
        if (cloesetDot >= 0.5f)
            return { closestIndex, false };
    }
    return { 0, true };
}

Vector2 ConstrainOffset(Vector2 constraint, Vector2 point)
{
    Vector2 difference = Vector2Subtract(point, constraint);
    float xSize = abs(difference.x);
    float ySize = abs(difference.y);
    auto [shortLength, longLength] = std::minmax(xSize, ySize);

    if (shortLength > longLength * 0.5) // Diagonal
    {
        return {
            constraint.x + copysignf(shortLength, difference.x),
            constraint.y + copysignf(shortLength, difference.y)
        };
    }
    else // Cardinal
    {
        if (xSize < ySize)
            return { constraint.x, point.y };
        else
            return { point.x, constraint.y };
    }
}

enum ModifierKey
{
    KEY_CONTROL,
    KEY_ALT,
    KEY_SHIFT,
};
bool IsModifierDown(int id)
{
    switch (id)
    {
    case KEY_CONTROL: return IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
    case KEY_ALT: return IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT);
    case KEY_SHIFT: return IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    default: return false;
    }
}

Rectangle TwoPointRec(Vector2 a, Vector2 b)
{
    auto [x1, x2] = std::minmax(a.x, b.x);
    auto [y1, y2] = std::minmax(a.y, b.y);
    return { x1, y1, x2 - x1, y2 - y1 };
}

int Editor(int windowWidth, int windowHeight)
{
    struct Tool {
        Tool()
        {
            g.plt.activeBlueprint = nullptr;
            g.plt.activeGate = Gate::OR;
            g.hvr.comp = nullptr;
            g.hvr.node = nullptr;
            g.hvr.wire = WireNull();
            g.drawGateOptions = false;
            SetMode(Mode::Select);
        }
        ~Tool() {}

        struct {
            struct {
                ComponentBlueprint* activeBlueprint;
                Gate activeGate;
            } plt;

            struct {
                Component* comp;
                Node* node;
                Wire wire;
            } hvr;

            bool drawGateOptions;
        } g;

        enum class Mode {
            // (V) Selects nodes and components if the start point overlaps none.
            //     Replaces the selection with the clicked object if it is not in the current selection.
            //     Moves the currently selected elements if the element clicked is in the current selection.
            //     Sets all selected elements to match the gate/blueprint picker if the picker changes.
            //     Displays the inputs and outputs of the hovered node, wire, or component
            Select,

            // (P) Draws nodes of the active gate connected by wires where the mouse is pressed.
            //     Connects nodes when a wire is drawn between one another.
            //     Cancels the current wire when right clicking
            Pen,

            // (A) Manually orders the input/output nodes on the hovered component.
            //     Reverses the direction of clicked wires.
            //     Moves dragged nodes.
            Direct,

            // (K) Sets the gate of the clicked node to match the active gate.
            //     Sets the blueprint of clicked components to match the active blueprint.
            Edit,

            // (Z) Moves the camera when dragging.
            //     Zooms when scrolling.
            Camera,
        } mode;

        void SetMode(Mode newMode)
        {
            switch (mode = newMode)
            {
            case Tool::Mode::Select:
                sel.start = Vector2Zero();
                sel.rect = {};
                sel.b_selecting = false;
                sel.b_dragging = false;
                break;
            case Tool::Mode::Pen:
                pen.wireStart = nullptr;
                break;
            case Tool::Mode::Direct:
                // todo
                break;
            case Tool::Mode::Edit:
                // todo
                break;
            case Tool::Mode::Camera:
                // todo
                break;
            default:
                static_assert("error");
                break;
            }
        }

        union {
            struct {
                Vector2 start;
                Rectangle rect;
                bool b_selecting;
                bool b_dragging;
                std::vector<Node*> nodes;
                std::vector<Component*> comps;
            } sel;

            struct {
                Node* wireStart; // The previous node in the wire
                Node* wireEnd; // The next node in the wire
            } pen;

            struct {
                // todo
            } dir;
            
            struct {
                // todo
            } edt;
            
            struct {
                // todo
            } cam;
        };
    } tool;
    bool holdingTemporaryComponent = false;

    // UI

    RadioButtonHandler group0;

    Button button_select;
    button_select.SetDisplayName("sel");
    button_select.SetToolTip("[@todo]");
    button_select.SetToggle(true);
    button_select.SetRect({ 30, 30, 30, 30 });
    
    Button button_pen;
    button_pen.SetDisplayName("pen");
    button_pen.SetToolTip("[@todo]");
    button_pen.SetToggle(true);
    button_pen.CopyShape(button_select);
    button_pen.OffsetFrom(button_select, Spacing::OVERLAP, 0, Spacing::PAD, 10);
    
    Button button_direct;
    button_direct.SetDisplayName("dir");
    button_direct.SetToolTip("[@todo]");
    button_direct.SetToggle(true);
    button_direct.CopyShape(button_select);
    button_direct.OffsetFrom(button_pen, Spacing::OVERLAP, 0, Spacing::PAD, 10);
    
    Button button_edit;
    button_edit.SetDisplayName("edit");
    button_edit.SetToolTip("[@todo]");
    button_edit.SetToggle(true);
    button_edit.CopyShape(button_select);
    button_edit.OffsetFrom(button_direct, Spacing::OVERLAP, 0, Spacing::PAD, 10);
    
    Button button_camera;
    button_camera.SetDisplayName("cam");
    button_camera.SetToolTip("[@todo]");
    button_camera.SetToggle(true);
    button_camera.CopyShape(button_select);
    button_camera.OffsetFrom(button_edit, Spacing::OVERLAP, 0, Spacing::PAD, 10);

    UIHandler::Global().Expect(5);
    UIHandler::Global().CreateButtonGroup_FromNew(&group0, { &button_select, &button_pen, &button_direct, &button_edit, &button_camera });

    Rectangle uiContainerRec = TwoPointRec(
        { button_select.GetX(), button_select.GetY() },
        { button_camera.GetX() + button_camera.GetWidth(), button_camera.GetY() + button_camera.GetHeight() }
    );
    uiContainerRec.x -= 6;
    uiContainerRec.y -= 6;
    uiContainerRec.width += 12;
    uiContainerRec.height += 12;


    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        // Saving and loading
        if (IsKeyPressed(KEY_S) && IsModifierDown(KEY_CONTROL))
        {
            if (IsModifierDown(KEY_SHIFT)) // Load
                Graph::Global().Load("save.graph");
            else // Save
                Graph::Global().Save("save.graph");
        }

        // Grid-snapped cursor
        Vector2 cursor = Snap(GetMousePosition(), g_nodeRadius * 2.0f);

        // Lock wire directions
        {
            if (tool.mode == Tool::Mode::Pen && !!tool.pen.wireStart)
                cursor = ConstrainOffset(tool.pen.wireStart->GetPosition(), cursor);
        }

        UIHandler::Global().UpdateCursorPos(cursor);
        UIHandler::Global().UpdateButtons();
        if (!CheckCollisionPointRec(GetMousePosition(), uiContainerRec))
        {
            SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);
        }

        // Hover
        tool.g.hvr.node = Graph::Global().FindNodeAtGridPoint(cursor);
        tool.g.hvr.wire = (tool.g.hvr.node ? WireNull() : Graph::Global().FindWireIntersectingGridPoint(cursor));

        if (!tool.sel.b_selecting)
        {
            if (!(tool.sel.nodes.empty() && tool.sel.comps.empty()))
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    if (!holdingTemporaryComponent)
                        tool.sel.start = cursor;
                    tool.sel.b_selecting = true;
                }

                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                {
                    tool.sel.b_selecting = false;
                    Vector2 offset = Vector2Subtract(cursor, tool.sel.start);

                    // Copy
                    if (IsModifierDown(KEY_ALT) && !holdingTemporaryComponent)
                    {                        
                        for (Component*& comp : tool.sel.comps)
                        {
                            Component* clone = new Component(comp->GetBlueprint());
                            comp = clone;
                        }
                        for (Node*& node : tool.sel.nodes)
                        {
                            Node* clone = new Node(node->GetPosition(), node->GetGate());
                            node = clone;
                        }
                        Graph::Global().AddNodes(tool.sel.nodes);
                    }

                    // Move
                    for (Component* comp : tool.sel.comps)
                    {
                        comp->SetPosition(Vector2Add(comp->GetPosition(), offset));
                    }
                    for (Node* node : tool.sel.nodes)
                    {
                        if (!node->IsInComponent())
                            node->SetPosition(Vector2Add(node->GetPosition(), offset));
                    }

                    // todo: ????
                    if (holdingTemporaryComponent)
                        holdingTemporaryComponent = false;
                }
            }
            else
            {
                // Drag
                if (!!tool.pen.wireStart)
                {
                    tool.pen.wireEnd = tool.g.hvr.node;

                    // Finish making connection
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                    {
                        if (tool.pen.wireEnd != tool.pen.wireStart)
                        {
                            if (!tool.pen.wireEnd)
                                tool.pen.wireEnd = Graph::Global().AddNewNode(cursor);

                            Graph::Global().ConnectNodes(tool.pen.wireStart, tool.pen.wireEnd);
                            Graph::Global().MarkRouteDirty();

                            // Divide wire
                            if (tool.g.hvr.wire)
                            {
                                Graph::Global().DivideWire(tool.g.hvr.wire, tool.pen.wireEnd);
                                tool.g.hvr.wire = WireNull();
                            }
                        }
                        tool.pen.wireStart = tool.pen.wireEnd = nullptr;
                    }
                }
                // Create new node
                else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !tool.pen.wireStart)
                {
                    Node* wireEnd = tool.g.hvr.node;

                    if (!tool.pen.wireStart)
                    {
                        tool.pen.wireStart = Graph::Global().AddNewNode(cursor);
                        Graph::Global().MarkRouteDirty();

                        // Divide wire
                        if (tool.g.hvr.wire)
                        {
                            Graph::Global().DivideWire(tool.g.hvr.wire, tool.pen.wireStart);
                            tool.g.hvr.wire = WireNull();
                        }
                    }
                }

                //drawGateOptions = (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON) && tool.pen.wireStart); // todo: What?...
                if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON) && !tool.pen.wireStart)
                {
                    if (tool.g.hvr.node)
                        tool.pen.wireStart = tool.g.hvr.node;
                    else if (tool.g.hvr.wire)
                    {
                        tool.g.hvr.wire = Graph::Global().ReverseWire(tool.g.hvr.wire);
                        Graph::Global().MarkRouteDirty();
                    }
                }
                if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON) && tool.pen.wireStart)
                {

                    // Middle click
                    if (Vector2Equals(cursor, tool.pen.wireStart->GetPosition()))
                    {
                        tool.pen.wireStart->SetGate(tool.pen.wireStart->GetGate() != Gate::NOR ? Gate::NOR : Gate::OR);
                    }
                    // Middle drag
                    else
                    {
                        // In clockwise order
                        GateSelection select = ClosestHoveredGateOption(cursor, tool.pen.wireStart->GetPosition());
                        tool.pen.wireStart->SetGate(GateSelection::gates[select.index]);
                    }

                    // Inputless nodes take output from the user and therefore are treated as mutable
                    if (!tool.pen.wireStart->HasNoInputs())
                        Graph::Global().MarkGateDirty();
                    else
                        tool.pen.wireStart->SetState(tool.pen.wireStart->GetGate() == Gate::NOR);

                    tool.pen.wireStart = tool.pen.wireEnd = nullptr;
                }
            }
        }

        // Delete selected
        if ((IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_BACKSPACE)))
        {
            if (!tool.sel.nodes.empty())
            {
                for (Node* node : tool.sel.nodes)
                {
                    Graph::Global().RemoveNode(node);
                    delete node;
                }
                tool.sel.nodes.clear();
            }
            if (!tool.sel.comps.empty())
            {
                for (Component* comp : tool.sel.comps)
                {
                    Graph::Global().RemoveComponentEntirely(comp);
                }
                tool.sel.comps.clear();
            }
        }

        // Make sure no other locally stored nodes can possibly exist at the time, or set them to nullptr
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && !tool.pen.wireStart && !tool.pen.wireEnd && !tool.sel.b_selecting)
        {
            if (tool.g.hvr.node)
            {
                bool removed; // ???

                // Transfer inputs to outputs
                if (IsModifierDown(KEY_SHIFT) &&
                    !(tool.g.hvr.node->InputCount() == 1 &&
                        tool.g.hvr.node->OutputCount() == 1))
                {
                    tool.g.hvr.node->SetGate(Gate::OR);
                }
                else
                {
                    if (IsModifierDown(KEY_SHIFT))
                        Graph::Global().ConnectNodes(tool.g.hvr.node->GetInput(0), tool.g.hvr.node->GetOutput(0));

                    Graph::Global().RemoveNode(tool.g.hvr.node);
                    delete tool.g.hvr.node;
                    tool.g.hvr.node = nullptr;
                }

                Graph::Global().MarkRouteDirty();
            }
            else if (tool.g.hvr.wire)
            {
                if (IsModifierDown(KEY_SHIFT)) // Also delete the related nodes
                {
                    Graph::Global().RemoveNode(tool.g.hvr.wire.a);
                    Graph::Global().RemoveNode(tool.g.hvr.wire.b);
                    delete tool.g.hvr.wire.a;
                    delete tool.g.hvr.wire.b;
                    tool.g.hvr.wire.a = nullptr;
                    tool.g.hvr.wire.b = nullptr;
                }
                else
                    Graph::Global().DisconnectNodes(tool.g.hvr.wire.a, tool.g.hvr.wire.b);

                tool.g.hvr.wire = WireNull();
                Graph::Global().MarkRouteDirty();
            }
            else
            {
                tool.sel.start = cursor;
                tool.sel.b_selecting = true;
            }
        }
        if (tool.sel.b_selecting)
        {
            Vector2 a = {
                std::min(tool.sel.start.x, cursor.x),
                std::min(tool.sel.start.y, cursor.y)
            };
            tool.sel.rect = {
                    a.x,
                    a.y,
                    std::max(tool.sel.start.x, cursor.x) - a.x,
                    std::max(tool.sel.start.y, cursor.y) - a.y,
            };
            Graph::Global().FindObjectsInRectangle(tool.sel.nodes, tool.sel.comps, tool.sel.rect); // todo: remove selectables

            // Finalize
            if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON))
                tool.sel.b_selecting = false;
        }

        // Package up components
        if (IsKeyPressed(KEY_SPACE) && (tool.sel.nodes.size() + tool.sel.comps.size()) > 1)
        {
            size_t nodeCount = tool.sel.nodes.size();
            for (Component* comp : tool.sel.comps)
            {
                nodeCount += comp->GetNodeCount();
            }
            std::vector<Node*> breakdown;
            breakdown.reserve(nodeCount);
            for (Node* node : tool.sel.nodes)
            {
                breakdown.push_back(node);
            }
            for (Component* comp : tool.sel.comps)
            {
                for (Node* node : comp->GetNodesVector())
                {
                    breakdown.push_back(node);
                }
            }

            ComponentBlueprint* blueprint = new ComponentBlueprint();
            blueprint->GenerateBlueprintFromSelection(breakdown);
            tool.sel.nodes.clear();
            tool.sel.comps.clear();
            tool.sel.start = { g_gridUnit * -6, 0 };
            Component* clone = Graph::Global().CloneComponentAtPosition(blueprint, tool.sel.start); // Place the component offscreen
            tool.sel.comps.push_back(clone);
            holdingTemporaryComponent = true;
            Graph::Global().MarkRouteDirty();
        }

        Graph::Global().UnDirty();
        Graph::Global().Step();

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            constexpr int gridRadius = 8;

            // World grid
            static const Color worldGridColor = ColorAlpha(DARKGRAY, 0.125f);
            for (float x = 0.0f; x < (float)windowWidth; x += 2.0f * g_nodeRadius)
            {
                DrawLineV({ x, 0 }, { x, (float)windowHeight }, worldGridColor);
            }
            for (float y = 0.0f; y < (float)windowHeight; y += 2.0f * g_nodeRadius)
            {
                DrawLineV({ 0.0f, y }, { (float)windowWidth, y }, worldGridColor);
            }

            if (tool.sel.b_selecting)
            {
                DrawRectangleRec(tool.sel.rect, ColorAlpha(GRAY, 0.125));
                DrawRectangleLines((int)lroundf(tool.sel.rect.x), (int)lroundf(tool.sel.rect.y), (int)lroundf(tool.sel.rect.width), (int)lroundf(tool.sel.rect.height), GRAY);
            }


            // Current wire being made
            if (!!tool.pen.wireStart)
            {
                Vector2 end;
                if (tool.pen.wireEnd)
                    end = tool.pen.wireEnd->GetPosition();
                else
                    end = cursor;

                DrawLineV(tool.pen.wireStart->GetPosition(), end, GRAY);

                // Cursor grid
                Vector2 p = tool.pen.wireStart->GetPosition();
                Vector2 c = cursor;

                float xLength = abs(c.x - p.x);
                float yLength = abs(c.y - p.y);
                float s = xLength;

                Vector2 directions[3];
                directions[0] = { (c.x < p.x ? -1.0f : 1.0f), (c.y < p.y ? -1.0f : 1.0f) };
                if (xLength == yLength)
                {
                    directions[1] = { directions[0].x, 0.0f };
                    directions[2] = { 0.0f, directions[0].y };
                }
                else
                {
                    if (xLength > yLength)
                    {
                        directions[1] = { directions[0].x, -directions[0].x };
                        directions[2] = { directions[0].x, +directions[0].x };
                        directions[0].y = 0.0f;
                    }
                    else
                    {
                        s = yLength;
                        directions[1] = { -directions[0].y, directions[0].y };
                        directions[2] = { +directions[0].y, directions[0].y };
                        directions[0].x = 0.0f;
                    }
                }

                for (int o = -gridRadius; o <= gridRadius; ++o)
                {
                    Color color = ColorAlpha(WHITE, 1.0f - abs((float)o / (float)gridRadius));
                    for (int i = 0; i < 3; ++i)
                    {
                        DrawPixelV(Vector2Add(Vector2Add(p, Vector2Scale(directions[i], s)), Vector2Scale(directions[i], g_gridUnit * (float)o)), color);
                    }
                }

                for (int i = -gridRadius; i <= gridRadius; ++i)
                {
                    float offset = i * g_gridUnit - 1;
                    Color color = ColorAlpha(WHITE, 1.0f - Clamp(abs(i) / (float)gridRadius, 0.0f, 1.0f));
                    Vector2 offsets[2] = {
                        { 0.0f, offset },
                        { offset, 0.0f }
                    };
                    for (int i = 0; i < 2; ++i)
                    {
                        DrawPixelV(Vector2Add(cursor, offsets[i]), color);
                    }
                }
            }
            else
            {
                // Cursor grid
                for (int y = -gridRadius; y <= gridRadius; ++y)
                {
                    int halfWidth = (int)lroundf(sqrtf((float)(gridRadius * gridRadius - y * y)));
                    for (int x = -halfWidth; x <= halfWidth; ++x)
                    {
                        Vector2 offset;
                        offset.x = x * g_gridUnit;
                        offset.y = y * g_gridUnit;
                        float alpha = 1.0f - Clamp(Vector2Length(offset) / ((float)gridRadius * g_gridUnit), 0.0f, 1.0f);
                        DrawPixelV(Vector2Add(cursor, offset), ColorAlpha(WHITE, alpha));
                    }
                }
            }

            // Crosshair
            {
                Color color = ColorAlpha(WHITE, 0.0625);
                DrawLineV({ cursor.x, 0 }, { cursor.x, (float)windowHeight }, color);
                DrawLineV({ 0, cursor.y }, { (float)windowWidth,  cursor.y }, color);
            }


            // Wires
            Graph::Global().DrawWires();

            // Moving points
            if (tool.sel.b_dragging || holdingTemporaryComponent)
            {
                Vector2 offset = Vector2Subtract(cursor, tool.sel.start);
                for (Component* comp : tool.sel.comps)
                {
                    /*
                        * 0---1
                        * | 3-2
                        * 5-4
                        *
                        * 0 is repeated for 6
                        */
                    const Rectangle& a = comp->GetCasingA();
                    const Rectangle& b = comp->GetCasingB();

                    Vector2 points[7] = {
                        { a.x + offset.x,           a.y + offset.y            },
                        { b.x + offset.x + b.width, a.y + offset.y            },
                        { b.x + offset.x + b.width, b.y + offset.y + b.height },
                        { b.x + offset.x,           b.y + offset.y + b.height },
                        { b.x + offset.x,           a.y + offset.y + a.height },
                        { a.x + offset.x,           a.y + offset.y + a.height },
                        points[0]
                    };

                    DrawLineStrip(points, 7, MAGENTA);
                }
                for (Node* node : tool.sel.nodes)
                {
                    DrawCircleV(Vector2Add(node->GetPosition(), offset), 2.0f, MAGENTA);
                }
            }

            // Selection
            for (Component* comp : tool.sel.comps)
            {
                Rectangle rec;

                rec = comp->GetCasingA();
                rec.x -= 2.0f;
                rec.y -= 2.0f;
                rec.width += 4.0f;
                rec.height += 4.0f;
                DrawRectangleRec(rec, WHITE);

                rec = comp->GetCasingB();
                rec.x -= 2.0f;
                rec.y -= 2.0f;
                rec.width += 4.0f;
                rec.height += 4.0f;
                DrawRectangleRec(rec, WHITE);
            }
            for (Node* node : tool.sel.nodes)
            {
                DrawGateIcon(node->GetGate(), node->GetPosition(), g_nodeRadius + 2.0f, WHITE);
            }

            // Components
            Graph::Global().DrawComponents();

            // Hovered wire
            if (tool.g.hvr.wire.a && !tool.g.drawGateOptions) // Todo: find a good spot to store this
                DrawLineV(tool.g.hvr.wire.a->GetPosition(), tool.g.hvr.wire.b->GetPosition(), LIGHTGRAY);

            // Nodes
            Graph::Global().DrawNodes();

            // Cursor
            if (tool.g.hvr.node)
                DrawHighlightedGate(tool.g.hvr.node->GetPosition(), tool.g.hvr.node->GetGate());
            else
                DrawRectanglePro({ cursor.x,cursor.y,5,5 }, { 2.5,2.5 }, 45, RAYWHITE);

            // Gate options
            if (tool.g.drawGateOptions)
            {
                if (!!tool.pen.wireStart)
                    DrawHighlightedGate(tool.pen.wireStart->GetPosition(), tool.pen.wireStart->GetGate());

                GateSelection select = ClosestHoveredGateOption(cursor, tool.pen.wireStart->GetPosition());
                Gate pick;
                if (select.b_toggle)
                    pick = (tool.pen.wireStart->GetGate() != Gate::NOR ? Gate::NOR : Gate::OR);
                else
                    pick = GateSelection::gates[select.index];

                for (size_t i = 0; i < 4; ++i)
                {
                    constexpr float l = g_nodeRadius * 7.0f;
                    Vector2 center = Vector2Scale(GateSelection::sectors[i], l);
                    DrawCircleV(Vector2Add(tool.pen.wireStart->GetPosition(), center), g_nodeRadius * 3.0f + 2.0f, GRAY);
                    DrawCircleV(Vector2Add(tool.pen.wireStart->GetPosition(), center), g_nodeRadius * 3.0f, (pick == GateSelection::gates[i] ? GRAY : Color{ 30, 30, 30, 255 }));
                    DrawGateIcon(GateSelection::gates[i], Vector2Add(tool.pen.wireStart->GetPosition(), center), g_nodeRadius * 1.5f, WHITE);
                }
            }
            
            DrawRectangleRec(uiContainerRec, RAYWHITE);

            UIHandler::Global().Draw();

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    for (Node* node : Graph::Global().GetNodes())
    {
        delete node;
    }

    return 0;
}

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    Editor(windowWidth, windowHeight);

    CloseWindow();

	return 0;
}
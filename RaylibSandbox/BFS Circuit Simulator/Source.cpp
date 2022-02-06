#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <fstream>
#include <stack>
#include <queue>
#include <algorithm>
#include <unordered_map>

#include "Node.h"
#include "Component.h"
#include "Graph.h"

#define VECTOR_CONTAINS_ELEMENT(vector,element) (std::find((vector).begin(), (vector).end(), (element)) != (vector).end())

struct Action
{
    // todo
};
std::queue<Action> g_history;

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
        Vector2 direction = Vector2Normalize(cursor - origin);
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

enum class ToolType : char
{
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
};
class Tool
{
protected:
    static Component* hoveredComp;
    static Node* hoveredNode;
    static Wire hoveredWire;

public:
    inline static bool IsHoveringObject() { return hoveredNode || hoveredComp || hoveredWire; }

    // No event
    virtual void Tick()
    {
        if (hoveredComp = Graph::Global().FindComponentAtPosition(GetMousePosition()))
        {
            hoveredNode = nullptr;
            hoveredWire = WireNull();
        }
        else
        {
            if (hoveredNode = Graph::Global().FindNodeAtGridPoint(GetMousePosition()))
                hoveredWire = WireNull();
            else
                hoveredWire = Graph::Global().FindWireIntersectingGridPoint(GetMousePosition());
        }
    }

    // Left mouse
    virtual void Primary_Press() {}
    virtual void Primary_Release() {}
    // Right mouse
    virtual void Secondary_Press() {}
    virtual void Secondary_Release() {}
    // Middle mouse
    virtual void Tertiary_Press() {}
    virtual void Tertiary_Release() {}

    // Keyboard
    virtual void TickKeyboardPressCheck() {} // Check if any keys being listened for are pressed or released
    virtual void TickKeyboardReleaseCheck() {} // Check if any keys being listened for are pressed or released
    virtual void Key_Press(KeyboardKey key) {}
    virtual void Key_Release(KeyboardKey key) {}

    // Inputs
    virtual void SimulateTick()
    {
        // Pre-tick
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)  ) Primary_Press  ();
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) ) Secondary_Press();
        if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)) Tertiary_Press ();
        TickKeyboardPressCheck();

        // Tick
        Tick();

        // Post-tick
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)  ) Primary_Release  ();
        if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON) ) Secondary_Release();
        if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON)) Tertiary_Release ();
        TickKeyboardReleaseCheck();
    }
    virtual void DrawTick() {}
};
Component* Tool::hoveredComp = nullptr;
Node* Tool::hoveredNode = nullptr;
Wire Tool::hoveredWire = WireNull();

Rectangle TwoPointRec(Vector2 a, Vector2 b)
{
    auto [x1, x2] = std::minmax(a.x, b.x);
    auto [y1, y2] = std::minmax(a.y, b.y);
    return { x1, y1, x2 - x1, y2 - y1 };
}

class Tool_Select : public Tool
{
private:
    union
    {
        Vector2 selectionStart;
        Vector2 draggingStart;
    };
    bool selecting = false;
    bool dragging = false;
    std::vector<Node*> selectedNodes;
    std::vector<Component*> selectedComps;

public:
    Rectangle GetSelectionRec() const { return TwoPointRec(selectionStart, GetMousePosition()); }
    Vector2 GetDraggingOffset() const { return Vector2Subtract(GetMousePosition(), selectionStart); }

    // No event
    void Tick() override
    {
        // Don't look for a hovered node/component while selecting or dragging
        if (selecting)
            TickSelecting();
        else if (dragging)
            TickDragging();
        else
            Tool::Tick();
    }

    void StartSelecting()
    {
        // Hovered objects won't be getting updated until the selection is no longer active, so make sure they aren't needlessly and confusingly drawn
        hoveredComp = nullptr;
        hoveredNode = nullptr;
        selectionStart = GetMousePosition();
        selecting = true;
    }
    void TickSelecting()
    {
        Rectangle selectionRec = GetSelectionRec();

        // Select nodes
        {
            auto NodeIsInRec = [selectionRec](Node* node) {
                return CheckCollisionPointRec(node->GetPosition(), selectionRec);
            };

            const std::vector<Node*>& nodes = Graph::Global().GetNodes();

            selectedNodes.clear(); // We don't need to bother copying over the data to the new reservation
            selectedNodes.reserve(std::count_if(nodes.begin(), nodes.end(), NodeIsInRec));

            for (Node* node : Graph::Global().GetNodes())
            {
                if (NodeIsInRec(node)) selectedNodes.push_back(node);
            }
        }

        // Select components
        {
            auto CompIsInRec = [selectionRec](Component* comp) {
                return CheckCollisionRecs(selectionRec, comp->GetCasingA()) ||
                       CheckCollisionRecs(selectionRec, comp->GetCasingB());
            };

            const std::vector<Component*>& comps = Graph::Global().GetComponents();

            selectedComps.clear(); // We don't need to bother copying over the data to the new reservation
            selectedComps.reserve(std::count_if(comps.begin(), comps.end(), CompIsInRec));

            for (Component* comp : Graph::Global().GetComponents())
            {
                if (CompIsInRec(comp)) selectedComps.push_back(comp);
            }
        }
    }
    void FinishSelecting()
    {
        selecting = false;
    }

    void StartDragging()
    {
        draggingStart = GetMousePosition();
        dragging = true;
    }
    void TickDragging()
    {
        // Only important when drawing - which is not handled in here
    }
    void FinishDragging()
    {
        Vector2 offset = GetDraggingOffset();

        for (Component* comp : selectedComps)
        {
            comp->SetPosition(Vector2Add(comp->GetPosition(), offset));
        }
        for (Node* node : selectedNodes)
        {
            node->SetPosition(Vector2Add(node->GetPosition(), offset));
        }
        dragging = false;
    }

    // Left mouse
    void Primary_Press() override
    {
        if (hoveredNode || hoveredComp) // Drag if hovering a component in the selection
        {
            // Replace selection with hover
            {
                if (hoveredComp && !VECTOR_CONTAINS_ELEMENT(selectedComps, hoveredComp))
                {
                    selectedNodes.clear();
                    selectedComps.resize(1);
                    selectedComps.back() = hoveredComp;
                }
                else if (hoveredNode && !VECTOR_CONTAINS_ELEMENT(selectedNodes, hoveredNode))
                {
                    selectedComps.clear();
                    selectedNodes.resize(1);
                    selectedNodes.back() = hoveredNode;
                }
            }
            StartDragging();
        }
        else // Select
            StartSelecting();
    }
    void Primary_Release() override
    {
        if (selecting)
            FinishSelecting();
        else if (dragging)
            FinishDragging();
    }
};

class Tool_Pen : public Tool
{

};

class Tool_Direct : public Tool
{
};

class Tool_Edit : public Tool
{
};

class Tool_Camera : public Tool
{
};

class ToolHandler
{
private:
    ToolHandler()
    {
        m_toolType = ToolType::Select;
        m_toolControl = new Tool_Select;
    }
    ToolType m_toolType;
    Tool* m_toolControl;

public:
    ~ToolHandler()
    {
        delete m_toolControl;
    }
    static ToolHandler& Global()
    {
        static ToolHandler singleton;
        return singleton;
    }

    void SetToolType(ToolType type)
    {
        if (m_toolType != type)
        {
            m_toolType = type;
            delete m_toolControl; // There should ALWAYS be a valid pointer here until the program closes
            switch (type)
            {
            default: // Select is default
            case ToolType::Select:  m_toolControl = new Tool_Select;    break;
            case ToolType::Pen:     m_toolControl = new Tool_Pen;       break;
            case ToolType::Direct:  m_toolControl = new Tool_Direct;    break;
            case ToolType::Edit:    m_toolControl = new Tool_Edit;      break;
            case ToolType::Camera:  m_toolControl = new Tool_Camera;    break;
            }
        }
    }
    ToolType GetToolType() const
    {
        return m_toolType;
    }
    const Tool* GetTool() const
    {
        return m_toolControl;
    }
};

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    bool graphDirty = false;
    bool gateDirty = false;
    bool drawGateOptions = false;

    Node* selectionStart = nullptr;
    Node* selectionEnd = nullptr;

    bool marqueeSelecting = false;
    bool marqueeDragging = false;
    Vector2 marqueeStart = Vector2Zero();
    Rectangle selectionRec;
    std::vector<Selectable> selection;
    bool holdingTemporaryComponent = false; // If move is cancelled while this is true, we need to clean up the component

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
        if (selectionStart)
            cursor = ConstrainOffset(selectionStart->GetPosition(), cursor);

        // Hover
        Node* hoveredNode = Graph::Global().FindNodeAtGridPoint(cursor);
        Wire hoveredWire = (hoveredNode ? WireNull()  : Graph::Global().FindWireIntersectingGridPoint(cursor));

        if (!marqueeSelecting)
        {
            if (!selection.empty())
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    if (!holdingTemporaryComponent)
                        marqueeStart = cursor;
                    marqueeDragging = true;
                }

                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                {
                    marqueeDragging = false;
                    Vector2 offset = Vector2Subtract(cursor, marqueeStart);

                    // Copy
                    if (IsModifierDown(KEY_ALT) && !holdingTemporaryComponent)
                        Graph::Global().CloneNodesWithOffset(selection, offset);
                    // Move
                    else
                    {
                        Graph::Global().OffsetNodes(selection, offset);

                        // todo: ????
                        if (holdingTemporaryComponent)
                            holdingTemporaryComponent = false;
                    }
                }
            }
            else
            {
                // Drag
                if (selectionStart)
                {
                    selectionEnd = hoveredNode;

                    // Finish making connection
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                    {
                        if (selectionEnd != selectionStart)
                        {
                            if (!selectionEnd)
                                selectionEnd = Graph::Global().AddNewNode(cursor);

                            Graph::Global().ConnectNodes(selectionStart, selectionEnd);
                            graphDirty = true;

                            // Divide wire
                            if (hoveredWire)
                            {
                                Graph::Global().DivideWire(hoveredWire, selectionEnd);
                                hoveredWire = WireNull();
                            }
                        }
                        selectionStart = selectionEnd = nullptr;
                    }
                }
                // Create new node
                else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !selectionStart)
                {
                    selectionStart = hoveredNode;

                    if (!selectionStart)
                    {
                        selectionStart = Graph::Global().AddNewNode(cursor);
                        graphDirty = true;

                        // Divide wire
                        if (hoveredWire)
                        {
                            graph.DivideWire(hoveredWire, selectionStart);
                            hoveredWire = WireNull();
                        }
                    }
                }

                drawGateOptions = (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON) && selectionStart);
                if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON) && !selectionStart)
                {
                    if (hoveredNode)
                        selectionStart = hoveredNode;
                    else if (hoveredWire)
                    {
                        hoveredWire = Graph::Global().ReverseWire(hoveredWire);
                        graphDirty = true;
                    }
                }
                if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON) && selectionStart)
                {

                    // Middle click
                    if (Vector2Equals(cursor, selectionStart->GetPosition()))
                    {
                        selectionStart->SetGate(selectionStart->GetGate() != Gate::NOR ? Gate::NOR : Gate::OR);
                    }
                    // Middle drag
                    else
                    {
                        // In clockwise order
                        GateSelection select = ClosestHoveredGateOption(cursor, selectionStart->GetPosition());
                        selectionStart->SetGate(GateSelection::gates[select.index]);
                    }

                    // Inputless nodes take output from the user and therefore are treated as mutable
                    if (!selectionStart->HasNoInputs())
                        gateDirty = true;
                    else
                        selectionStart->SetState(selectionStart->GetGate() == Gate::NOR);

                    selectionStart = selectionEnd = nullptr;
                }
            }
        }

        // Delete selected
        if ((IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_BACKSPACE)) && !selection.empty())
        {
            for (Selectable element : selection)
            {
                if (element.type == Selectable::Type::COMP)
                    Graph::Global().RemoveComponentEntirely(element.comp);
                else
                    Graph::Global().RemoveNodeEntirely(element.node);
            }
            selection.clear();
        }

        // Make sure no other locally stored nodes can possibly exist at the time, or set them to nullptr
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && !selectionStart && !selectionEnd && !marqueeSelecting)
        {
            if (hoveredNode)
            {
                bool removed;

                // Transfer inputs to outputs
                if (IsModifierDown(KEY_SHIFT) &&
                    !(hoveredNode->InputCount () == 1 &&
                      hoveredNode->OutputCount() == 1))
                {
                    hoveredNode->SetGate(Gate::OR);
                }
                else
                {
                    if (IsModifierDown(KEY_SHIFT))
                        Graph::Global().ConnectNodes(hoveredNode->GetInput(0), hoveredNode->GetOutput(0));

                    Graph::Global().RemoveNodeEntirely(hoveredNode);
                    hoveredNode = nullptr;
                }

                graphDirty = true;
            }
            else if (hoveredWire)
            {
                if (IsModifierDown(KEY_SHIFT)) // Also delete the related nodes
                {
                    Graph::Global().RemoveNodeEntirely(hoveredWire.a);
                    Graph::Global().RemoveNodeEntirely(hoveredWire.b);
                }
                else
                    Graph::Global().DisconnectNodes(hoveredWire.a, hoveredWire.b);

                hoveredWire = WireNull();
                graphDirty = true;
            }
            else
            {
                marqueeStart = cursor;
                marqueeSelecting = true;
            }
        }
        if (marqueeSelecting)
        {
            Vector2 a = {
                std::min(marqueeStart.x, cursor.x),
                std::min(marqueeStart.y, cursor.y)
            };
            selectionRec = {
                    a.x,
                    a.y,
                    std::max(marqueeStart.x, cursor.x) - a.x,
                    std::max(marqueeStart.y, cursor.y) - a.y,
            };
            Graph::Global().FindSelectablesInRectangle(&selection, selectionRec);

            // Finalize
            if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON))
                marqueeSelecting = false;
        }

        // Package up components
        if (IsKeyPressed(KEY_SPACE) && selection.size() > 1)
        {
            size_t nodeCount = selection.size();
            for (Selectable element : selection)
            {
                if (element.type == Selectable::Type::COMP)
                {
                    nodeCount += element.comp->GetNodeCount() - 1;
                }
            }
            std::vector<Node*> breakdown;
            breakdown.reserve(nodeCount);
            for (Selectable element : selection)
            {
                if (element.type == Selectable::Type::COMP)
                {
                    for (Node* node : element.comp->GetNodes())
                    {
                        breakdown.push_back(node);
                    }
                }
                else
                {
                    breakdown.push_back(element.node);
                }
            }

            ComponentBlueprint* blueprint = new ComponentBlueprint();
            blueprint->GenerateBlueprintFromSelection(breakdown);
            selection.clear();
            marqueeStart = { g_gridUnit * -6, 0 };
            Component* clone = Graph::Global().CloneComponentAtPosition(blueprint, marqueeStart); // Place the component offscreen
            selection.push_back(Selectable(clone));
            holdingTemporaryComponent = true;
            graphDirty = true;
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

            if (marqueeSelecting)
            {
                DrawRectangleRec(selectionRec, ColorAlpha(GRAY, 0.125));
                DrawRectangleLines((int)lroundf(selectionRec.x), (int)lroundf(selectionRec.y), (int)lroundf(selectionRec.width), (int)lroundf(selectionRec.height), GRAY);
            }


            // Current wire being made
            if (selectionStart)
            {
                Vector2 end;
                if (selectionEnd)
                    end = selectionEnd->GetPosition();
                else
                    end = cursor;

                DrawLineV(selectionStart->GetPosition(), end, GRAY);

                // Cursor grid
                Vector2 p = selectionStart->GetPosition();
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
                        DrawPixelV((p + directions[i] * s) + (directions[i] * g_gridUnit * (float)o), color);
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
                        DrawPixelV(cursor + offsets[i], color);
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
                        DrawPixelV(cursor + offset, ColorAlpha(WHITE, alpha));
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
            if (marqueeDragging || holdingTemporaryComponent)
            {
                Vector2 offset = cursor - marqueeStart;
                for (Selectable element : selection)
                {
                    if (element.type == Selectable::Type::COMP)
                    {
                        /*
                        * 0---1
                        * | 3-2
                        * 5-4
                        * 
                        * 0 is repeated for 6
                        */
                        const Rectangle& a = element.comp->GetCasingA();
                        const Rectangle& b = element.comp->GetCasingB();

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
                    else
                        DrawCircleV(element.node->GetPosition() + offset, 2.0f, MAGENTA);
                }
            }

            // Selection
            for (Selectable element : selection)
            {
                if (element.type == Selectable::Type::COMP)
                {
                    Rectangle rec;

                    rec = element.comp->GetCasingA();
                    rec.x -= 2.0f;
                    rec.y -= 2.0f;
                    rec.width += 4.0f;
                    rec.height += 4.0f;
                    DrawRectangleRec(rec, WHITE);

                    rec = element.comp->GetCasingB();
                    rec.x -= 2.0f;
                    rec.y -= 2.0f;
                    rec.width += 4.0f;
                    rec.height += 4.0f;
                    DrawRectangleRec(rec, WHITE);
                }
                else
                    DrawGateIcon(element.node->GetGate(), element.node->GetPosition(), g_nodeRadius + 2.0f, WHITE);
            }

            // Components
            Graph::Global().DrawComponents();

            // Hovered wire
            if (hoveredWire.a && !drawGateOptions)
                DrawLineV(hoveredWire.a->GetPosition(), hoveredWire.b->GetPosition(), LIGHTGRAY);

            // Nodes
            Graph::Global().DrawNodes();

            // Cursor
            if (hoveredNode)
                DrawHighlightedGate(hoveredNode->GetPosition(), hoveredNode->GetGate());
            else
                DrawRectanglePro({ cursor.x,cursor.y,5,5 }, { 2.5,2.5 }, 45, RAYWHITE);

            // Gate options
            if (drawGateOptions)
            {
                if (selectionStart)
                    DrawHighlightedGate(selectionStart->GetPosition(), selectionStart->GetGate());

                GateSelection select = ClosestHoveredGateOption(cursor, selectionStart->GetPosition());
                Gate pick;
                if (select.b_toggle)
                    pick = (selectionStart->GetGate() != Gate::NOR ? Gate::NOR : Gate::OR);
                else
                    pick = GateSelection::gates[select.index];

                for (size_t i = 0; i < 4; ++i)
                {
                    constexpr float l = g_nodeRadius * 7.0f;
                    Vector2 center = GateSelection::sectors[i] * l;
                    DrawCircleV(selectionStart->GetPosition() + center, g_nodeRadius * 3.0f + 2.0f, GRAY);
                    DrawCircleV(selectionStart->GetPosition() + center, g_nodeRadius * 3.0f, (pick == GateSelection::gates[i] ? GRAY : Color{ 30, 30, 30, 255 }));
                    DrawGateIcon(GateSelection::gates[i], selectionStart->GetPosition() + center, g_nodeRadius * 1.5f, WHITE);
                }
            }

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    for (Node* node : Graph::Global().GetNodes())
    {
        delete node;
    }

    CloseWindow();

	return 0;
}
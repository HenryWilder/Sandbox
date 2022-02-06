#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <fstream>
#include <stack>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include "Node.h"
#include "Component.h"
#include "Graph.h"
#include "Tool.h"

#define VECTOR_CONTAINS_ELEMENT(vector,element) (std::find((vector).begin(), (vector).end(), (element)) != (vector).end())
#define SET_CONTAINS_ELEMENT(set,element) ((set).find((element)) != (set).end())

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


// No event
void Tool::Tick()
{
    if (s_hoveredComp = Graph::Global().FindComponentAtPosition(GetMousePosition()))
    {
        s_hoveredNode = nullptr;
        s_hoveredWire = WireNull();
    }
    else
    {
        if (s_hoveredNode = Graph::Global().FindNodeAtGridPoint(GetMousePosition()))
            s_hoveredWire = WireNull();
        else
            s_hoveredWire = Graph::Global().FindWireIntersectingGridPoint(GetMousePosition());
    }
}

void Tool::Primary_Press()
{
}

void Tool::Primary_Release()
{
}

void Tool::Secondary_Press()
{
}

void Tool::Secondary_Release()
{
}

void Tool::Tertiary_Press()
{
}

void Tool::Tertiary_Release()
{
}

void Tool::TickKeyboardPressCheck()
{
}

void Tool::TickKeyboardReleaseCheck()
{
}

// Inputs

void Tool::SimulateTick()
{
    // Pre-tick
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) Primary_Press();
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) Secondary_Press();
    if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)) Tertiary_Press();
    TickKeyboardPressCheck();

    // Tick
    Tick();

    // Post-tick
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) Primary_Release();
    if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) Secondary_Release();
    if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON)) Tertiary_Release();
    TickKeyboardReleaseCheck();
}

// Basic draw
void Tool::DrawTick()
{
    
}

Component* Tool::s_hoveredComp = nullptr;
Node* Tool::s_hoveredNode = nullptr;
Wire Tool::s_hoveredWire = WireNull();

ComponentBlueprint* Tool::s_activeBlueprint = nullptr;
Gate Tool::s_activeGate = Gate::OR;


inline Rectangle Tool_Select::GetSelectionRec() const { return TwoPointRec(m_selectionStart, GetMousePosition()); }

inline Vector2 Tool_Select::GetDraggingOffset() const { return Vector2Subtract(GetMousePosition(), m_selectionStart); }

// No event

void Tool_Select::Tick()
{
    // Don't look for a hovered node/component while selecting or dragging
    if (m_selecting)
        TickSelecting();
    else if (!m_dragging)
        Tool::Tick();
}

void Tool_Select::StartSelecting()
{
    // Hovered objects won't be getting updated until the selection is no longer active, so make sure they aren't needlessly and confusingly drawn
    s_hoveredComp = nullptr;
    s_hoveredNode = nullptr;
    m_selectionStart = GetMousePosition();
    m_selecting = true;
}

void Tool_Select::TickSelecting()
{
    Rectangle selectionRec = GetSelectionRec();

    // Select nodes
    {
        auto NodeIsInRec = [selectionRec](Node* node) {
            return CheckCollisionPointRec(node->GetPosition(), selectionRec);
        };

        const std::vector<Node*>& nodes = Graph::Global().GetNodes();

        m_selectedNodes.clear(); // We don't need to bother copying over the data to the new reservation
        m_selectedNodes.reserve(std::count_if(nodes.begin(), nodes.end(), NodeIsInRec));

        for (Node* node : Graph::Global().GetNodes())
        {
            if (NodeIsInRec(node)) m_selectedNodes.push_back(node);
        }
    }

    // Select components
    {
        auto CompIsInRec = [selectionRec](Component* comp) {
            return CheckCollisionRecs(selectionRec, comp->GetCasingA()) ||
                CheckCollisionRecs(selectionRec, comp->GetCasingB());
        };

        const std::vector<Component*>& comps = Graph::Global().GetComponents();

        m_selectedComps.clear(); // We don't need to bother copying over the data to the new reservation
        m_selectedComps.reserve(std::count_if(comps.begin(), comps.end(), CompIsInRec));

        for (Component* comp : Graph::Global().GetComponents())
        {
            if (CompIsInRec(comp)) m_selectedComps.push_back(comp);
        }
    }
}

void Tool_Select::FinishSelecting()
{
    m_selecting = false;
}

void Tool_Select::StartDragging()
{
    m_draggingStart = GetMousePosition();
    m_dragging = true;
}

void Tool_Select::FinishDragging()
{
    Vector2 offset = GetDraggingOffset();

    for (Component* comp : m_selectedComps)
    {
        comp->SetPosition(Vector2Add(comp->GetPosition(), offset));
    }
    for (Node* node : m_selectedNodes)
    {
        node->SetPosition(Vector2Add(node->GetPosition(), offset));
    }
    m_dragging = false;
}

void Tool_Select::DeleteSelected()
{
    // Components
    Graph::Global().RemoveMultipleComponents(m_selectedComps);
    m_selectedComps.clear();
    // Nodes
    Graph::Global().RemoveMultipleNodes(m_selectedNodes);
    m_selectedNodes.clear();
}

// Left mouse

void Tool_Select::Primary_Press()
{
    if (s_hoveredNode || s_hoveredComp) // Drag if hovering a component in the selection
    {
        // Replace selection with hover
        {
            if (s_hoveredComp && !VECTOR_CONTAINS_ELEMENT(m_selectedComps, s_hoveredComp))
            {
                m_selectedNodes.clear();
                m_selectedComps.resize(1);
                m_selectedComps.back() = s_hoveredComp;
            }
            else if (s_hoveredNode && !VECTOR_CONTAINS_ELEMENT(m_selectedNodes, s_hoveredNode))
            {
                m_selectedComps.clear();
                m_selectedNodes.resize(1);
                m_selectedNodes.back() = s_hoveredNode;
            }
        }
        StartDragging();
    }
    else // Select
        StartSelecting();
}

void Tool_Select::Primary_Release()
{
    if (m_selecting)
        FinishSelecting();
    else if (m_dragging)
        FinishDragging();
}

// Keyboard

void Tool_Select::TickKeyboardPressCheck()
{
    if (IsKeyPressed(KEY_DELETE))
        DeleteSelected();
}

void Tool_Pen::Primary_Press()
{
}

void Tool_Pen::Primary_Release()
{
}

ToolHandler::ToolHandler()
{
    m_toolType = ToolType::Select;
    m_tool = new Tool_Select;
}

ToolHandler::~ToolHandler()
{
    delete m_tool;
}

ToolHandler& ToolHandler::Global()
{
    static ToolHandler singleton;
    return singleton;
}

void ToolHandler::SetToolType(ToolType type)
{
    if (m_toolType != type)
    {
        m_toolType = type;
        delete m_tool; // There should ALWAYS be a valid pointer here until the program closes
        switch (type)
        {
        default: // Select is default
        case ToolType::Select:  m_tool = new Tool_Select;    break;
        case ToolType::Pen:     m_tool = new Tool_Pen;       break;
        case ToolType::Direct:  m_tool = new Tool_Direct;    break;
        case ToolType::Edit:    m_tool = new Tool_Edit;      break;
        case ToolType::Camera:  m_tool = new Tool_Camera;    break;
        }
    }
}

ToolType ToolHandler::GetToolType() const
{
    return m_toolType;
}

const Tool* ToolHandler::GetTool() const
{
    return m_tool;
}

void ToolHandler::SimulateTick() { m_tool->SimulateTick(); }

inline void ToolHandler::DrawTick() { m_tool->DrawTick(); }

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


void Tool::UpdateCursorPos()
{
    m_cursorPosition.x = roundf((float)GetMouseX() / g_gridUnit) * g_gridUnit;
    m_cursorPosition.y = roundf((float)GetMouseY() / g_gridUnit) * g_gridUnit;
}

// No event
void Tool::Tick()
{
    m_hoveredComp = Graph::Global().FindComponentAtPosition(m_cursorPosition);
    if (!!m_hoveredComp)
    {
        m_hoveredNode = nullptr;
        m_hoveredWire = WireNull();
    }
    else
    {
        m_hoveredNode = Graph::Global().FindNodeAtGridPoint(m_cursorPosition);
        if (!!m_hoveredNode)
            m_hoveredWire = WireNull();
        else
            m_hoveredWire = Graph::Global().FindWireIntersectingGridPoint(m_cursorPosition);
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
}

// Basic draw
void Tool::DrawTick()
{
    
}

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
    m_hoveredComp = nullptr;
    m_hoveredNode = nullptr;
    m_selectionStart = GetMousePosition();
    m_selecting = true;
    if (!IsModifierDown(KEY_SHIFT))
    {
        m_selectedComps.clear();
        m_selectedNodes.clear();
    }
}

void Tool_Select::TickSelecting()
{
    // TODO: Add to selection instead of just creating the selection each tick

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
    if (m_hoveredNode || m_hoveredComp) // Drag if hovering a component in the selection
    {
        // Replace selection with hover
        {
            if (m_hoveredComp && !VECTOR_CONTAINS_ELEMENT(m_selectedComps, m_hoveredComp))
            {
                m_selectedNodes.clear();
                m_selectedComps.resize(1);
                m_selectedComps.back() = m_hoveredComp;
            }
            else if (m_hoveredNode && !VECTOR_CONTAINS_ELEMENT(m_selectedNodes, m_hoveredNode))
            {
                m_selectedComps.clear();
                m_selectedNodes.resize(1);
                m_selectedNodes.back() = m_hoveredNode;
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


Node* Tool_Pen::GetStartNode() const
{
    return m_wireStart;
}

void Tool_Pen::Tick()
{

    Tool::Tick();
}

void Tool_Pen::Primary_Press()
{
    // The node that the current wire should end at
    Node* endNode;

    if (!!m_hoveredNode) // There is a node being hovered (make that the end node)
        endNode = m_hoveredNode;
    else // No node at the clicked position (create one)
        endNode = Graph::Global().AddNewNode(GetMousePosition());

    // We are continuing an existing wire
    if (!!m_wireStart)
        Graph::Global().ConnectNodes(m_wireStart, endNode);

    // Transfer this node to be the start of the next wire
    m_wireStart = endNode;
}

void Tool_Pen::Secondary_Press()
{
    // Stop wire
    m_wireStart = nullptr;
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

#include <raylib.h>
#pragma warning(push)
#pragma warning(disable : 26451)
#include <raymath.h>
#pragma warning(pop)
#include <vector>
#include <fstream>
#include <stack>
#include <algorithm>
#include <unordered_map>

#include "Node.h"

Node::Node(Vector2 position, Gate type) : m_position(position), m_type(type), b_state(false), b_visited(false), b_hidden(false), m_componentFlags(0b00), m_container(nullptr), m_inputs{}, m_outputs{} {}

Vector2 Node::GetPosition() const
{
    return m_position;
}
void Node::SetPosition(Vector2 position)
{
    m_position = position;
}

IVec2 Node::GetPos() const
{
    return pos;
}

Gate Node::GetGate() const
{
    return m_type;
}
void Node::SetPos(IVec2 pos)
{

}
void Node::SetGate(Gate type)
{
    m_type = type;
}

bool Node::GetState() const
{
    return b_state;
}
void Node::SetState(bool state)
{
    b_state = state;
}

bool Node::GetVisited() const
{
    return b_visited;
}
void Node::SetVisited(bool visited) const
{
    b_visited = visited;
}

bool Node::GetHidden() const
{
    return b_hidden;
}
void Node::SetHidden(bool hidden)
{
    b_hidden = hidden;
}

bool Node::IsComponentExternal() const
{
    return m_componentFlags != 0b00 && m_componentFlags != 0b11;
}
bool Node::IsComponentInput() const
{
    return m_componentFlags == 0b01;
}
bool Node::IsComponentOutput() const
{
    return m_componentFlags == 0b10;
}
bool Node::IsInComponent() const
{
    return m_componentFlags != 0b00;
}
void Node::SetNotInComponent()
{
    m_componentFlags = 0b00;
}
void Node::SetComponentInput()
{
    m_componentFlags = 0b01;
}
void Node::SetComponentOutput()
{
    m_componentFlags = 0b10;
}
void Node::SetComponentInternal()
{
    m_componentFlags = 0b11;
}


Component* Node::GetContainingComponent() const
{
    return m_container;
}
void Node::SetContainingComponent(Component* comp)
{
    m_container = comp;
}

bool Node::HasNoInputs() const
{
    return m_inputs.empty();
}
bool Node::InputCount() const
{
    return m_inputs.size();
}
const std::vector<Node*>& Node::GetInputs() const
{
    return m_inputs;
}
Node* Node::GetInput(size_t index) const
{
    return m_inputs[index];
}
void Node::AddInput(Node* input)
{
    if (!input)
        return;

    if (HasNodeAsInput(input)) // Prevent duplicates
        m_inputs.push_back(input);
}
void Node::RemoveInput(const Node* input)
{
    if (!input)
        return;

    auto it = std::find(m_inputs.begin(), m_inputs.end(), input);
    if (it != m_inputs.end())
        m_inputs.erase(it);
}

bool Node::HasNodeAsInput(const Node* input) const
{
    return IsNodeInVector(m_inputs, input);
}

bool Node::HasNoOutputs() const
{
    return m_outputs.empty();
}
bool Node::OutputCount() const
{
    return m_outputs.size();
}
const std::vector<Node*>& Node::GetOutputs() const
{
    return m_outputs;
}
Node* Node::GetOutput(size_t index) const
{
    return m_outputs[index];
}

void Node::AddOutput(Node* output)
{
    if (!output)
        return;

    if (HasNodeAsOutput(output)) // Prevent duplicates
        m_outputs.push_back(output);
}
void Node::RemoveOutput(const Node* output)
{
    if (!output)
        return;

    auto it = std::find(m_outputs.begin(), m_outputs.end(), output);
    if (it != m_outputs.end())
        m_outputs.erase(it);
}

bool Node::HasNodeAsOutput(const Node* output) const
{
    return IsNodeInVector(m_outputs, output);
}

void Node::ClearReferencesToSelf()
{
    for (Node* input : m_inputs)
    {
        input->RemoveOutput(this);
    }
    m_inputs.clear();

    for (Node* output : m_outputs)
    {
        output->RemoveInput(this);
    }
    m_outputs.clear();
}

void DrawGateIcon(Gate type, Vector2 center, float radius, Color color, bool drawXORline)
{
    switch (type)
    {
    case Gate::XOR:
    case Gate::OR:
    {
        static const float b = sqrt(3.0f) / 3.0f;
        static const float a = b * 2.0f;
        static const Vector2 offsets[3] = {
            {  0.0f, -a },
            { -1.0f, +b },
            { +1.0f, +b }
        };

        // Compensate for line drawing outside of triangle instead of inside
        if (type == Gate::XOR && drawXORline)
            radius += 1.0f;

        Vector2 points[3];
        for (size_t i = 0; i < 3; ++i)
        {
            points[i] = Vector2Add(center, Vector2Scale(offsets[i], radius));
        }

        if (type == Gate::XOR && drawXORline)
        {
            DrawTriangleLines(points[0], points[1], points[2], color);
            DrawGateIcon(Gate::OR, center, radius - 2.0f, color);
        }
        else
            DrawTriangle(points[0], points[1], points[2], color);

    }
    break;

    case Gate::NOR:
        DrawCircleV(center, radius - 1, color);
        break;

    case Gate::AND:
    {
        radius -= sqrt(2.0f);
        float diameter = radius * 2.0f;
        DrawRectangleV(Vector2SubtractValue(center, radius), Vector2{ diameter, diameter }, color);
    }
    break;
    }
}

void DrawHighlightedGate(Vector2 position, Gate type)
{
    constexpr float outerEdge = g_nodeRadius * 4.0f;
    constexpr float innerEdge = g_nodeRadius * 3.0f;
    constexpr Vector2 origin1 = { g_nodeRadius * 2.0f, g_nodeRadius * 2.0f };
    constexpr Vector2 origin2 = { g_nodeRadius * 1.5f, g_nodeRadius * 1.5f };
    Rectangle outerRec = {
        position.x,
        position.y,
        outerEdge,
        outerEdge
    };
    Rectangle innerRec = {
        position.x,
        position.y,
        innerEdge,
        innerEdge
    };
    DrawRectanglePro(outerRec, origin1, 45, WHITE);
    DrawRectanglePro(innerRec, origin2, 45, BLACK);

    DrawGateIcon(type, position, g_nodeRadius + 2.0f, GRAY);
}


bool IsNodeInVector(const std::vector<Node*>& vector, const Node* node)
{
    return std::find(vector.begin(), vector.end(), node) != vector.end();
}

bool FindNodeIndexInVector(const std::vector<Node*>& vector, const Node* node, size_t* index)
{
    auto it = std::find(vector.begin(), vector.end(), node);
    *index = it - vector.begin();
    return it != vector.end();
}

size_t NodeIndexInVector(const std::vector<Node*>& vector, const Node* node)
{
    return std::find(vector.begin(), vector.end(), node) - vector.begin();
}

void MapNodeRelativeIndices(const std::vector<Node*>& source, std::unordered_map<const Node*, size_t>& dest)
{
    dest.reserve(source.size());
    for (size_t i = 0; i < source.size(); ++i)
    {
        dest.insert({ source[i],i });
    }
}

// Test if a wire might not be valid anymore
bool CheckWireIntegrity(Wire wire)
{
    return
        (bool)wire &&
        wire.a->HasNodeAsOutput(wire.b) &&
        wire.b->HasNodeAsInput(wire.a);
}

void ListWireRelativeIndices(const std::vector<Node*>& source, std::vector<WireRelative>& dest)
{
    std::unordered_map<const Node*, size_t> map;
    MapNodeRelativeIndices(source, map);

    size_t totalWires = 0;
    for (const Node* start : source)
    {
        for (const Node* end : start->GetOutputs())
        {
            if (map.find(end) != map.end())
                ++totalWires;
        }
    }
    dest.reserve(totalWires);

    for (const Node* start : source)
    {
        size_t startIndex = map.find(start)->second;
        for (const Node* end : start->GetOutputs())
        {
            auto it = map.find(end);
            if (it == map.end())
                continue;

            size_t endIndex = it->second;
            dest.push_back({ startIndex, endIndex });
        }
    }
}

Connector::Connector(const Connector& base)
{
    b_node = base.b_node;
    if (b_node) n = base.n;
    else c = base.c;
}

bool Connector::IsNode() const
{
    return b_node;
}

Node* Connector::GetAsNode() const
{
    return n;
}

CompIO* Connector::GetAsCompIO() const
{
    return c;
}

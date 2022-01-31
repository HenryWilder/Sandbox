#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <fstream>
#include <stack>
#include <algorithm>
#include <unordered_map>

#include "Node.h"
#include "Component.h"

bool NodeBP_Helper::operator<(const NodeBP_Helper& other) const
{
    return position.y < other.position.y;
}

void ComponentBlueprint::GenerateBlueprintFromSelection(const std::vector<Node*>& source)
{
    std::vector<NodeBP_Helper*> helpers;
    helpers.reserve(source.size());

    inputs = 0;
    outputs = 0;

    // Write node data
    for (Node* src : source)
    {
        src->SetVisited(false);
        helpers.push_back(new NodeBP_Helper{ src->GetGate(), src->GetPosition() });

        if (src->HasNoInputs())
        {
            helpers.back()->noInputs = true;
            ++inputs;
        }
        else
        {
            for (Node* input : src->GetInputs())
            {
                if (std::find(src->GetInputs().begin(), src->GetInputs().end(), input) == src->GetInputs().end())
                {
                    helpers.back()->noInputs = true;
                    ++inputs;
                    break;
                }
            }
        }

        if (src->HasNoOutputs())
        {
            helpers.back()->noOutputs = true;
            ++outputs;
        }
        else
        {
            for (Node* input : src->GetOutputs())
            {
                if (std::find(src->GetOutputs().begin(), src->GetOutputs().end(), input) == src->GetOutputs().end())
                {
                    helpers.back()->noOutputs = true;
                    ++outputs;
                    break;
                }
            }
        }
    }

    // Write wire data as relative indices
    for (Node* start : source)
    {
        if (start->GetVisited())
            continue;

        // Find the index of the start node
        size_t a = std::find(source.begin(), source.end(), start) - source.begin();

        // We don't need to look at the inputs because that information is implied by the outputs
        for (Node* end : start->GetOutputs())
        {
            if (end->GetVisited())
                continue;

            // Find the index of the end node
            auto it = std::find(source.begin(), source.end(), end);

            // Write the wire
            if (it != source.end())
            {
                size_t b = it - source.begin();
                helpers[a]->outputs.push_back(helpers[b]);
            }
        }
    }

    // Sort
    std::partition(helpers.begin(), helpers.end(), [](const NodeBP_Helper* a) { return a->noInputs; });
    std::partition(helpers.begin() + inputs, helpers.end(), [](const NodeBP_Helper* a) { return a->noOutputs; });

    std::sort(helpers.begin(), helpers.begin() + inputs);
    std::sort(helpers.begin() + inputs, helpers.begin() + (inputs + outputs));

    nodes.reserve(source.size());

    for (NodeBP_Helper* help : helpers)
    {
        nodes.push_back(NodeBlueprint{ help->gate });
        nodes.back().outputs.reserve(help->outputs.size());
        for (NodeBP_Helper* next : help->outputs)
        {
            auto it = std::find(helpers.begin(), helpers.end(), next);
            size_t outputIndex = it - helpers.begin();
            nodes.back().outputs.push_back(outputIndex);
        }
    }

    for (NodeBP_Helper* help : helpers)
    {
        delete help;
    }
}


ComponentBlueprint* const Component::GetBlueprint() const
{
    return m_blueprint;
}
void Component::SetBlueprint(ComponentBlueprint* blueprint)
{
    m_blueprint = blueprint;
}
// Parameters are vectors of the nodes that need to be added/removed
void Component::Regenerate(std::vector<Node*>* add, std::vector<Node*>* remove)
{
    m_inputs = m_blueprint->inputs;
    m_outputs = m_blueprint->outputs;

    long difference = (long)m_blueprint->nodes.size() - (long)m_nodes.size();

    // Nodes that need to be removed
    if (difference < 0)
    {
        remove->reserve(remove->size() - difference);
        for (size_t i = m_blueprint->nodes.size() - 1; i < m_nodes.size(); ++i)
        {
            remove->push_back(m_nodes[i]);
        }
    }

    // Save space for the nodes -- some can be reused, while the excess get dropped off.
    // Do not worry about leaking the excess, as their pointers have been saved above for external removal.
    m_nodes.reserve(m_blueprint->nodes.size());

    // Modify
    for (size_t i = 0; i < m_nodes.size(); ++i)
    {
        m_nodes[i]->SetGate(m_blueprint->nodes[i].gate);
        m_nodes[i]->ClearReferencesToSelf(); // Make this node effectively a new one
    }
    // Create
    for (size_t i = m_nodes.size(); i < m_blueprint->nodes.size(); ++i)
    {
        Node* node = new Node(Vector2Zero(), m_blueprint->nodes[i].gate);
        m_nodes.push_back(node);
    }

    // At this point, m_nodes and m_blueprint->nodes have the same size.

    // Nodes that need to be added
    if (difference > 0)
    {
        add->reserve(add->size() + difference);
        for (size_t i = m_blueprint->nodes.size() - difference; i < m_blueprint->nodes.size(); ++i)
        {
            add->push_back(m_nodes[i]);
        }
    }

    // Add connections
    // Adding/removing nodes requires permission from the Graph structure, but wires only require the nodes.
    for (size_t i = 0; i < m_nodes.size(); ++i)
    {
        for (size_t outputIndex : m_blueprint->nodes[i].outputs)
        {
            m_nodes[i]->AddOutput(m_nodes[outputIndex]);
            m_nodes[outputIndex]->AddInput(m_nodes[i]);
        }
    }

    // Hide internals
    for (size_t i = 0; i < m_inputs; ++i)
    {
        m_nodes[i]->SetHidden(false);
        m_nodes[i]->SetComponentInput();
    }
    for (size_t i = m_inputs; i < m_inputs + m_outputs; ++i)
    {
        m_nodes[i]->SetHidden(false);
        m_nodes[i]->SetComponentOutput();
    }
    for (size_t i = m_inputs + m_outputs; i < m_nodes.size(); ++i)
    {
        m_nodes[i]->SetHidden(true);
        m_nodes[i]->SetComponentInternal();
    }
}

size_t Component::GetInputCount() const
{
    return m_inputs;
}
size_t Component::GetOutputCount() const
{
    return m_outputs;
}
size_t Component::GetInternalCount() const
{
    return m_nodes.size() - (m_inputs + m_outputs);
}
size_t Component::GetNodeCount() const
{
    return m_nodes.size();
}

Rectangle Component::GetCasingA() const
{
    return m_casingLeft;
}
Rectangle Component::GetCasingB() const
{
    return m_casingRight;
}
Vector2 Component::GetPosition() const
{
    return { m_casingLeft.x, m_casingLeft.y + g_gridUnit };
}
void Component::SetPosition(Vector2 topLeft)
{
    constexpr float halfWidth = 2 * g_gridUnit;

    m_casingLeft.x = topLeft.x;
    m_casingLeft.y = topLeft.y - g_gridUnit;
    m_casingLeft.width = halfWidth;
    m_casingLeft.height = g_gridUnit * m_inputs + g_gridUnit;

    m_casingRight.x = topLeft.x + halfWidth;
    m_casingRight.y = topLeft.y - g_gridUnit;
    m_casingRight.width = halfWidth;
    m_casingRight.height = g_gridUnit * m_outputs + g_gridUnit;

    float y = topLeft.y;
    for (size_t i = 0; i < m_inputs; ++i)
    {
        m_nodes[i]->SetPosition({ topLeft.x, y });
        y += g_gridUnit;
    }
    float rightSide = topLeft.x + halfWidth * 2;
    y = topLeft.y;
    for (size_t i = 0; i < m_outputs; ++i)
    {
        m_nodes[i + m_inputs]->SetPosition({ rightSide, y });
        y += g_gridUnit;
    }
}

Node* Component::GetNode(size_t index) const
{
    return m_nodes[index];
}
const std::vector<Node*>& Component::GetNodes() const
{
    return m_nodes;
}
std::vector<Node*>& Component::GetNodesVector()
{
    return m_nodes;
}

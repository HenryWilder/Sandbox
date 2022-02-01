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
    size_t helperIndex = 0;
    NodeBP_Helper* helpers_block = new NodeBP_Helper[source.size()]; // Draw memory from this block to save on "new" calls

    std::vector<NodeBP_Helper*> helpers;
    helpers.reserve(source.size());

    // Write wire data as relative indices
    std::unordered_map<const Node*, size_t> nodeIndex;
    MapNodeRelativeIndices(source, nodeIndex);

    // Nodes
    inputs = 0;
    outputs = 0;
    for (Node* src : source)
    {
        helpers_block[helperIndex] = NodeBP_Helper{ src->GetGate(), src->GetPosition() };
        helpers.push_back(helpers_block + helperIndex);
        ++helperIndex;

        // No inputs on source means it is an input for the component
        bool isInput = src->HasNoInputs();
        for (Node* input : src->GetInputs())
        {
            if (nodeIndex.find(input) == nodeIndex.end()) // Inputs from external node
            {
                isInput = true;
                break;
            }
        }
        if (helpers.back()->noInputs = isInput)
            ++inputs;

        bool isOutput = src->HasNoOutputs();
        for (Node* output : src->GetOutputs())
        {
            if (nodeIndex.find(output) == nodeIndex.end()) // Outputs to external node
            {
                isOutput = true;
                break;
            }
        }
        if (isOutput)
            ++outputs;
    }

    // Wires
    for (Node* start : source)
    {
        size_t a = nodeIndex.find(start)->second;
        for (Node* end : start->GetOutputs())
        {
            auto it = nodeIndex.find(end);
            if (it != nodeIndex.end())
                helpers[a]->outputs.push_back(helpers[it->second]);
        }
    }

    // Sort
    auto inputsBegin    = helpers.begin();
    auto outputsBegin   = helpers.begin() + inputs;
    auto internalsBegin = helpers.begin() + (inputs + outputs);

    std::partition(inputsBegin,  helpers.end(), [](const NodeBP_Helper* node) { return node->noInputs;        });
    std::partition(outputsBegin, helpers.end(), [](const NodeBP_Helper* node) { return node->outputs.empty(); });

    std::sort(inputsBegin,  outputsBegin  );
    std::sort(outputsBegin, internalsBegin);
    // Internals don't need to be sorted

    // Write wire data as relative indices
    std::unordered_map<const NodeBP_Helper*, size_t> helperIndexMap;
    {
        helperIndexMap.reserve(helpers.size());
        for (size_t i = 0; i < helpers.size(); ++i)
        {
            helperIndexMap.insert({ helpers[i],i });
        }
    }

    // Convert helpers to node blueprints
    nodes.reserve(source.size());
    for (NodeBP_Helper* help : helpers)
    {
        nodes.push_back(NodeBlueprint{ help->gate });
        nodes.back().outputs.reserve(help->outputs.size());
        for (NodeBP_Helper* output : help->outputs)
        {
            nodes.back().outputs.push_back(helperIndexMap.find(output)->second);
        }
    }

    delete[] helpers_block;
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

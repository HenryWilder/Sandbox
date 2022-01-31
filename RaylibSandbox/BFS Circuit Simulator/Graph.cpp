#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <fstream>
#include <stack>
#include <algorithm>
#include <unordered_map>

#include "Node.h"
#include "Component.h"
#include "Graph.h"

bool Selectable::operator==(const Selectable& other) const
{
    if (other.type != type)
        return false;

    if (type == Type::COMP)
        return other.comp == comp;
    else
        return other.node == node;
}

bool Graph::IsNodeInGraph(const Node* node) const
{
    return IsNodeInVector(nodes, node);
}
bool Graph::NodeIndexInGraph(const Node* node, size_t* index) const
{
    return NodeIndexInVector(nodes, node, index);
}

void Graph::ResetVisited() const
{
    for (Node* node : nodes)
    {
        node->SetVisited(false);
    }
}
void Graph::ResetStates()
{
    for (Node* node : nodes)
    {
        node->SetState(node->GetGate() == Gate::NOR);
    }
}

// For components
void Graph::RegenerateSingleComponent(Component* component)
{
    std::vector<Node*> add;
    std::vector<Node*> remove;

    component->Regenerate(&add, &remove);

    for (Node* node : add)
    {
        AddNode(node);
    }

    for (Node* node : remove)
    {
        RemoveNode(node);
        delete node;
    }
}
void Graph::RegenerateComponents(std::vector<Component*>& components)
{
    std::vector<Node*> add;
    std::vector<Node*> remove;

    for (Component* comp : components)
    {
        comp->Regenerate(&add, &remove);
    }

    for (Node* node : add)
    {
        AddNode(node);
    }

    for (Node* node : remove)
    {
        RemoveNode(node);
        delete node;
    }
}
void Graph::RegenerateAllComponents()
{
    RegenerateComponents(components);
}

void Graph::CloneComponentAtPosition(ComponentBlueprint* blueprint, Vector2 position)
{
    Component* comp = new Component(blueprint);
    components.push_back(comp);
    RegenerateSingleComponent(comp);
    comp->SetPosition(position);
}

void Graph::MarkRouteDirty()
{
    routeDirty = true;
}

void Graph::MarkGateDirty()
{
    gateDirty = true;
}

void Graph::UnDirty()
{
    if (routeDirty)
        CalculateRoute();
    if (routeDirty || gateDirty)
        ResetStates();

    routeDirty = gateDirty = false;
}

// Uses modified BFS
void Graph::CalculateRoute()
{
    ResetVisited();

    size_t totalNodes = nodes.size();

    // Put the "seed" nodes at the start
    {
        std::vector<Node*> buff;
        buff.reserve(totalNodes);
        for (Node* node : nodes)
        {
            if (!node->GetVisited())
            {
                bool found = FindSeeds(&buff, node);
                if (!found)
                    buff.push_back(node);
            }
        }
        nodes.swap(buff);
    }
    seedCount = nodes.size(); // Only the seeds will be in at this time

    ResetVisited();

    size_t startIndex = 0;
    size_t endIndex;
    while (nodes.size() < totalNodes)
    {
        endIndex = nodes.size();
        for (size_t i = startIndex; i < endIndex; ++i)
        {
            for (Node* output : nodes[i]->GetOutputs())
            {
                if (!!output->GetVisited())
                {
                    nodes.push_back(output);
                    output->SetVisited(!true);
                }
            }
        }
        startIndex = endIndex;
    }
}

// Runs every tick
void Graph::Step()
{
    if (routeDirty)
        CalculateRoute();

    if (gateDirty)
        ResetStates();

    for (Node* node : nodes)
    {
        switch (node->GetGate())
        {
        case Gate::OR:
            node->SetState(false);
            for (Node* input : node->GetInputs())
            {
                if (input->GetState())
                {
                    node->SetState(true);
                    break;
                }
            }
            break;

        case Gate::NOR:
            node->SetState(true);
            for (Node* input : node->GetInputs())
            {
                if (input->GetState())
                {
                    node->SetState(false);
                    break;
                }
            }
            break;

        case Gate::AND:
            node->SetState(!node->HasNoInputs());
            for (Node* input : node->GetInputs())
            {
                if (!input->GetState())
                {
                    node->SetState(false);
                    break;
                }
            }
            break;

        case Gate::XOR:
            node->SetState(false);
            for (Node* input : node->GetInputs())
            {
                if (input->GetState())
                {
                    if (node->GetState())
                    {
                        node->SetState(false);
                        break;
                    }
                    else
                        node->SetState(true);
                }
            }
            break;
        }
    }
}

// Runs every frame
void Graph::DrawComponents() const
{
    for (Component* comp : components)
    {
        DrawRectangleRec(comp->GetCasingA(), DARKGRAY);
        DrawRectangleRec(comp->GetCasingB(), DARKGRAY);
    }
}
void Graph::DrawWires() const
{
    // Draw the wires first so the nodes can be drawn on top
    for (Node* node : nodes)
    {
        if (node->GetHidden())
            continue;

        for (Node* next : node->GetOutputs())
        {
            if (next->GetHidden())
                continue;

            Color color = node->GetState() ? DARKBLUE : DARKGRAY;
            DrawLineV(node->GetPosition(), next->GetPosition(), color);

            Vector2 midpoint = (node->GetPosition() + next->GetPosition()) * 0.5f;
            Vector2 direction = Vector2Normalize(next->GetPosition() - node->GetPosition()) * g_gridUnit * 0.5f;
            Vector2 angle = Vector2Rotate(direction, -135.0f);
            DrawLineV(midpoint + direction, midpoint + direction + angle, color);
        }
    }
}
void Graph::DrawNodes() const
{
    // Node drawing is separate so that cyclic graphs can still draw nodes on top
    for (Node* node : nodes)
    {
        if (node->GetHidden())
            continue;

        DrawGateIcon(node->GetGate(), node->GetPosition(), g_nodeRadius, (node->GetState() ? BLUE : GRAY));

        if (node->HasNoOutputs())
            DrawCircleV(node->GetPosition(), g_nodeRadius * 0.35f, ORANGE);

        if (node->HasNoInputs())
            DrawCircleV(node->GetPosition(), g_nodeRadius * 0.35f, GREEN);
    }
}

bool Graph::FindSeeds(std::vector<Node*>* buff, Node* node)
{
    bool foundSeed = node->HasNoInputs();
    if (foundSeed)
        buff->push_back(node);

    node->SetVisited(true);

    for (Node* output : node->GetOutputs())
    {
        if (!output->GetVisited())
            foundSeed = FindSeeds(buff, output) || foundSeed;
    }
    for (Node* input : node->GetInputs())
    {
        if (!input->GetVisited())
            foundSeed = FindSeeds(buff, input) || foundSeed;
    }
    return foundSeed;
}

// Create a node without connections
void Graph::AddNode(Node* newNode)
{
    if (!newNode)
        return;

    if (std::find(nodes.begin(), nodes.end(), newNode) == nodes.end())
        nodes.push_back(newNode);
}

void Graph::ConnectNodes(Node* start, Node* end)
{
    auto endIt = std::find(start->GetInputs().begin(), start->GetInputs().end(), end);
    auto startIt = std::find(end->GetOutputs().begin(), end->GetOutputs().end(), start);

    if (!(endIt == start->GetInputs().end() &&
        startIt == end->GetOutputs().end()))
        return;

    start->AddOutput(end);
    end->AddInput(start);
}
void Graph::DisconnectNodes(Node* start, Node* end)
{
    start->RemoveOutput(end);
    end->RemoveInput(start);
}

// Erases a node from existence
void Graph::RemoveNode(Node* node)
{
    if (!node)
        return;

    auto it = std::find(nodes.begin(), nodes.end(), node);
    if (it != nodes.end())
    {
        node->ClearReferencesToSelf();
        nodes.erase(it);
    }
}
void Graph::RemoveComponent(Component* comp)
{
    if (!comp)
        return;

    auto it = std::find(components.begin(), components.end(), comp);
    if (it != components.end())
    {
        for (Node* node : comp->GetNodes())
        {
            RemoveNode(node);
            delete node;
        }
        components.erase(it);
    }
}

Node* Graph::FindNodeAtPosition(Vector2 position, float radius) const
{
    float shortestDistance = radius;
    Node* closest = nullptr;
    for (Node* node : nodes)
    {
        if (node->GetHidden())
            continue;

        float distance = Vector2Distance(position, node->GetPosition());
        if (distance < shortestDistance)
        {
            shortestDistance = distance;
            closest = node;
        }
    }
    return closest;
}
Wire Graph::FindWireIntersectingPosition(Vector2 position, float radius) const
{
    float shortestDistance = radius;
    Wire closest = { nullptr, nullptr };
    ResetVisited();
    for (Node* start : nodes)
    {
        if (start->GetVisited())
            continue;

        if (start->GetHidden())
            continue;

        for (Node* end : start->GetOutputs())
        {
            if (end->GetVisited())
                break;

            if (end->GetHidden())
                continue;

            if (start->GetInComponent() && end->GetInComponent())
                continue;

            float distance = Vector2DistanceToLine(start->GetPosition(), end->GetPosition(), position);
            if (distance < shortestDistance)
            {
                shortestDistance = distance;
                closest = { start, end };
            }
        }
    }
    return closest;
}
void Graph::FindSelectablesInRectangle(std::vector<Selectable>* output, Rectangle search) const
{
    output->clear();
    for (Node* node : nodes)
    {
        if (!node->GetInComponent() && CheckCollisionPointRec(node->GetPosition(), search))
            output->push_back(Selectable(node));
    }
    for (Component* comp : components)
    {
        if (CheckCollisionRecs(search, comp->GetCasingA()) || CheckCollisionRecs(search, comp->GetCasingB()))
            output->push_back(Selectable(comp));
    }
}
Component* Graph::FindComponentAtPosition(Vector2 position) const
{
    for (Component* comp : components)
    {
        if (CheckCollisionPointRec(position, comp->GetCasingA()) || CheckCollisionPointRec(position, comp->GetCasingB()))
            return comp;
    }
}

void Graph::Save(const char* filename)
{
    // Open the file for writing
    std::ofstream file(filename);

    file << "version: 3.0.1\n";

    std::vector<ComponentBlueprint*> blueprints;

    // Blueprints
    {
        for (Component* comp : components)
        {
            if (std::find(blueprints.begin(), blueprints.end(), comp->GetBlueprint()) == blueprints.end())
                blueprints.push_back(comp->GetBlueprint());
        }
        file << "\nblueprints: " << blueprints.size() << '\n';
        for (ComponentBlueprint* bp : blueprints)
        {
            file << bp->inputs << ' ' << bp->outputs << ' ' << bp->nodes.size() << ' ';
            for (const NodeBlueprint& node : bp->nodes)
            {
                file << (char)node.gate << ' ' << node.outputs.size();
                for (size_t index : node.outputs)
                {
                    file << ' ' << index;
                }
                file << ' ';
            }
            file << '\n';
        }
    }

    // Nodes
    {
        // Write the number of nodes
        file << "\nnodes: " << nodes.size() << '\n';

        // Write node data
        for (Node* node : nodes)
        {
            file << node->GetPosition().x << ' ' << node->GetPosition().y << ' ' << (char)node->GetGate() << '\n';
        }
    }

    // Wires
    {
        // Count wires
        size_t connections = 0;
        ResetVisited();
        for (Node* start : nodes)
        {
            if (start->GetVisited())
                continue;

            for (Node* end : start->GetOutputs())
            {
                if (end->GetVisited())
                    break;

                ++connections;
            }
        }

        // Write number of wires
        file << "\nwires: " << connections << '\n';

        // Write wire data as relative indices
        ResetVisited();
        for (Node* start : nodes)
        {
            if (start->GetVisited())
                continue;

            // Find the index of the start node
            size_t a = std::find(nodes.begin(), nodes.end(), start) - nodes.begin();

            // We don't need to look at the inputs because that information is implied by the outputs
            for (Node* end : start->GetOutputs())
            {
                if (end->GetVisited())
                    break;

                // Find the index of the end node
                size_t b = std::find(nodes.begin(), nodes.end(), end) - nodes.begin();

                // Write the wire
                file << a << ' ' << b << '\n';
            }
        }
    }

    // Components
    {
        file << "\ncomponents: " << components.size() << '\n';
        for (Component* comp : components)
        {
            file << (std::find(blueprints.begin(), blueprints.end(), comp->GetBlueprint()) - blueprints.begin()) << ' '
                << comp->GetPosition().x << ' ' << comp->GetPosition().y << ' ' << comp->GetNodeCount();
            for (Node* node : comp->GetNodes())
            {
                file << ' ' << std::find(nodes.begin(), nodes.end(), node) - nodes.begin();
            }
            file << '\n';
        }
    }

    // Close the file
    file.close();
}

void Graph::Load(const char* filename)
{
    std::ifstream file(filename);
    file.ignore(256, ':');
    int api, version, patch;
    file >> api;
    file.ignore(1);
    file >> version;
    file.ignore(1);
    file >> patch;

    if (api != 3)
        return;
    if (version > 0)
        return;
    if (patch > 1)
        return;

    nodes.clear();
    components.clear();

    file.ignore(256, ':');

    std::vector<ComponentBlueprint*> blueprints;

    // Blueprints
    {
        // Read number of blueprints
        size_t count;
        file >> count;

        blueprints.reserve(count);

        for (size_t i = 0; i < count; ++i)
        {
            size_t in, out;
            file >> in >> out;
            blueprints.push_back(new ComponentBlueprint(in, out));
            size_t nodes;
            file >> nodes;
            blueprints.back()->nodes.reserve(nodes);
            for (size_t j = 0; j < nodes; ++j)
            {
                char gate;
                file >> gate;
                blueprints.back()->nodes.push_back(NodeBlueprint{ (Gate)gate });
                size_t outputCount;
                file >> outputCount;
                blueprints.back()->nodes.back().outputs.reserve(outputCount);
                for (size_t k = 0; k < outputCount; ++k)
                {
                    size_t index;
                    file >> index;
                    blueprints.back()->nodes.back().outputs.push_back(index);
                }
            }
        }
    }

    file.ignore(256, ':');

    // Nodes
    {
        // Read number of nodes
        size_t count;
        file >> count;

        // Prep storage for the incoming number of nodes
        graph->nodes.reserve(count);

        // Read node data
        for (size_t i = 0; i < count; ++i)
        {
            Vector2 pos;
            char gate;
            file >> pos.x >> pos.y >> gate;
            graph->AddNode(new Node(pos, (Gate)gate));
        }
    }

    file.ignore(256, ':');

    // Wires
    {
        // Read number of wires
        size_t count;
        file >> count;

        // Read wire data
        for (size_t i = 0; i < count; ++i)
        {
            size_t a, b;
            file >> a >> b;
            graph->ConnectNodes(graph->nodes[a], graph->nodes[b]);
        }
    }

    file.ignore(256, ':');

    // Components
    {
        size_t count;
        file >> count;
        graph->components.reserve(count);

        for (size_t i = 0; i < count; ++i)
        {
            size_t bpIndex;
            Vector2 position;
            size_t nodeCount;
            file >> bpIndex >> position.x >> position.y >> nodeCount;

            Component* comp = new Component(blueprints[bpIndex]);
            graph->components.push_back(comp);
            comp->SetPosition(position);

            // Save space for the nodes -- some can be reused, while the excess get dropped off.
            // Do not worry about leaking the excess, as their pointers have been saved above for external removal.
            comp->GetNodesVector().reserve(nodeCount);

            for (size_t i = 0; i < blueprints[bpIndex]->nodes.size(); ++i)
            {
                size_t nodeIndex;
                file >> nodeIndex;
                comp->GetNodesVector().push_back(graph->nodes[nodeIndex]);
                graph->nodes[nodeIndex]->SetInComponent(true);
            }

            // Hide internals
            for (size_t i = blueprints[bpIndex]->inputs + blueprints[bpIndex]->outputs; i < comp->GetNodeCount(); ++i)
            {
                comp->GetNode(i)->SetHidden(true);
            }
        }
    }

    file.close();
}

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
#include <unordered_set>

#include "Node.h"
#include "Component.h"
#include "Graph.h"

bool Vector2Equals(Vector2 a, Vector2 b)
{
    return
        a.x == b.x &&
        a.y == b.y;
}

float Vector2DistanceToLine(Vector2 startPos, Vector2 endPos, Vector2 point)
{
    Vector2 direction = Vector2Normalize(Vector2Subtract(endPos, startPos));
    Vector2 nearestPoint = Vector2Add(startPos, Vector2Scale(direction, Clamp(Vector2DotProduct(direction, Vector2Subtract(point, startPos)), 0, Vector2Distance(startPos, endPos))));
    return Vector2Distance(point, nearestPoint);
}

bool CheckCollisionLineCircle(Vector2 startPos, Vector2 endPos, Vector2 center, float radius)
{
    return Vector2DistanceToLine(startPos, endPos, center) <= radius;
}


bool Graph::IsNodeInGraph(const Node* node) const
{
    return IsNodeInVector(nodes, node);
}
bool Graph::FindNodeIndexInGraph(const Node* node, size_t* index) const
{
    return FindNodeIndexInVector(nodes, node, index);
}
size_t Graph::NodeIndexInGraph(const Node* node) const
{
    return NodeIndexInVector(nodes, node);
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

const std::vector<Component*>& Graph::GetComponents() const
{
    return components;
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

Component* Graph::CloneComponentAtPosition(ComponentBlueprint* blueprint, Vector2 position)
{
    Component* comp = new Component(blueprint);
    components.push_back(comp);
    RegenerateSingleComponent(comp);
    comp->SetPosition(position);
    return comp;
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

            Vector2 midpoint = Vector2Scale(Vector2Add(node->GetPosition(), next->GetPosition()), 0.5f);
            Vector2 direction = Vector2Scale(Vector2Normalize(Vector2Subtract(next->GetPosition(), node->GetPosition())), g_gridUnit * 0.5f);
            Vector2 angle = Vector2Rotate(direction, -135.0f);
            Vector2 offset = Vector2Add(midpoint, direction);
            DrawLineV(offset, Vector2Add(offset, angle), color);
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
void Graph::AddNodes(const std::vector<Node*>& source)
{
    nodes.reserve(source.size());
    for (Node* node : source)
    {
        AddNode(node);
    }
}
Node* Graph::AddNewNode(Vector2 position)
{
    Node* node = new Node(position, Gate::OR);
    AddNode(node);
    return node;
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
void Graph::RemoveMultipleNodes(const std::vector<Node*>& remove)
{
    std::unordered_set<Node*> removeSet(remove.begin(), remove.end());
    size_t newSize = nodes.size();
    std::stable_partition(nodes.begin(), nodes.end(), [&removeSet, &newSize](Node* node)
        {
            if (removeSet.find(node) == removeSet.end())
                return true;
            newSize--;
            return false;
        });
    nodes.resize(newSize);
}
bool Graph::RemoveNodeAndTransferConnections(Node* node)
{
    bool safe =
        node->InputCount () == 1 &&
        node->OutputCount() == 1;

    if (safe)
    {
        ConnectNodes(node->GetInput(0), node->GetOutput(0));
        RemoveNode(node);
    }
    else
        node->SetGate(Gate::OR);

    return safe;
}
void Graph::RemoveNodeAndDelete(Node* node)
{
    RemoveNode(node);
    delete node;
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
void Graph::RemoveMultipleComponents(const std::vector<Component*>& remove)
{
    std::unordered_set<Component*> removeSet(remove.begin(), remove.end());
    size_t newSize = components.size();
    std::stable_partition(components.begin(), components.end(), [&removeSet, &newSize](Component* comp)
        {
            if (removeSet.find(comp) == removeSet.end())
                return true;
            newSize--;
            return false;
        });
    components.resize(newSize);
}
void Graph::RemoveComponentEntirely(Component* comp)
{
    RemoveComponent(comp);
    delete comp;
}

Node* Graph::FindNodeAtGridPoint(Vector2 point) const
{
    for (Node* node : nodes)
    {
        if (node->GetHidden())
            continue;

        if (Vector2Equals(node->GetPosition(), point))
            return node;
    }
    return nullptr;
}
Wire Graph::FindWireIntersectingGridPoint(Vector2 point) const
{
    ResetVisited();
    for (Node* start : nodes)
    {
        if (start->GetVisited() || start->GetHidden())
            continue;

        Vector2 a = start->GetPosition();

        for (Node* end : start->GetOutputs())
        {
            if (end->GetVisited() || end->GetHidden() || (start->IsInComponent() && end->IsInComponent()))
                continue;

            Vector2 b = end->GetPosition();

            // Vertical
            if (a.x == b.x)
            {
                if (point.x != a.x)
                    continue;

                auto [min, max] = std::minmax(a.y, b.y);
                if (min <= point.y && point.y <= max)
                    return Wire{ start, end };
            }
            // Horizontal
            else if (a.y == b.y)
            {
                if (point.y != a.y)
                    continue;

                auto [min, max] = std::minmax(a.x, b.x);
                if (min <= point.x && point.x <= max)
                    return Wire{ start, end };
            }
            // Diagonal
            else
            {
                auto [minx, maxx] = std::minmax(a.x, b.x);
                auto [miny, maxy] = std::minmax(a.y, b.y);

                // Bounding box
                if (minx < point.x && point.x < maxx &&
                    miny < point.y && point.y < maxy &&
                    abs(point.x - a.x) == abs(point.y - a.y))
                {
                    return Wire{ start, end };
                }
            }
        }
    }
    return Wire{ nullptr, nullptr };
}
void Graph::FindObjectsInRectangle(std::vector<Node*>& nodeOutput, std::vector<Component*>& compOutput, Rectangle search) const
{
    nodeOutput.clear();
    for (Node* node : nodes)
    {
        if (CheckCollisionPointRec(node->GetPosition(), search))
            nodeOutput.push_back(node);
    }
    compOutput.clear();
    for (Component* comp : components)
    {
        if (CheckCollisionRecs(search, comp->GetCasingA()) ||
            CheckCollisionRecs(search, comp->GetCasingB()))
            compOutput.push_back(comp);
    }
    // todo: Extract nodes from components?
}
void Graph::RemoveObjects(const Selection& remove)
{
    // Nodes
    {
        std::unordered_set<Node*> nodesToRemove(remove.nodes.begin(), remove.nodes.end());

        // Reduce redundant
        size_t count = remove.nodes.size();
        for (Component* comp : remove.comps)
        {
            count += comp->GetNodeCount();
        }
        nodesToRemove.reserve(count);

        // Include component nodes
        for (Component* comp : remove.comps)
        {
            nodesToRemove.insert(comp->GetNodes().begin(), comp->GetNodes().end());
        }

        // Remove
        {
            size_t popCount = 0;

            auto pred = [&nodesToRemove, &popCount](Node* node)
            {
                if (nodesToRemove.find(node) == nodesToRemove.end())
                    return true;
                popCount++;
                return false;
            };
            std::stable_partition(nodes.begin(), nodes.end(), pred);

            for (size_t i = 0; i < popCount; ++i)
            {
                for (Node* input : nodes.back()->GetInputs())
                {
                    if (nodesToRemove.find(input) == nodesToRemove.end())
                        input->RemoveOutput(nodes.back());
                }
                for (Node* output : nodes.back()->GetOutputs())
                {
                    if (nodesToRemove.find(output) == nodesToRemove.end())
                        output->RemoveOutput(nodes.back());
                }
                nodes.pop_back();
            }
        }
    }

    // Components
    {
        std::unordered_set<Component*> componentsToRemove(remove.comps.begin(), remove.comps.end());

        size_t popCount = 0;
        std::stable_partition(components.begin(), components.end(), [&componentsToRemove, &popCount](Component* comp)
            {
                if (componentsToRemove.find(comp) == componentsToRemove.end())
                    return true;
                popCount++;
                return false;
            });
        for (size_t i = 0; i < popCount; ++i)
        {
            components.pop_back();
        }
    }
}
Component* Graph::FindComponentAtPosition(Vector2 position) const
{
    for (Component* comp : components)
    {
        if (CheckCollisionPointRec(position, comp->GetCasingA()) ||
            CheckCollisionPointRec(position, comp->GetCasingB()))
            return comp;
    }
    return nullptr;
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
            file << bp->inputs << ' '
                 << bp->outputs << ' '
                 << bp->nodes.size() << ' ';
            for (const NodeBlueprint& node : bp->nodes)
            {
                file << (char)node.gate << ' '
                     << node.outputs.size();
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
            file << node->GetPosition().x << ' '
                 << node->GetPosition().y << ' '
                 << (char)node->GetGate() << '\n';
        }
    }

    // Wires
    {
        std::vector<WireRelative> wires;
        ListWireRelativeIndices(nodes, wires);

        // Write number of wires
        file << "\nwires: " << wires.size() << '\n';

        // Write wire data as relative indices
        for (WireRelative wire : wires)
        {
            file << wire.a << ' '
                 << wire.b << '\n';
        }
    }

    // Components
    {
        file << "\ncomponents: " << components.size() << '\n';
        for (Component* comp : components)
        {
            size_t blueprintIndex = std::find(blueprints.begin(), blueprints.end(), comp->GetBlueprint()) - blueprints.begin();
            file << blueprintIndex << ' '
                 << comp->GetPosition().x << ' '
                 << comp->GetPosition().y;

            for (Node* node : comp->GetNodes())
            {
                file << ' ' << NodeIndexInGraph(node);
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
        nodes.reserve(count);

        // Read node data
        for (size_t i = 0; i < count; ++i)
        {
            Vector2 pos;
            char gate;
            file >> pos.x >> pos.y >> gate;
            AddNode(new Node(pos, (Gate)gate));
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
            ConnectNodes(nodes[a], nodes[b]);
        }
    }

    file.ignore(256, ':');

    // Components
    {
        size_t count;
        file >> count;
        components.reserve(count);

        for (size_t i = 0; i < count; ++i)
        {
            size_t bpIndex;
            Vector2 position;
            size_t nodeCount;
            file >> bpIndex >> position.x >> position.y >> nodeCount;

            Component* comp = new Component(blueprints[bpIndex]);
            components.push_back(comp);
            comp->SetPosition(position);

            // Save space for the nodes -- some can be reused, while the excess get dropped off.
            // Do not worry about leaking the excess, as their pointers have been saved above for external removal.
            comp->GetNodesVector().reserve(nodeCount);

            for (size_t i = 0; i < blueprints[bpIndex]->nodes.size(); ++i)
            {
                size_t nodeIndex;
                file >> nodeIndex;
                comp->GetNodesVector().push_back(nodes[nodeIndex]);
                nodes[nodeIndex]->SetComponentInternal();
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

const std::vector<Node*>& Graph::GetNodes() const
{
    return nodes;
}

void Graph::OffsetNodes(const Selection& source, Vector2 offset)
{
    for (Component* comp : source.comps)
    {
        comp->SetPosition(Vector2Add(comp->GetPosition(), offset));
    }
    for (Node* node : source.nodes)
    {
        if (!node->IsInComponent())
            node->SetPosition(Vector2Add(node->GetPosition(), offset));
    }
}

void Graph::DivideWire(Wire wire, Node* interloper)
{
    DisconnectNodes(wire.a, wire.b);
    ConnectNodes(wire.a, interloper);
    ConnectNodes(interloper, wire.b);
}

Wire Graph::ReverseWire(Wire wire)
{
    DisconnectNodes(wire.a, wire.b);
    ConnectNodes(wire.b, wire.a);
    return { wire.b, wire.a };
}

void Selection::AccountForComponentNodes()
{
    std::unordered_set<Node*> existing(nodes.begin(), nodes.end());
    size_t size = existing.size();
    for (Component* comp : comps)
    {
        size += comp->GetNodeCount();
    }
    existing.reserve(size);
    for (Component* comp : comps)
    {
        existing.insert(comp->GetNodes().begin(), comp->GetNodes().end());
    }
    nodes = std::vector<Node*>(existing.begin(), existing.end());
}

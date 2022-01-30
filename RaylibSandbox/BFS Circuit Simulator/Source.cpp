#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <fstream>
#include <stack>
#include <algorithm>

#define sign(x) (((x) > (decltype(x))(0)) - ((x) < (decltype(x))(0)))

using Vector1 = float;

#pragma region Vector math

inline Vector2  operator- (Vector2  vec)          { return      Vector2Negate(vec);             }
inline Vector2  operator+ (Vector2  a, Vector2 b) { return      Vector2Add(a, b);               }
inline Vector2& operator+=(Vector2& a, Vector2 b) { return (a = Vector2Add(a, b));              }
inline Vector2  operator+ (Vector2  a, float val) { return      Vector2AddValue(a, val);        }
inline Vector2& operator+=(Vector2& a, float val) { return (a = Vector2AddValue(a, val));       }
inline Vector2  operator- (Vector2  a, Vector2 b) { return      Vector2Subtract(a, b);          }
inline Vector2& operator-=(Vector2& a, Vector2 b) { return (a = Vector2Subtract(a, b));         }
inline Vector2  operator- (Vector2  a, float val) { return      Vector2SubtractValue(a, val);   }
inline Vector2& operator-=(Vector2& a, float val) { return (a = Vector2SubtractValue(a, val));  }
inline Vector2  operator* (Vector2  a, Vector2 b) { return      Vector2Multiply(a, b);          }
inline Vector2& operator*=(Vector2& a, Vector2 b) { return (a = Vector2Multiply(a, b));         }
inline Vector2  operator* (Vector2  a, float scl) { return      Vector2Scale(a, scl);           }
inline Vector2& operator*=(Vector2& a, float scl) { return (a = Vector2Scale(a, scl));          }
inline Vector2  operator/ (Vector2  a, Vector2 b) { return      Vector2Divide(a, b);            }
inline Vector2& operator/=(Vector2& a, Vector2 b) { return (a = Vector2Divide(a, b));           }
inline Vector2  operator/ (Vector2  a, float div) { return      Vector2Scale(a, 1.0f / div);    }
inline Vector2& operator/=(Vector2& a, float div) { return (a = Vector2Scale(a, 1.0f / div));   }

inline Vector3  operator- (Vector3  vec)          { return      Vector3Negate(vec);             }
inline Vector3  operator+ (Vector3  a, Vector3 b) { return      Vector3Add(a, b);               }
inline Vector3& operator+=(Vector3& a, Vector3 b) { return (a = Vector3Add(a, b));              }
inline Vector3  operator+ (Vector3  a, float val) { return      Vector3AddValue(a, val);        }
inline Vector3& operator+=(Vector3& a, float val) { return (a = Vector3AddValue(a, val));       }
inline Vector3  operator- (Vector3  a, Vector3 b) { return      Vector3Subtract(a, b);          }
inline Vector3& operator-=(Vector3& a, Vector3 b) { return (a = Vector3Subtract(a, b));         }
inline Vector3  operator- (Vector3  a, float val) { return      Vector3SubtractValue(a, val);   }
inline Vector3& operator-=(Vector3& a, float val) { return (a = Vector3SubtractValue(a, val));  }
inline Vector3  operator* (Vector3  a, Vector3 b) { return      Vector3Multiply(a, b);          }
inline Vector3& operator*=(Vector3& a, Vector3 b) { return (a = Vector3Multiply(a, b));         }
inline Vector3  operator* (Vector3  a, float scl) { return      Vector3Scale(a, scl);           }
inline Vector3& operator*=(Vector3& a, float scl) { return (a = Vector3Scale(a, scl));          }
inline Vector3  operator/ (Vector3  a, Vector3 b) { return      Vector3Divide(a, b);            }
inline Vector3& operator/=(Vector3& a, Vector3 b) { return (a = Vector3Divide(a, b));           }
inline Vector3  operator/ (Vector3  a, float div) { return      Vector3Scale(a, 1.0f / div);    }
inline Vector3& operator/=(Vector3& a, float div) { return (a = Vector3Scale(a, 1.0f / div));   }

#pragma endregion

constexpr float g_nodeRadius = 6.0f;
constexpr float g_gridUnit = g_nodeRadius * 2.0f;

float Vector2DistanceToLine(Vector2 startPos, Vector2 endPos, Vector2 point)
{
    Vector2 direction = Vector2Normalize(endPos - startPos);
    Vector2 nearestPoint = startPos + direction * Clamp(Vector2DotProduct(direction, point - startPos), 0, Vector2Distance(startPos, endPos));
    return Vector2Distance(point, nearestPoint);
}
bool CheckCollisionLineCircle(Vector2 startPos, Vector2 endPos, Vector2 center, float radius)
{
    return Vector2DistanceToLine(startPos, endPos, center) <= radius;
}

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

enum class Gate : char
{
    OR  = '|',
    NOR = '!',
    AND = '&',
    XOR = '^',
};

class Node
{
private:
    Vector2 m_position;
    Gate m_type;
    bool b_state;
    bool b_visited;
    bool b_hidden;

    std::vector<Node*> m_inputs;
    std::vector<Node*> m_outputs;

public:
    Node(Vector2 position, Gate type) : m_position(position), m_type(type), b_state(false), b_visited(false), b_hidden(false), m_inputs{}, m_outputs{} {}

    Vector2 GetPosition() const
    {
        return m_position;
    }
    void SetPosition(Vector2 position)
    {
        m_position = position;
    }

    Gate GetGate() const
    {
        return m_type;
    }
    void SetGate(Gate type)
    {
        m_type = type;
    }

    bool GetState() const
    {
        return b_state;
    }
    void SetState(bool state)
    {
        b_state = state;
    }

    bool GetVisited() const
    {
        return b_visited;
    }
    void SetVisited(bool visited)
    {
        b_visited = visited;
    }

    bool GetHidden() const
    {
        return b_hidden;
    }
    void SetHidden(bool hidden)
    {
        b_hidden = hidden;
    }

    bool HasNoInputs() const
    {
        return m_inputs.empty();
    }
    bool InputCount() const
    {
        return m_inputs.size();
    }
    const std::vector<Node*>& GetInputs() const
    {
        return m_inputs;
    }
    void AddInput(Node* input)
    {
        if (!input)
            return;

        if (std::find(m_inputs.begin(), m_inputs.end(), input) == m_inputs.end()) // Prevent duplicates
            m_inputs.push_back(input);
    }
    void RemoveInput(const Node* input)
    {
        if (!input)
            return;

        auto it = std::find(m_inputs.begin(), m_inputs.end(), input);
        if (it != m_inputs.end())
            m_inputs.erase(it);
    }

    bool HasNoOutputs() const
    {
        return m_outputs.empty();
    }
    bool OutputCount() const
    {
        return m_outputs.size();
    }
    const std::vector<Node*>& GetOutputs() const
    {
        return m_outputs;
    }
    void AddOutput(Node* output)
    {
        if (!output)
            return;

        if (std::find(m_outputs.begin(), m_outputs.end(), output) == m_outputs.end()) // Prevent duplicates
            m_outputs.push_back(output);
    }
    void RemoveOutput(const Node* output)
    {
        if (!output)
            return;

        auto it = std::find(m_outputs.begin(), m_outputs.end(), output);
        if (it != m_outputs.end())
            m_outputs.erase(it);
    }

    // Warning: Affects input/output nodes
    // Always use this before deleting the node
    // It *can* be skipped when it is certain no references to this node can possibly exist
    // Or if it is certain all inputs and outputs are also being deleted
    // Forgetting to call this function when removing a node with references to it can lead to a crash!
    void ClearReferencesToSelf()
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
};

void DrawGateIcon(Gate type, Vector2 center, float radius, Color color, bool drawXORline = true)
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
            points[i] = center + (offsets[i] * radius);
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
        DrawRectangleV(center - Vector2{ radius, radius }, { diameter, diameter }, color);
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

struct Wire
{
    Node* a;
    Node* b;
};

float Signf(float value)
{
    return (value > 0.01f ? 1.0f : (value < -0.01f ? -1.0f : 0.0f));
}
void CompressWire(Node* start, Node* end, float length)
{
    Vector2 pos = end->GetPosition();
    pos.x = start->GetPosition().x + length * Signf(pos.x - start->GetPosition().x);
    pos.y = start->GetPosition().y + length * Signf(pos.y - start->GetPosition().y);
    end->SetPosition(pos);
}
void CompressNetwork(Node* start, float length)
{
    std::vector<Node*> wave;
    start->SetVisited(true);


    for (Node* output : start->GetOutputs())
    {
        if (!output->GetVisited())
            wave.push_back(output);
    }
    for (Node* input : start->GetInputs())
    {
        if (!input->GetVisited())
            wave.push_back(input);
    }

    for (Node* node : wave)
    {
        node->SetVisited(true);
        CompressWire(start, node, length);
    }
    for (Node* node : wave)
    {
        CompressNetwork(node, length);
    }
    wave.clear();
}

struct NodeBlueprint
{
    Gate gate;
    std::vector<size_t> inputs;
    std::vector<size_t> outputs;
};

struct ComponentBlueprint
{
    ComponentBlueprint(std::vector<Node*>& source)
    {
        // Write the number of nodes
        nodes.reserve(source.size());

        // Write node data
        for (Node* src : source)
        {
            src->SetVisited(false);
            nodes.push_back(NodeBlueprint{ src->GetGate() });
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
                    break;

                // Find the index of the end node
                auto it = std::find(source.begin(), source.end(), end);

                // Write the wire
                if (it != source.end())
                {
                    size_t b = it - source.begin();
                    nodes[a].outputs.push_back(b);
                    nodes[b].inputs.push_back(a);
                }
            }
        }

        //
        // Sort
        //


        std::vector<NodeBlueprint> buff;
        buff.reserve(nodes.size());
        std::vector<bool> visited(nodes.size(), false);

        struct Vert
        {
            NodeBlueprint bp;
            float y;

            bool operator<(const Vert& other) const
            {
                return y < other.y;
            }
            bool operator>(const Vert& other) const
            {
                return y > other.y;
            }
        };

        // Inputs
        {
            std::vector<Vert> temp;
            for (size_t i = 0; i < nodes.size(); ++i)
            {
                // No inputs
                if (nodes[i].inputs.empty())
                {
                    temp.push_back(Vert{ NodeBlueprint{ nodes[i] }, source[i]->GetPosition().y });
                    visited[i] = true;
                    ++inputs;
                }
            }
            std::sort(temp.begin(), temp.end());
        }

        // Outputs
        for (size_t i = 0; i < nodes.size(); ++i)
        {
            if (visited[i])
                continue;

            // No outputs
            if (nodes[i].outputs.empty())
            {
                buff.push_back(nodes[i]);
                visited[i] = true;
                ++outputs;
            }
        }

        // Remaining
        for (size_t i = 0; i < nodes.size(); ++i)
        {
            if (visited[i])
                continue;

            buff.push_back(nodes[i]);
        }

        nodes.swap(buff);
    }

    size_t inputs;
    size_t outputs;
    std::vector<NodeBlueprint> nodes; // Sort: [inputs][outputs][internals]
};

class Component
{
private:
    ComponentBlueprint* m_blueprint;
    size_t inputs;
    size_t outputs;
    std::vector<Node*> m_nodes; // Sorted by blueprint

public:
    Component(ComponentBlueprint* blueprint) : m_blueprint(blueprint), inputs(blueprint->inputs), outputs(blueprint->outputs), m_nodes{} {}

    ComponentBlueprint* const GetBlueprint() const
    {
        return m_blueprint;
    }
    void SetBlueprint(ComponentBlueprint* blueprint)
    {
        m_blueprint = blueprint;
    }
    // Parameters are vectors of the nodes that need to be added/removed
    void Regenerate(std::vector<Node*>* add, std::vector<Node*>* remove)
    {
        inputs = m_blueprint->inputs;
        outputs = m_blueprint->outputs;

        int difference = m_blueprint->nodes.size() - m_nodes.size();

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
            for (size_t i = m_nodes.size() - 1; i < m_blueprint->nodes.size(); ++i)
            {
                add->push_back(m_nodes[i]);
            }
        }

        // Add connections
        // Adding/removing nodes requires permission from the Graph structure, but wires only require the nodes.
        for (size_t i = 0; i < m_nodes.size(); ++i)
        {
            for (size_t input : m_blueprint->nodes[i].inputs)
            {
                m_nodes[i]->AddInput(m_nodes[input]);
            }
            for (size_t output : m_blueprint->nodes[i].outputs)
            {
                m_nodes[i]->AddOutput(m_nodes[output]);
            }
        }
    }

    size_t GetInputCount() const
    {
        return inputs;
    }
    size_t GetOutputCount() const
    {
        return outputs;
    }
    size_t GetInternalCount() const
    {
        return m_nodes.size() - (inputs + outputs);
    }
};

struct Graph
{
    size_t seedCount = 0;
    bool routeDirty = false;
    bool gateDirty = false;
    // Sort using CalculateRoute() every time a node is added or removed
    std::vector<Node*> nodes;
    std::vector<ComponentBlueprint*> blueprints;
    std::vector<Component*> components;

    void ResetVisited()
    {
        for (Node* node : nodes)
        {
            node->SetVisited(false);
        }
    }
    void ResetStates()
    {
        for (Node* node : nodes)
        {
            node->SetState(node->GetGate() == Gate::NOR);
        }
    }

    // For components
    void RegenerateSingleComponent(Component* component)
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
    void RegenerateComponents(std::vector<Component*>& components)
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
    void RegenerateAllComponents()
    {
        RegenerateComponents(components);
    }

    // Uses modified BFS
    void CalculateRoute()
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
    void Step()
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
    void DrawComponents()
    {
        // Todo
        for (Component* comp : components)
        {
            // DrawRectangleRec(comp);
        }
    }
    void DrawWires()
    {
        // Draw the wires first so the nodes can be drawn on top
        for (Node* node : nodes)
        {
            if (node->GetHidden())
                continue;

            for (Node* next : node->GetOutputs())
            {
                if (node->GetHidden())
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
    void DrawNodes()
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

    bool FindSeeds(std::vector<Node*>* buff, Node* node)
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
    void AddNode(Node* newNode)
    {
        if (!newNode)
            return;

        if (std::find(nodes.begin(), nodes.end(), newNode) == nodes.end())
            nodes.push_back(newNode);
    }

    void ConnectNodes(Node* start, Node* end)
    {
        auto endIt   = std::find(start->GetInputs() .begin(), start->GetInputs() .end(), end  );
        auto startIt = std::find(end  ->GetOutputs().begin(), end  ->GetOutputs().end(), start);

        if (!(endIt   == start->GetInputs() .end() &&
              startIt == end  ->GetOutputs().end()))
            return;

        start->AddOutput(end  );
        end  ->AddInput (start);
    }
    void DisconnectNodes(Node* start, Node* end)
    {
        start->RemoveOutput(end);
        end->RemoveInput(start);
    }

    // Erases a node from existence
    void RemoveNode(Node* node)
    {
        if (!node)
            return;

        auto it = std::find(nodes.begin(), nodes.end(), node);
        if (it != nodes.end())
        {
            node->ClearReferencesToSelf();
            nodes.erase(it);

            CalculateRoute();
        }
    }

    Node* FindNodeAtPosition(Vector2 position, float radius)
    {
        float shortestDistance = radius;
        Node* closest = nullptr;
        for (Node* node : nodes)
        {
            float distance = Vector2Distance(position, node->GetPosition());
            if (distance < shortestDistance)
            {
                shortestDistance = distance;
                closest = node;
            }
        }
        return closest;
    }

    Wire FindWireIntersectingPosition(Vector2 position, float radius)
    {
        float shortestDistance = radius;
        Wire closest = { nullptr, nullptr };
        ResetVisited();
        for (Node* start : nodes)
        {
            if (start->GetVisited())
                continue;

            for (Node* end : start->GetOutputs())
            {
                if (end->GetVisited())
                    break;

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

    void FindNodesInRectangle(std::vector<Node*>* output, Rectangle search)
    {
        output->clear();
        for (Node* node : nodes)
        {
            if (CheckCollisionPointRec(node->GetPosition(), search))
                output->push_back(node);
        }
    }
};

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

void Save(Graph* graph)
{
    // Open the file for writing
    std::ofstream file("save.graph");

    // Write the number of nodes
    file << graph->nodes.size() << '\n';

    // Write node data
    for (Node* node : graph->nodes)
    {
        file << node->GetPosition().x << ' ' << node->GetPosition().y << ' ' << (char)node->GetGate() << '\n';
    }

    // Count wires
    size_t connections = 0;
    graph->ResetVisited();
    for (Node* start : graph->nodes)
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
    file << '\n' << connections << '\n';

    // Write wire data as relative indices
    graph->ResetVisited();
    for (Node* start : graph->nodes)
    {
        if (start->GetVisited())
            continue;

        // Find the index of the start node
        size_t a = std::find(graph->nodes.begin(), graph->nodes.end(), start) - graph->nodes.begin();

        // We don't need to look at the inputs because that information is implied by the outputs
        for (Node* end : start->GetOutputs())
        {
            if (end->GetVisited())
                break;

            // Find the index of the end node
            size_t b = std::find(graph->nodes.begin(), graph->nodes.end(), end) - graph->nodes.begin();

            // Write the wire
            file << a << ' ' << b << '\n';
        }
    }

    // Close the file
    file.close();
}
void Load(Graph* graph)
{
    std::ifstream file("save.graph");

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

    // Read number of wires
    file >> count;

    // Read wire data
    for (size_t i = 0; i < count; ++i)
    {
        size_t a, b;
        file >> a >> b;
        graph->ConnectNodes(graph->nodes[a], graph->nodes[b]);
    }

    file.close();
}

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    Graph graph;
    bool graphDirty = false;
    bool gateDirty = false;
    bool drawGateOptions = false;

    Node* selectionStart = nullptr;
    Node* selectionEnd = nullptr;

    bool marqueeSelecting = false;
    bool marqueeDragging = false;
    Vector2 marqueeStart = Vector2Zero();
    Rectangle selectionRec;
    std::vector<Node*> selection;

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        // Saving and loading
        if (IsKeyPressed(KEY_S) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)))
        {
            if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) // Load
                Load(&graph);
            else // Save
                Save(&graph);
        }


        // Grid-snapped cursor
        Vector2 cursor = Snap(GetMousePosition(), g_nodeRadius * 2.0f);
        // Lock wire directions
        if (selectionStart)
        {
            float xSize = abs(selectionStart->GetPosition().x - cursor.x);
            float ySize = abs(selectionStart->GetPosition().y - cursor.y);
            float shortLength = std::min(xSize, ySize);
            float longLength = std::max(xSize, ySize);
            if (shortLength > longLength * 0.5)
            {
                cursor = {
                    selectionStart->GetPosition().x + (selectionStart->GetPosition().x > cursor.x ? -shortLength : shortLength),
                    selectionStart->GetPosition().y + (selectionStart->GetPosition().y > cursor.y ? -shortLength : shortLength)
                };
            }
            else
            {
                if (xSize < ySize)
                    cursor.x = selectionStart->GetPosition().x;
                else
                    cursor.y = selectionStart->GetPosition().y;
            }
        }
        // Hover
        Node* hoveredNode = graph.FindNodeAtPosition(cursor, g_nodeRadius * 2.0f);
        Wire hoveredWire = (hoveredNode ? Wire{ nullptr, nullptr }  : graph.FindWireIntersectingPosition(cursor, g_nodeRadius * 2.0f));

        if (!marqueeSelecting)
        {
            if (!selection.empty())
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    marqueeStart = cursor;
                    marqueeDragging = true;
                }

                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                {
                    marqueeDragging = false;
                    Vector2 offset = cursor - marqueeStart;

                    // Copy
                    if (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT))
                    {
                        std::vector<Node*> copies;

                        copies.reserve(selection.size());
                        for (Node* src : selection)
                        {
                            copies.push_back(new Node(src->GetPosition() + offset, src->GetGate()));
                            graph.AddNode(copies.back());
                        }

                        graph.ResetVisited();
                        for (Node* start : selection)
                        {
                            if (start->GetVisited())
                                continue;

                            size_t a = std::find(selection.begin(), selection.end(), start) - selection.begin();

                            for (Node* end : start->GetOutputs())
                            {
                                if (end->GetVisited())
                                    break;

                                auto it = std::find(selection.begin(), selection.end(), end);

                                if (it != selection.end())
                                {
                                    size_t b = it - selection.begin();
                                    graph.ConnectNodes(copies[a], copies[b]);
                                }
                            }
                        }
                    }
                    // Move
                    else
                    {
                        for (Node* node : selection)
                        {
                            node->SetPosition(node->GetPosition() + offset);
                        }
                    }
                }
            }
            else
            {
                // Drag
                if (selectionStart)
                {
                    selectionEnd = hoveredNode;

                    // Finalize
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                    {
                        if (selectionEnd != selectionStart)
                        {
                            if (!selectionEnd)
                            {
                                selectionEnd = new Node(cursor, Gate::OR);
                                graph.AddNode(selectionEnd);
                            }
                            graph.ConnectNodes(selectionStart, selectionEnd);
                            graphDirty = true;

                            // Divide wire
                            if (hoveredWire.a)
                            {
                                graph.DisconnectNodes(hoveredWire.a, hoveredWire.b);
                                graph.ConnectNodes(hoveredWire.a, selectionEnd);
                                graph.ConnectNodes(selectionEnd, hoveredWire.b);
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
                        selectionStart = new Node(cursor, Gate::OR);
                        hoveredNode = selectionStart;
                        graph.AddNode(selectionStart);
                        graphDirty = true;

                        // Divide wire
                        if (hoveredWire.a)
                        {
                            graph.DisconnectNodes(hoveredWire.a, hoveredWire.b);
                            graph.ConnectNodes(hoveredWire.a, selectionStart);
                            graph.ConnectNodes(selectionStart, hoveredWire.b);
                        }
                    }
                }

                drawGateOptions = (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON) && selectionStart);
                if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON) && !selectionStart)
                {
                    if (hoveredNode)
                    {
                        selectionStart = hoveredNode;
                    }
                    else if (hoveredWire.a)
                    {
                        graph.DisconnectNodes(hoveredWire.a, hoveredWire.b);
                        graph.ConnectNodes(hoveredWire.b, hoveredWire.a);
                        hoveredWire = { hoveredWire.b, hoveredWire.a };
                        graphDirty = true;
                    }
                }
                if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON) && selectionStart)
                {

                    // Middle click
                    if (cursor.x == selectionStart->GetPosition().x &&
                        cursor.y == selectionStart->GetPosition().y)
                    {
                        if (selectionStart->GetGate() != Gate::NOR)
                            selectionStart->SetGate(Gate::NOR);
                        else
                            selectionStart->SetGate(Gate::OR);
                    }
                    else
                        // Middle drag
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

                    selectionStart = nullptr;
                    selectionEnd = nullptr;
                }
            }
        }

        // Delete selected
        if ((IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_BACKSPACE)) && !selection.empty())
        {
            for (Node* node : selection)
            {
                graph.RemoveNode(node);
                delete node;
            }
            selection.clear();
        }

        // Make sure no other locally stored nodes can possibly exist at the time, or set them to nullptr
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && !selectionStart && !selectionEnd && !marqueeSelecting && selection.empty())
        {
            if (hoveredNode)
            {
                graph.RemoveNode(hoveredNode);
                delete hoveredNode;
                hoveredNode = nullptr;
                graphDirty = true;
            }
            else if (hoveredWire.a)
            {
                graph.DisconnectNodes(hoveredWire.a, hoveredWire.b);
                hoveredWire = { nullptr, nullptr };
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
            graph.FindNodesInRectangle(&selection, selectionRec);

            // Finalize
            if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON))
                marqueeSelecting = false;
        }

        // Package up components
        if (IsKeyPressed(KEY_SPACE) && !selection.empty())
        {
            ComponentBlueprint* blueprint = new ComponentBlueprint(selection);
            
            Component* component = new Component(blueprint);
            component->Regenerate();
        }


        // Cleanup
        if (graphDirty)
        {
            graph.CalculateRoute();
            graphDirty = false;
        }
        if (gateDirty)
        {
            graph.ResetStates();
            gateDirty = false;
        }
        graph.Step();

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
                DrawRectangleLines(selectionRec.x, selectionRec.y, selectionRec.width, selectionRec.height, GRAY);
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
                    int halfWidth = lroundf(sqrtf(gridRadius * gridRadius - y * y));
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
            graph.DrawWires();

            // Hovered wire
            if (hoveredWire.a && !drawGateOptions)
                DrawLineV(hoveredWire.a->GetPosition(), hoveredWire.b->GetPosition(), LIGHTGRAY);

            // Moving points
            if (marqueeDragging)
            {
                Vector2 offset = cursor - marqueeStart;
                for (Node* node : selection)
                {
                    DrawCircleV(node->GetPosition() + offset, 2.0f, MAGENTA);
                }
            }

            // Selection
            for (Node* selected : selection)
            {
                DrawGateIcon(selected->GetGate(), selected->GetPosition(), g_nodeRadius + 2.0f, WHITE); // todo
            }

            // Nodes
            graph.DrawNodes();

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

    for (Node* node : graph.nodes)
    {
        delete node;
    }

    CloseWindow();

	return 0;
}
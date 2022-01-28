#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <fstream>

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

constexpr float g_nodeRadius = 4.0f;
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

    std::vector<Node*> m_inputs;
    std::vector<Node*> m_outputs;

public:
    Node(Vector2 position, Gate type) : m_position(position), m_type(type), b_state(false), b_visited(false), m_inputs{}, m_outputs{} {}

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
        constexpr Vector2 offsets[3] = {
            {  0.0f, -1.1547f },
            { -1.0f, 0.5774f },
            { +1.0f, 0.5774f }
        };
        Vector2 points[3];
        for (size_t i = 0; i < 3; ++i)
        {
            points[i] = center + offsets[i] * radius;
        }
        DrawTriangle(points[0], points[1], points[2], color);

        if (type == Gate::XOR && drawXORline)
        {
            float outlineRadius = radius + 2.0f;
            for (size_t i = 1; i < 8; ++i)
            {
                points[i] = center + offsets[i] * outlineRadius;
            }
            DrawTriangleLines(points[0], points[1], points[2], color);
        }
    }
        break;

    case Gate::NOR:
        DrawCircleV(center, radius, color);
        break;

    case Gate::AND:
    {
        float diameter = radius * 2.0f;
        DrawRectangleV(center - Vector2{ radius, radius }, { diameter, diameter }, color);
    }
        break;
    }
}

struct Wire
{
    Node* a;
    Node* b;
};

struct Graph
{
    size_t seedCount = 0;
    bool routeDirty = false;
    bool gateDirty = false;
    // Sort using CalculateRoute() every time a node is added or removed
    std::vector<Node*> nodes;

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
            if (node->GetGate() == Gate::XOR)
            {
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
            }
            else
            {
                bool finished = node->GetGate() == Gate::AND;
                for (Node* input : node->GetInputs())
                {
                    if (input->GetState())
                    {
                        node->SetState(node->GetGate() != Gate::NOR);
                        finished = node->GetGate() != Gate::AND;
                        break;
                    }
                }
                if (!finished)
                    node->SetState(node->GetGate() == Gate::NOR);
            }
        }
    }

    // Runs every frame
    void DrawWires()
    {
        // Draw the wires first so the nodes can be drawn on top
        for (Node* node : nodes)
        {
            for (Node* next : node->GetOutputs())
            {
                Color color = node->GetState() ? DARKBLUE : DARKGRAY;
                DrawLineV(node->GetPosition(), next->GetPosition(), color);

                Vector2 midpoint = (node->GetPosition() + next->GetPosition()) * 0.5f;
                Vector2 angle = Vector2Rotate(Vector2Normalize(next->GetPosition() - node->GetPosition()) * g_gridUnit, 135.0f);
                DrawLineV(midpoint, midpoint + angle, color);
                angle = Vector2Rotate(Vector2Normalize(next->GetPosition() - node->GetPosition()) * g_gridUnit, -135.0f);
                DrawLineV(midpoint, midpoint + angle, color);
            }
        }
    }
    void DrawNodes()
    {
        // Node drawing is separate so that cyclic graphs can still draw nodes on top
        for (Node* node : nodes)
        {
            DrawGateIcon(node->GetGate(), node->GetPosition(), g_nodeRadius + 2.0f, GRAY);
            DrawGateIcon(node->GetGate(), node->GetPosition(), g_nodeRadius, (node->GetState() ? BLUE : BLACK), false);
            if (node->HasNoInputs())
                DrawGateIcon(node->GetGate(), node->GetPosition(), g_nodeRadius - 2.0f, GREEN, false);
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

    Graph graph;
    bool graphDirty = false;
    bool gateDirty = false;

    Node* selectionStart = nullptr;
    Node* selectionEnd = nullptr;

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        if (IsKeyPressed(KEY_S) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)))
        {
            if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) // Load
            {
                std::ifstream file("save.graph");
                size_t count;
                file >> count;
                graph.nodes.reserve(count);
                for (size_t i = 0; i < count; ++i)
                {
                    Vector2 pos;
                    char gate;
                    file >> pos.x >> pos.y >> gate;
                    graph.AddNode(new Node(pos, (Gate)gate));
                }
                file >> count;
                for (size_t i = 0; i < count; ++i)
                {
                    size_t a, b;
                    file >> a >> b;
                    graph.ConnectNodes(graph.nodes[a], graph.nodes[b]);
                }
                file.close();
            }
            else // Save
            {
                std::ofstream file("save.graph");
                file << graph.nodes.size() << '\n';
                for (Node* node : graph.nodes)
                {
                    file << node->GetPosition().x << ' ' << node->GetPosition().y << ' ' << (char)node->GetGate() << '\n';
                }
                size_t connections = 0;
                graph.ResetVisited();
                for (Node* start : graph.nodes)
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
                file << '\n' << connections << '\n';
                graph.ResetVisited();
                for (Node* start : graph.nodes)
                {
                    if (start->GetVisited())
                        continue;

                    for (Node* end : start->GetOutputs())
                    {
                        if (end->GetVisited())
                            break;

                        size_t a = std::find(graph.nodes.begin(), graph.nodes.end(), start) - graph.nodes.begin();
                        size_t b = std::find(graph.nodes.begin(), graph.nodes.end(), end) - graph.nodes.begin();

                        file << a << ' ' << b << '\n';
                    }
                }
                file.close();
            }
        }

        Vector2 cursor = Snap(GetMousePosition(), g_nodeRadius * 2.0f);
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

        Node* hoveredNode = graph.FindNodeAtPosition(cursor, g_nodeRadius * 2.0f);
        Wire hoveredWire = (hoveredNode ? Wire{ nullptr, nullptr }  : graph.FindWireIntersectingPosition(cursor, g_nodeRadius * 2.0f));

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
                }
                selectionStart = selectionEnd = nullptr;
            }
        }
        else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            selectionStart = hoveredNode;

            if (!selectionStart)
            {
                selectionStart = new Node(cursor, Gate::OR);
                hoveredNode = selectionStart;
                graph.AddNode(selectionStart);
                graphDirty = true;
            }
        }

        if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON))
        {
            if (hoveredNode)
            {
                switch (hoveredNode->GetGate())
                {
                case Gate::OR:  hoveredNode->SetGate(Gate::NOR); break;
                case Gate::NOR: hoveredNode->SetGate(Gate::AND); break;
                case Gate::AND: hoveredNode->SetGate(Gate::XOR); break;
                case Gate::XOR: hoveredNode->SetGate(Gate::OR ); break;
                }
                if (!hoveredNode->HasNoInputs()) // Inputless nodes take output from the user and therefore are treated as mutable
                    gateDirty = true;
            }
            else if (hoveredWire.a)
            {
                graph.DisconnectNodes(hoveredWire.a, hoveredWire.b);
                graph.ConnectNodes(hoveredWire.b, hoveredWire.a);
                hoveredWire = { hoveredWire.b, hoveredWire.a };
                graphDirty = true;
            }
        }
        
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        {
            if (hoveredNode)
            {
                if (hoveredNode == selectionStart)
                    selectionStart = nullptr;
                if (hoveredNode == selectionEnd)
                    selectionEnd = nullptr;

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
        }

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

            graph.DrawWires();

            if (hoveredWire.a)
                DrawLineV(hoveredWire.a->GetPosition(), hoveredWire.b->GetPosition(), LIGHTGRAY);

            graph.DrawNodes();

            if (hoveredNode)
            {
                DrawRectanglePro({ cursor.x,cursor.y, g_nodeRadius * 4.0f, g_nodeRadius * 4.0f }, { g_nodeRadius * 2.0f, g_nodeRadius * 2.0f }, 45, WHITE);
                DrawRectanglePro({ cursor.x,cursor.y, g_nodeRadius * 3.0f, g_nodeRadius * 3.0f }, { g_nodeRadius * 1.5f, g_nodeRadius * 1.5f }, 45, BLACK);

                DrawGateIcon(hoveredNode->GetGate(), hoveredNode->GetPosition(), g_nodeRadius + 2.0f, GRAY);
            }
            else
            {
                DrawRectanglePro({ cursor.x,cursor.y,5,5 }, { 2.5,2.5 }, 45, RAYWHITE);
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
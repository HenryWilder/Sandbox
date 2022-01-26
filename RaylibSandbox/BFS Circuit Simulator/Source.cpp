#include <raylib.h>
#include <raymath.h>
#include <vector>

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

class Node
{
private:
    Vector2 m_position;
    bool b_inverts; // Treat this as a NOR gate? If not, treat it as an OR gate
    bool b_state;
    bool b_visited;

    std::vector<Node*> m_inputs;
    std::vector<Node*> m_outputs;

public:
    Node(Vector2 position, bool inverts) : m_position(position), b_inverts(inverts), b_state(false), b_visited(false), m_inputs{}, m_outputs{} {}

    Vector2 GetPosition() const
    {
        return m_position;
    }
    void SetPosition(Vector2 position)
    {
        m_position = position;
    }

    bool GetInverts() const
    {
        return b_inverts;
    }
    void SetInverts(bool inverts)
    {
        b_inverts = inverts;
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

struct Graph
{
    // Sorted using CalculateRoute() every time a node is added or removed
    std::vector<Node*> nodes;

    // Runs every tick
    void Step()
    {
        for (Node* node : nodes)
        {
            for (Node* input : node->GetInputs())
            {
                if (input->GetState())
                {
                    node->SetState(!node->GetInverts());
                    break;
                }
            }
        }
    }
    // Runs every frame
    void Draw()
    {
        // Draw the wires first so the nodes can be drawn on top
        for (Node* node : nodes)
        {
            for (Node* output : node->GetOutputs())
            {
                DrawLineV(node->GetPosition(), output->GetPosition(), (node->GetState() ? DARKBLUE : DARKGRAY));
            }
        }
        // Node drawing is separate so that cyclic graphs can still draw nodes on top
        for (Node* node : nodes)
        {
            DrawCircleV(node->GetPosition(), g_nodeRadius, (node->GetState() ? BLUE : GRAY));
        }
    }

    // Uses modified BFS
    // Make sure to run this before simulating a step when the graph has changed
    void CalculateRoute()
    {
        // Clear node visitation status
        for (Node* node : nodes)
        {
            node->SetVisited(false);
        }

        size_t totalNodes = nodes.size();

        // Put the "seed" nodes at the start
        {
            std::vector<Node*> buff;
            buff.reserve(totalNodes);
            for (Node* node : nodes)
            {
                if (node->HasNoInputs())
                {
                    buff.push_back(node);
                    node->SetVisited(true);
                }
            }
            nodes.swap(buff);
        }

        size_t startIndex = 0;
        size_t endIndex;
        while (nodes.size() < totalNodes)
        {
            endIndex = nodes.size();
            for (size_t i = startIndex; i < endIndex; ++i)
            {
                for (Node* output : nodes[i]->GetOutputs())
                {
                    if (!output->GetVisited())
                    {
                        nodes.push_back(output);
                        output->SetVisited(true);
                    }
                }
            }
            startIndex = endIndex;
        }
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
        start->AddOutput(end);
        end->AddInput(start);
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

    bool dirty = false;
    Graph graph;

    Node* selectionStart = nullptr;
    Node* selectionEnd = nullptr;

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        if (selectionStart)
        {
            selectionEnd = graph.FindNodeAtPosition(GetMousePosition(), g_nodeRadius * 2.0f);

            // Finalize
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                if (selectionEnd != selectionStart)
                {
                    if (!selectionEnd)
                    {
                        selectionEnd = new Node(GetMousePosition(), false);
                        graph.AddNode(selectionEnd);
                    }
                    graph.ConnectNodes(selectionStart, selectionEnd);
                    dirty = true;
                }
                selectionStart = selectionEnd = nullptr;
            }
        }
        else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            selectionStart = graph.FindNodeAtPosition(GetMousePosition(), g_nodeRadius * 2.0f);

            if (!selectionStart)
            {
                selectionStart = new Node(GetMousePosition(), false);
                graph.AddNode(selectionStart);
                dirty = true;
            }
        }

        if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON))
        {
            Node* nodeToChange = graph.FindNodeAtPosition(GetMousePosition(), g_nodeRadius * 2.0f);
            nodeToChange->SetInverts(!nodeToChange->GetInverts());
        }

        if (dirty)
        {
            graph.CalculateRoute();
            dirty = false;
        }

        graph.Step();

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            if (selectionStart)
            {
                Vector2 end;
                if (selectionEnd)
                    end = selectionEnd->GetPosition();
                else
                    end = GetMousePosition();

                DrawLineV(selectionStart->GetPosition(), end, GRAY);
            }

            graph.Draw();

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
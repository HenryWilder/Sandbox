#pragma once

constexpr float g_nodeRadius = 6.0f;
constexpr float g_gridUnit = g_nodeRadius * 2.0f;

enum class Gate : char
{
    OR = '|',
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
    mutable bool b_visited;
    bool b_hidden;
    uint8_t m_componentFlags; // 00 = normal; 01 = comp input; 10 = comp output; 11 = comp internal; !00 = in component; (!00 && !11) = component external
    Component* m_container;

    std::vector<Node*> m_inputs; // Mostly used for clearing references
    std::vector<Node*> m_outputs;

public:
    Node(Vector2 position, Gate type) :
        m_position(position),
        m_type(type),
        b_state(false),
        b_visited(false),
        b_hidden(false),
        m_componentFlags(0b00),
        m_container(nullptr),
        m_inputs{},
        m_outputs{}
    {}

    Vector2 GetPosition() const;
    void SetPosition(Vector2 position);

    Gate GetGate() const;
    void SetGate(Gate type);

    bool GetState() const;
    void SetState(bool state);

    bool GetVisited() const;
    void SetVisited(bool visited) const;

    bool GetHidden() const;
    void SetHidden(bool hidden);

    bool IsComponentExternal() const;
    bool IsComponentInput() const;
    bool IsComponentOutput() const;
    bool IsInComponent() const;
    void SetNotInComponent();
    void SetComponentInput();
    void SetComponentOutput();
    void SetComponentInternal();

    Component* GetContainingComponent() const;
    void SetContainingComponent(Component* comp);

    bool HasNoInputs() const;
    bool InputCount() const;
    const std::vector<Node*>& GetInputs() const;
    Node* GetInput(size_t index) const;

    void AddInput(Node* input);
    void RemoveInput(const Node* input);

    bool HasNoOutputs() const;
    bool OutputCount() const;
    const std::vector<Node*>& GetOutputs() const;
    Node* GetOutput(size_t index) const;

    void AddOutput(Node* output);
    void RemoveOutput(const Node* output);

    // Warning: Affects input/output nodes
    // Always use this before deleting the node
    // It *can* be skipped when it is certain no references to this node can possibly exist
    // Or if it is certain all inputs and outputs are also being deleted
    // Forgetting to call this function when removing a node with references to it can lead to a crash!
    void ClearReferencesToSelf();
};

void DrawGateIcon(Gate type, Vector2 center, float radius, Color color, bool drawXORline = true);
void DrawHighlightedGate(Vector2 position, Gate type);

struct Wire
{
    Node* a;
    Node* b;
};

struct WireRelative
{
    size_t a;
    size_t b;
};

bool IsNodeInVector(const std::vector<Node*>& vector, const Node* node);
bool NodeIndexInVector(const std::vector<Node*>& vector, const Node* node, size_t* index);

// Helper function for finding the relative indices of nodes in a vector
// Destination map is expected to be empty
void MapNodeRelativeIndices(const std::vector<Node*>& source, std::unordered_map<const Node*, size_t>& dest);

// Helper function for finding the relative indices of nodes for all contained wires in a vector
// Destination vector is expected to be empty
void ListWireRelativeIndices(const std::vector<Node*>& source, std::vector<WireRelative>& dest);

#pragma once

enum class Gate : char;
class Node;

struct NodeBlueprint
{
    Gate gate;
    std::vector<size_t> outputs;
};


struct NodeBP_Helper
{
    Gate gate;
    Vector2 position;
    bool noInputs;
    std::vector<NodeBP_Helper*> outputs;

    bool operator<(const NodeBP_Helper& other) const;
};


struct ComponentBlueprint
{
    ComponentBlueprint();
    ComponentBlueprint(size_t inputs, size_t outputs);

    size_t inputs;
    size_t outputs;
    std::vector<NodeBlueprint> nodes; // Partitions: [inputs|outputs|internals]

    void GenerateBlueprintFromSelection(const std::vector<Node*>& source);
};


class Component
{
private:
    ComponentBlueprint* m_blueprint;
    Rectangle m_casingLeft;
    Rectangle m_casingRight;
    std::vector<Node*> m_nodes; // Sorted in blueprint

public:
    Component(ComponentBlueprint* blueprint);

    ComponentBlueprint* const GetBlueprint() const;
    void SetBlueprint(ComponentBlueprint* blueprint);
    // Parameters are vectors of the nodes that need to be added/removed
    void Regenerate(std::vector<Node*>* add, std::vector<Node*>* remove);

    size_t GetInputCount() const;
    size_t GetOutputCount() const;
    size_t GetInternalCount() const;
    size_t GetNodeCount() const;

    Rectangle GetCasingA() const;
    Rectangle GetCasingB() const;
    Vector2 GetPosition() const;
    void SetPosition(Vector2 topLeft);

    Node* GetNode(size_t index) const;
    const std::vector<Node*>& GetNodes() const;
    std::vector<Node*>& GetNodesVector();
};

#pragma once

struct ComponentBlueprint;
class Component;
enum class Gate : char;
class Node;
struct Wire;

float Vector2DistanceToLine(Vector2 startPos, Vector2 endPos, Vector2 point);
bool CheckCollisionLineCircle(Vector2 startPos, Vector2 endPos, Vector2 center, float radius);

struct Selectable
{
    Selectable(Node* node) :
        type(Type::NODE),
        node(node)
    {}

    Selectable(Component* comp) :
        type(Type::COMP),
        comp(comp)
    {}

    enum class Type
    {
        NODE,
        COMP,
    } type;

    union
    {
        Node* node;
        Component* comp;
    };

    bool operator==(const Selectable& other) const;
};

class Graph
{
private:
    size_t seedCount = 0;
    bool routeDirty = false;
    bool gateDirty = false;
    // Sort using CalculateRoute() every time a node is added or removed
    std::vector<Node*> nodes;
    std::vector<Component*> components;

public:

    bool IsNodeInGraph(const Node* node) const;
    bool FindNodeIndexInGraph(const Node* node, size_t* index) const;
    size_t NodeIndexInGraph(const Node* node) const;

    void ResetVisited() const;
    void ResetStates();

    // For components
    void RegenerateSingleComponent(Component* component);
    void RegenerateComponents(std::vector<Component*>& components);
    void RegenerateAllComponents();

    void CloneComponentAtPosition(ComponentBlueprint* blueprint, Vector2 position);

    void MarkRouteDirty();
    void MarkGateDirty();
    void UnDirty();

    // Uses modified BFS
    void CalculateRoute();

    // Runs every tick
    void Step();

    // Runs every frame
    void DrawComponents() const;
    void DrawWires() const;
    void DrawNodes() const;

    bool FindSeeds(std::vector<Node*>* buff, Node* node);

    // Create a node without connections
    void AddNode(Node* newNode);

    void ConnectNodes(Node* start, Node* end);
    void DisconnectNodes(Node* start, Node* end);

    // Erases a node from existence
    void RemoveNode(Node* node);
    void RemoveComponent(Component* comp);

    Node* FindNodeAtPosition(Vector2 position, float radius) const;
    Wire FindWireIntersectingPosition(Vector2 position, float radius) const;
    void FindSelectablesInRectangle(std::vector<Selectable>* output, Rectangle search) const;
    Component* FindComponentAtPosition(Vector2 position) const;

    void Save(const char* filename);
    void Load(const char* filename);

    const std::vector<Node*>& GetNodes() const;
};

#pragma once
#include <vector>

struct ComponentBlueprint;
class Component;
enum class Gate : char;
class Node;
struct Wire;

bool Vector2Equals(Vector2 a, Vector2 b);

float Vector2DistanceToLine(Vector2 startPos, Vector2 endPos, Vector2 point);
bool CheckCollisionLineCircle(Vector2 startPos, Vector2 endPos, Vector2 center, float radius);

struct Selection
{
    bool dirty = true; // Track at your own leisure
    std::vector<Node*> nodes;
    std::vector<Component*> comps;

    void AccountForComponentNodes();

    inline void Dirty()
    {
        dirty = true;
    }
    inline void Clean()
    {
        if (dirty)
            AccountForComponentNodes();
        dirty = false;
    }
};

class Graph
{
public:
    // Use this to access the (only) graph
    static Graph& Global()
    {
        static Graph singleton; // Instantiated on first use
        return singleton;
    }

private:
    Graph() {}

    size_t seedCount = 0;
    bool routeDirty = false;
    bool gateDirty = false;
    std::vector<Node*> nodes;
    std::vector<ComponentBlueprint*> blueprints;
    std::vector<Component*> components;

public:
    Graph(Graph const&) = delete;
    void operator=(Graph const&) = delete;

    /******************************************
    * 
    * #Region#
    * 
    * Get
    * 
    * Existence Check
    * Collision Check
    * Collision Find
    * 
    * Add
    * Clone
    * 
    * Remove
    * Edit (offset)
    * 
    * Drawers
    * 
    * 
    * Type-Specific Helpers
    * 
    * #End Region#
    * 
    ******************************************/

#pragma region Nodes

    const std::vector<Node*>& GetNodes() const;

    bool IsNodeInGraph(const Node* node) const;

    void AddNode(Node* newNode);
    void AddNodes(const std::vector<Node*>& source);
    Node* AddNewNode(Vector2 position); // Warning! Calls new

    Node* CloneNodeAndOffset(Node* source, Vector2 offset);
    void CloneNodesAndOffset(std::vector<Node*>& result, const std::vector<Node*>& source, Vector2 offset);

    // Erases a node from existence
    void RemoveNode(Node* node);
    void RemoveMultipleNodes(const std::vector<Node*>& remove);
    // Note: If the connections cannot be transfered cleanly the node will not be deleted and instead be made an OR gate.
    // Returns true if the node was removed and is safe to delete. False means the node is still in use and should not be deleted.
    bool RemoveNodeAndTransferConnections(Node* node);
    void RemoveNodeAndDelete(Node* node); // Warning! Calls delete

    Node* FindNodeAtGridPoint(Vector2 point) const; // Returns the first encountered node that meets the criteria

    void DrawNodes() const;

    void OffsetNodes(const Selection& source, Vector2 offset);
    bool FindNodeIndexInGraph(const Node* node, size_t* index) const;
    size_t NodeIndexInGraph(const Node* node) const;
    void ResetVisited() const;
    void ResetStates();

#pragma endregion
#pragma region Wires

    void GetWires(std::vector<Wire>& result) const; // Expensive! Only call this if you need ALL wires for some reason
    Wire GetWire(size_t index) const; // In order of evaluation

    void CreateWire(Vector2 start, Vector2 end); // Warning! Calls new
    void ConnectNodes(Node* start, Node* end);

    void RemoveWire(Wire wire); // Also removes connected nodes
    void RemoveWireAndDelete(Wire wire); // Warning! Calls delete
    void DisconnectNodes(Node* start, Node* end);

    Wire FindWireIntersectingGridPoint(Vector2 point) const; // Returns the first encountered wire that meets the criteria

    void DrawWires() const;

    WireRelative WireAsIndices(Wire wire) const;
    void DivideWire(Wire wire, Node* interloper);
    Wire ReverseWire(Wire wire);

#pragma endregion
#pragma region Components

    const std::vector<Component*>& GetComponents() const;
    void RegenerateSingleComponent(Component* component);
    void RegenerateComponents(std::vector<Component*>& components);
    void RegenerateAllComponents();

    void RemoveComponent(Component* comp);
    void RemoveMultipleComponents(const std::vector<Component*>& remove);
    void RemoveComponentEntirely(Component* comp); // Warning! Calls delete
    Component* FindComponentAtPosition(Vector2 position) const;

    // Returns the pointer to the clone
    Component* CloneComponentAtPosition(ComponentBlueprint* blueprint, Vector2 position);

    void DrawComponents() const;

#pragma endregion
#pragma region Selection

    void FindObjectsInRectangle(Selection* output, Rectangle search) const;
    void RemoveObjects(const Selection& remove);

#pragma endregion
#pragma region Other

    void MarkRouteDirty();
    void MarkGateDirty();
    void UnDirty();

    bool FindSeeds(std::vector<Node*>* buff, Node* node);
    // Uses modified BFS
    void CalculateRoute();

    // Runs every tick
    void Step();

    void Save(const char* filename);
    void Load(const char* filename);

#pragma endregion
};

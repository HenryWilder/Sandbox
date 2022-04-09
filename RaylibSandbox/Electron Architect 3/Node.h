#pragma once
#include <vector>
#include "ivec.h"

class Node;

struct Wire
{
	static constexpr float g_elbowRadius = 1.0f;
	IVec2 elbow;

	Node* start;
	Node* end;

	Wire() = default;
	Wire(Node* start, Node* end) : elbow(), start(start), end(end) {}
};

enum class Gate
{
	OR = '|',
	AND = '&',
	NOR = '!',
	XOR = '^',
};

class Node
{
private:
	static constexpr float g_nodeRadius = 1.0f;
	IVec2 pos;
	Gate gate;

	std::vector<Wire*> wires;

public:
	Node() = default;
	Node(IVec2 pos, Gate gate) : pos(pos), gate(gate) {}

	IVec2 GetPos() const;
	Gate GetGate() const;
	const std::vector<Wire*>& GetWires() const;
	void SetPos(IVec2 pos);
	void SetGate(Gate gate);
};

class NodeWorld
{
private:
	std::vector<Wire*> wires;
	std::vector<Node*> nodes;

public:
	void InsertNode(Node* node);
	void RemoveNode(Node* node);
};

#include "Node.h"
#include <algorithm>

IVec2 Node::GetPos() const
{
	return pos;
}
Gate Node::GetGate() const
{
	return gate;
}
const std::vector<Wire*>& Node::GetWires() const
{
	return wires;
}
void Node::SetPos(IVec2 pos)
{
	this->pos = pos;
}
void Node::SetGate(Gate gate)
{
	this->gate = gate;
}

void NodeWorld::InsertNode(Node* node)
{
	_ASSERT_EXPR(!!node, "Cannot insert nullptr as node.");
	_ASSERT_EXPR(std::find(nodes.begin(), nodes.end(), node) == nodes.end(), "Tried to re-insert existing node.");
	if (!node->GetWires().empty())
	{
		std::find(nodes.begin(), nodes.end(), node)
	}
	else
	{

	}
	nodes.insert(nodes.begin(), node);
}

void NodeWorld::RemoveNode(Node* node)
{
	_ASSERT_EXPR(!!node, "Cannot remove nullptr.");
	auto it = std::find(nodes.begin(), nodes.end(), node);
	_ASSERT_EXPR(it != nodes.end(), "Tried to remove non-existing node.");
	nodes.erase(it);
}

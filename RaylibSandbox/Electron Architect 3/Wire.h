#pragma once
#include "ivec.h"

class Node;

struct Wire
{
	static constexpr float g_elbowRadius = 1.0f;
	IVec2 elbow;

	Node* prev;
	Node* next;
};

void DrawWire(const Wire* wire, Color color);
void DrawWireElbow(IVec2 elbow);
void DrawWireElbow(const Wire* wire);
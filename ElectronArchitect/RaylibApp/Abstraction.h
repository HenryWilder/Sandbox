#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "Transistor.h"

// TODO
struct ComponentTransistor
{
	std::vector<int> inputs;	// List of indexes in the array (array is not self-contained)
	std::vector<int> outputs;	// List of indexes in the array (array is not self-contained)
	Transistor::Type type;
};

struct AbstractComponent // Blueprint for component
{
	AbstractComponent(std::vector<Transistor*> const* base);

	std::vector<ComponentTransistor> internals;

	void Spawn(Vector2 position);
};

struct Component
{
	// No custom constructor. It will be handled by the blueprint.
	~Component()
	{
		delete[] internals;
	}

	Vector2 position;
	int internalCount; // Fixed size once created
	ComponentTransistor* internals; // Inputs and outputs are implied by the io of the transistors

	void Evaluate();

	void Draw();
};

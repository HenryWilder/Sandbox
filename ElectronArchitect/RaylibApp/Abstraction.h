#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "Transistor.h"

// TODO
struct ComponentTransistor
{
	std::vector<int> inputs;	// List of indexes in the array
	std::vector<int> outputs;	// List of indexes in the array
	Transistor::Type type;
};

struct AbstractComponent // Blueprint for component
{
	AbstractComponent(std::vector<Transistor*> base);

	std::vector<ComponentTransistor> internals;
	std::vector<int> inputs;	// List of indexes in the array
	std::vector<int> outputs;	// List of indexes in the array

	void Spawn(Vector2 position);
};

struct Component
{
	~Component()
	{
		delete[] internals;
		delete[] inputs;
		delete[] outputs;
	}

	int internalCount, inputCount, outputCount; // Fixed size once created
	ComponentTransistor* internals;
	Transistor* inputs;
	Transistor* outputs;

	void Evaluate();

	void Draw();
};

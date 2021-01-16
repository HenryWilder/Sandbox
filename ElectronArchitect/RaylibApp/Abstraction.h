#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "Wire.h"
#include "Transistor.h"

// TODO
struct ComponentTransistor
{
	std::vector<ComponentTransistor*> inputs;
	std::vector<ComponentTransistor*> outputs;
	Transistor::Type type;

	void Evaluate();
};

// Note that these are blueprints on how to create the described component, not an instance of the component itself.
struct Abstraction
{
	// Copies
	std::vector<Transistor> transistors; // Make sure to push back these first so that they can have their properties taken correctly
	std::vector<Wire> wires;

	// These are pointers to the transistors in the previous vector; they refer to which transistors should be present on the outside of the component
	std::vector<Transistor*> inputs;
	std::vector<Transistor*> outputs;

	// Effectively recreates the actions of manually placing each component into the world.
	void Paste(Vector2 position)
	{
		// TODO
	}
};

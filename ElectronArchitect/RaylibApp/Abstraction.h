#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "Transistor.h"

/*******************
*
*	Circuit:
*	>---&--->
*       |
*	>---'
* 
*	Component:
*	   -------
*	  |       |
*	-->       >--
*	  |  AND  |
*	-->       |
*	  |       |
*	   -------
*
*******************/

// TODO
struct ComponentTransistor
{
	ComponentTransistor() : inputCount{ 0 }, outputCount{ 0 }, inputs{ nullptr }, outputs{ nullptr }, type{ TransistorType::Simple }, b_active{ false }, b_evaluated{ false } {};
	ComponentTransistor(Transistor* base) // Still need to set inputs and outputs
	{
		inputCount = base->inputs.size();
		outputCount = base->outputs.size();
		type = base->type;

		inputs = nullptr;
		outputs = nullptr;
		b_active = false;
		b_evaluated = false;
	}

	size_t inputCount; // 8
	size_t outputCount; // 8
	ComponentTransistor* inputs; // 8
	ComponentTransistor* outputs; // 8
	TransistorType type; // 1
	bool b_active; // 1
	bool b_evaluated; // 1
};

struct AbstractComponent // Blueprint for component
{
	AbstractComponent(std::vector<Transistor*> const* base);

	std::vector<ComponentTransistor> internals;

	void Spawn(Vector2 position);
};

void MakeAbstract(std::vector<Transistor*>& selection, Vector2 position, float g_gridSize);

struct ComponentPort
{
	Vector2 pos;
	Transistor* connection;
	void Draw();
};

class Component
{
public:
	// No custom constructor. It will be handled by the blueprint.
	~Component()
	{
		delete[] internals;
		delete[] inputs;
		delete[] outputs;
	}

	size_t inputCount;
	size_t outputCount;
	size_t internalCount;
	Vector2 pos;
	Rectangle casing;
	ComponentPort* inputs;
	ComponentPort* outputs;
	ComponentTransistor* internals; // Fixed size once created

	void Evaluate();

	void Draw();
};

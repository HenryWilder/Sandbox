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

struct Component
{
	~Component()
	{
		Erase(allComponents, this);
		for (size_t i = 0; i < internalCount; ++i)
		{
			delete internals[i];
		}
		for (size_t i = 0; i < inputCount; ++i)
		{
			delete inputs[i];
		}
		for (size_t i = 0; i < outputCount; ++i)
		{
			delete outputs[i];
		}
		delete[] internals;
		delete[] inputs;
		delete[] outputs;
	}

	static std::vector<Component*> allComponents;

	size_t inputCount;
	size_t outputCount;
	size_t internalCount;
	Vector2 pos;
	Rectangle casing;
	Transistor** inputs;
	Transistor** outputs;
	Transistor** internals; // Fixed size once created

	void Move(Vector2 deltaPos);
	void MoveAbsolute(Vector2 newPos);

	void UpdateCasing();

	void Icon(Color color);
	void Draw();
	void Highlight(Color color);

	void ClearReferences();
};
extern std::vector<Component*> allComponents;

Component* MakeAbstract(std::vector<Transistor*>& selection, Vector2 position, float g_gridSize);

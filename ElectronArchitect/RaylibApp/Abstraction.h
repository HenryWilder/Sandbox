#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>

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

class Transistor;

class Component
{
public:
	size_t inputCount;
	size_t outputCount;
	size_t internalCount;
	Vector2 pos;
	Rectangle casing;
	Transistor** inputs;
	Transistor** outputs;
	Transistor** internals; // Fixed size once created

	~Component();

	static std::vector<Component*> allComponents;

	void Move(Vector2 deltaPos);
	void MoveAbsolute(Vector2 newPos);

	void UpdateCasing();

	void Icon(Color color);
	void Draw();
	void Highlight(Color color);

	void ClearReferences();

	friend Component* MakeAbstract(std::vector<Transistor*>& selection, Vector2 position, float g_gridSize);
};
extern std::vector<Component*> allComponents;

Component* MakeAbstract(std::vector<Transistor*>& selection, Vector2 position, float g_gridSize);

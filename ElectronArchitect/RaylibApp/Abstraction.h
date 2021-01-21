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
	Component() : m_caseShape{}, m_componentTransistors{ nullptr }, m_inputCount{}, m_internalCount{}, m_outputCount{}, m_componentTransistorCount{} {};
	Component(std::vector<Transistor*>& selection, Vector2 position);
	~Component();

	static std::vector<Component*> s_allComponents;

private:
	Rectangle m_caseShape;

	// Fixed size once created
	Transistor** m_componentTransistors;
	size_t m_inputCount;
	size_t m_internalCount;
	size_t m_outputCount;
	size_t m_componentTransistorCount;
	
public:
	size_t GetInputCount() const;
	size_t GetOutputCount() const;
	size_t GetInternalCount() const;
	size_t GetTotalCount() const;

	Transistor** InputsBegin();
	Transistor** InputsEnd();
	Transistor** InternalsBegin();
	Transistor** InternalsEnd();
	Transistor** OutputsBegin();
	Transistor** OutputsEnd();

	Transistor** Begin();
	Transistor** End();

	Vector2 GetPos() const;
	Rectangle GetCaseRect() const;
	void Move(Vector2 deltaPos);
	void MoveAbsolute(Vector2 newPos);

	void UpdateCasing();

	void Icon(Color color);
	void Draw();
	void Highlight(Color color);

	void ClearReferences();
};
extern std::vector<Component*> s_allComponents;

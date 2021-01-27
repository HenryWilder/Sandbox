#pragma once
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

struct Int2;
struct IntRect;
class Transistor;

class Component
{
public:
	Component() : m_caseShape{}, m_componentTransistors{ nullptr }, m_inputCount{}, m_internalCount{}, m_outputCount{}, m_componentTransistorCount{} {};
	Component(std::vector<Transistor*>& selection, Int2 position);
	Component(const Component& src); // Copy constructor
	~Component();

	static std::vector<Component*> s_allComponents;

private:
	IntRect m_caseShape;

	// Fixed size once created
	Transistor* m_componentTransistors; // We copy the Transistors directly into this array instead of storing an array of their pointers, then delete/hollow the original.
	size_t m_inputCount;
	size_t m_internalCount;
	size_t m_outputCount;
	size_t m_componentTransistorCount;
	
public:
	size_t GetInputCount() const;
	size_t GetOutputCount() const;
	size_t GetInternalCount() const;
	size_t GetTotalCount() const;

	std::vector<Transistor*> GetInputs();
	void ForEachInput(void(*func)(Transistor& elem, size_t index));
	std::vector<Transistor*> GetInternals();
	void ForEachInternal(void(*func)(Transistor& elem, size_t index));
	std::vector<Transistor*> GetOutputs();
	void ForEachOutput(void(*func)(Transistor& elem, size_t index));
	std::vector<Transistor*> GetTransistors();
	void ForEachTransistor(void(*func)(Transistor& elem, size_t index));

	void SelectTransistors(std::vector<Transistor*>& selection);

	Int2 GetPos() const;
	IntRect GetCaseRect() const;
	void Move(Int2 deltaPos);
	void MoveAbsolute(Int2 newPos);

	void UpdateCasing();

	void Icon(Color color);
	void Draw();
	void Highlight(Color color);

	void ClearReferences();
};
extern std::vector<Component*> s_allComponents;

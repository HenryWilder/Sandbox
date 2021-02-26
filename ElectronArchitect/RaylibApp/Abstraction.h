#pragma once
#include <vector>
#include <array>

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
	Component() : m_caseShape{}, m_componentTransistors{} {};
	Component(std::vector<ID_t>& selection, Int2 position);
	Component(const Component& src); // Copy constructor

	static std::vector<Component*> s_allComponents;

private:
	IntRect m_caseShape;

	// Fixed size once created
	std::vector<ID_t> m_componentTransistors[3];
	
public:
	size_t GetInputCount() const;
	size_t GetOutputCount() const;
	size_t GetInternalCount() const;
	size_t GetTotalCount() const;

	std::vector<ID_t>& GetInputs();
	std::vector<ID_t>& GetInternals();
	std::vector<ID_t>& GetOutputs();
	std::vector<ID_t>  GetTransistors() const;
	std::vector<ID_t&>  GetTransistorRefs();

	void SelectTransistors(std::vector<ID_t>& selection);

	Int2 GetPos() const;
	IntRect GetCaseRect() const;
	void Move(Int2 deltaPos);
	void MoveAbsolute(Int2 newPos);

	void UpdateCasing();

	void Icon(Color color);
	void Draw();
	void Highlight(Color color);

	void Destroy();
};
extern std::vector<Component*> s_allComponents;

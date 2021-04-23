#include <raymath.h>
#include "Transistors.h"
#if 0
#define ASSERT_VALID_PTR(ptr) _ASSERT_EXPR(!!(ptr), "Passed null parameter(s) \"" #ptr "\".");

bool CanConnectPins(OPin* from, IPin* to)
{
#define PREV_OPEN_TEST !to->prev
#define NOT_IN_NEXT_TEST from->next.find(to) == from->next.end()
#if _DEBUG
	ASSERT_VALID_PTR(from && to);
	bool b_PrevIsOpen = (PREV_OPEN_TEST);
	bool b_NotInNext = (NOT_IN_NEXT_TEST);
	if (b_PrevIsOpen != b_NotInNext) // Connection is not mutual
	{
		bool b_PrevIsFrom = (to->prev == from);
		// "from" is in "to", but "to" is not in "from"
		_ASSERT_EXPR(!(b_PrevIsFrom && b_NotInNext),
					 "\"from\" does not recognise connection to \"to\"");
		// "to" is in "from", but "from" is not in "to"
		_ASSERT_EXPR(!(!b_PrevIsFrom && !b_NotInNext),
					 "\"to\" does not recognise connection to \"from\"");
	}
	return (b_PrevIsOpen && b_NotInNext);
#else
	return ((PREV_OPEN_TEST) && (NOT_IN_NEXT_TEST));
#endif
#undef PREV_OPEN_TEST
#undef NOT_IN_NEXT_TEST
}

void ConnectPins(OPin* from, IPin* to)
{
#if _DEBUG
	ASSERT_VALID_PTR(from && to);
	_ASSERT_EXPR(CanConnectPins(from, to), "Did not confirm safety of connection operation before connecting.");
#endif
	from->next.insert(to);
	to->prev = from;
}

void DisconnectOuputs(OPin* pin)
{
#if _DEBUG
	ASSERT_VALID_PTR(pin);
#endif
	for (IPin* output : pin->next)
	{
		output->prev = nullptr;
	}
	pin->next.clear();
}

void DisconnectInputs(IPin* pin)
{
#if _DEBUG
	ASSERT_VALID_PTR(pin);
	_ASSERT_EXPR(pin->prev, "Prev was nullptr.");
#endif
	for (size_t i = 0; i < pin->prev->next.size(); ++i)
	{
		if (pin->prev->next[i] == pin)
		{
			pin->prev->next.erase(pin->prev->next.begin() + i);
			break;
		}
	}
	pin->prev = nullptr;
}

typedef bool (*BasicTransistor_t)(IPinChipSet* evalSrc);

bool Transistor_OR(IPinChipSet* evalSrc)
{
	return ((*evalSrc)[0].prev->b_state || (*evalSrc)[1].prev->b_state);
}
bool Transistor_NOT(IPinChipSet* evalSrc)
{
	return !(*evalSrc)[0].prev->b_state;
}

void CreateChipInternal(ChipBaseInternal* chip)
{
#if _DEBUG
	_ASSERT_EXPR(!chip->i && !chip->o, "Chip has already been initialized.");
#endif
	chip->i = new ChipBaseInternal::Connection[chip->base->iCount];
	chip->o = new ChipBaseInternal::Connection[chip->base->oCount];
}
void DestroyChipInternal(ChipBaseInternal* chip)
{
	if (chip->i) delete[] chip->i;
	if (chip->o) delete[] chip->o;
}

std::unordered_map<std::string, ChipBase> g_ChipsSrc;

void PackageChipBase(std::string& name, Chip** package, size_t count)
{
	ChipBase& base = g_ChipsSrc[name];
	for (size_t i = 0; i < count; ++i)
	{

	}
}

void CreateChipInstance(Chip* chip, ChipBase*)
{
	// Todo
}
void DestroyChipInstance()
{
	// Todo
}

void EvaluateChip(Chip* chip)
{
	// Todo
}

void TickTransistors(Chip** chips, size_t count)
{
	// Reset
	for (size_t i = 0; i < count; ++i)
	{
		chips[i]->b_dirty = true;
	}
	// Evaluate
	for (size_t i = 0; i < count; ++i)
	{
		if (!chips[i]->b_dirty) continue;
		EvaluateChip(chips[i]);
		chips[i]->b_dirty = false;
	}
}
#endif
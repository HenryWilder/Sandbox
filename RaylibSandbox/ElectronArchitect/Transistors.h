#pragma once
#include <vector>
#include <stack>
#include <set>
#include <unordered_map>
#include <raylib.h>
#if 0
struct OPin;
struct IPin
{
	OPin* prev = nullptr;
};
struct OPin
{
	std::set<IPin*> next;
	bool b_state = false;
};

using IPinChipSet = std::vector<IPin>;
using OPinChipSet = std::vector<OPin>;

struct Pin
{
	enum class Side { Null, In, Out } tag;
	union
	{
		IPin* iPin;
		OPin* oPin;
	};
};


bool CanConnectPins(OPin* from, IPin* to);
void ConnectPins(OPin* from, IPin* to);
void DisconnectOuputs(OPin* pin);
void DisconnectInputs(IPin* pin);

typedef bool (*Transistor_t)(IPinChipSet* evalSrc);

bool Transistor_OR(IPinChipSet* evalSrc);  // Basic transistor OR
bool Transistor_NOT(IPinChipSet* evalSrc); // Basic transistor NOT


struct RelPin
{
	short chipID; // Index of the chip
	short pinID; // Index of the pin on the chip
};
struct IPinRel
{
	RelPin prev;
};
struct OPinRel
{
	std::vector<RelPin> next;
};
struct ChipBase;
struct ChipBaseInternal
{
	ChipBase* base; // Pointer to the base
	std::vector<RelPin> i; // Static size & values
	std::vector<RelPin> o; // Static size & values
};
void CreateChipInternal(ChipBaseInternal* chip);
void DestroyChipInternal(ChipBaseInternal* chip);

// Has set number of inputs and outputs once created
// All internal connections are relative to eachother
struct ChipBase
{
	std::string name;
	std::vector<OPinRel> i; // Relative connections to internals (on the input side)
	std::vector<IPinRel> o; // Relative connections from internals (on the output side)

	enum class Type { Basic, Package } tag;
	union
	{
		Transistor_t bsc; // Basic transistor function
		std::vector<ChipBaseInternal> pkg; // Chip/transistor package
	};
};

extern std::unordered_map<unsigned short, const ChipBase*> g_ChipBases;

struct ChipBaseRef
{
	unsigned short id;
	const ChipBase* operator*()
	{
		auto it = g_ChipBases.find(id);
		if (it != g_ChipBases.end()) return it->second;
		else return nullptr;
	}
};

struct Chip
{
	IPinChipSet i;
	OPinChipSet o;
	ChipBaseRef base;
	Vector2 origin; // Display size & shape deduced from number of inputs/outputs
	bool b_dirty; // Whether we need to re-evaluate
};

void PackageChipBase(std::string& name, Chip** package, size_t count);

void CreateChipInstance(Chip* chip, ChipBase*);
void DestroyChipInstance();

void EvaluateChip(Chip* chip);

void TickTransistors(Chip** chips, size_t count);
#endif
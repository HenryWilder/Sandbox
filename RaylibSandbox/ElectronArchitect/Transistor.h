#pragma once
#include <vector>
#include <variant>

struct Gate;
struct Unit;

using Port = std::variant<
	// Gate
	Gate*,										// Singular gate

	// Unit
	std::pair<
		Unit*,									// Unit with multiple inputs/outputs
		int										// Which port on the Unit
	>

	// Battery TODO
>;

// Effectively a transistor/junction. Variable inputs, logical evaluation, variable outputs.
struct Gate {
	// Logical eval method
	enum class Mode : int {
		OR   = '||',							// Any
		NOR  = '!',								// Invert
		AND  = '&&',							// All
		XOR  = 'pick',							// Exactly 1
	};

private:
	Mode mode;									// How to handle evaluation
	bool state;									// The current state of the transistor

	std::vector<Port> in;						// Vector of all inputs to this transistor
	std::vector<Port> out;						// Vector of all outputs this transistor leads to
};

// Acts as an arithmatic unit or self-contained processor circuit. "Typed" inputs, bitwise/arithmatic evaluation, "Typed" outputs.
struct Unit {
	// Arithmatic/bitwise eval method
	enum class Mode : int {
		OR =  '|',								// Bitwise
		AND = '&',								// Bitwise
		XOR = '^',								// Bitwise
		FLP = '~',								// Bitflip
		LSH = '<<',								// Bitshift left
		RSH = '>>',								// Bitshift right

		LES = '<',								// Less than
		MOR = '>',								// Greater than
		EQU = '==',								// Equal to

		ADD = '+',								// Add
		SUB = '-',								// Subtract
		MUL = '*',								// Multiply
		DIV = '/',								// Divide
		MOD = '%',								// Modulo

		TGL = 'TF',								// T-Flip flop
		RS  = 'RS',								// RS Latch (not gated)
		RSG = 'RSG',							// RS Latch (gated)
	};

	bool GetBit(int at);						// Returns the state of a single bit

	size_t AsNum();								// Returns the bits as a 64-bit integer, regardless of the actual size.

	void SetMode(Mode _mode);					// Sets 

	bool SetLanes(int _lanes);					// Sets the number of lanes in the unit. Returns true on success

private:
	Mode mode;									// How to process the states
	int lanes;									// How many bits in <size>

	std::vector<std::pair<Gate,bool>> state;	// The bits in the component
	std::vector<Port> out;						// Expected to be at most the number of inputs
};
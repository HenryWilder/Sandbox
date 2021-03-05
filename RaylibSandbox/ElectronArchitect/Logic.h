#pragma once
#include <vector>
#include <variant>
#include <array>
#include <unordered_map>

enum class WireShape : char {
	XFirst = 'x',
	YFirst = 'y',

	DiagFirst = 'd',
	DiagLast = 'l',
};
WireShape operator++(WireShape shape);
WireShape operator--(WireShape shape);

struct Component {
	virtual bool IsPolymorphic() = 0;
};

struct Gate;
struct Unit;
struct Batt;

// A connection for routing signals between objects of different types. Specifically gates/units.
struct IOPort {
	explicit operator Gate* ();
	explicit operator Unit* ();

	Component* component;
	Int3 pos;
};
// An IOPort with the ability to read the current state of the port
struct IPort : public IOPort {
	bool GetState();
};
// An IOPort
struct OPort : public IOPort {

};

// Battery/power supply. All circuits must both start & end at a battery.
struct Batt : public Component {
	OPort head;									// Start of a circuit
	IPort tail;									// End of a circuit

	bool IsPolymorphic() override;
};

// Effectively a transistor/junction. Variable inputs, logical evaluation, variable outputs.
struct Gate : public Component {
	// Logical eval method
	enum class Mode : int {
		OR   = '||',							// Any
		NOR  = '!',								// Invert
		AND  = '&&',							// All
		XOR  = 'xor',							// Exactly 1
	};

	bool GetState();							// Returns the current state of the gate (NOTE: does not check if it needs re-evaluation!!)

	// Returns the evaluation of the inputs using the supplied evaluation method.
	static bool Eval(Mode method, std::vector<bool> inputs);

	bool IsPolymorphic() override;

	Gate::Mode mode;							// How to handle evaluation
	bool state;									// The current state of the transistor

	std::vector<IPort> in;						// Vector of all inputs to this transistor
	std::vector<OPort> out;						// Vector of all outputs this transistor leads to
};
Gate::Mode operator++(Gate::Mode m);			// Returns the "next" mode. Returns Mode('err') on failure. NOTE: Will not modify passed var!
Gate::Mode operator--(Gate::Mode m);			// Returns the "last" mode. Returns Mode('err') on failure. NOTE: Will not modify passed var!
char Char(Gate::Mode m, int i);					// Returns the symbol at the supplied position i

// Acts as an arithmatic unit or self-contained processor circuit. "Typed" inputs, bitwise/arithmatic evaluation, "Typed" outputs.
struct Unit : public Component {
	// Arithmatic/bitwise eval method
	enum class Mode : int {
		OR =  '|',								// Bitwise
		AND = '&',								// Bitwise
		XOR = '^',								// Bitwise
		FLP = '~',								// Bitflip (THIS TAKES ONLY 1 PARAMETER, NOT 2)
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

	int LanesIn();								// Returns the number of bits used in the unit
	int LanesOut();								// Returns the number of bits used in the unit

	// Returns the evaluation of a & b using the supplied evaluation method.
	static size_t Eval(Mode method, size_t a, size_t b = 0);

	bool GetBit(int at);						// Returns the state of a single bit
	void SetBit(int at, bool set);				// Returns the state of a single bit

	size_t InputsAsNum();						// Returns the input values as a size_t
	size_t StateAsNum();						// Returns the state bits as a 64-bit integer, regardless of the actual size.
	void SetFromNum(size_t bits);				// Sets the state bits as a 64-bit integer, regardless of the actual size.

	Mode GetMode();								// Returns a copy of the current mode
	void SetMode(Mode _mode);					// Sets the method this unit should use for evaluation
	// Sets the number of lanes in the unit. Updates the mode in the process.
	void SetWidth(int _lanes, Mode _mode = Mode('err'));

	bool IsPolymorphic() override;

	Unit::Mode mode;							// How to process the states
	size_t state;								// The bits in the component

	int lanes_in, lanes_out;					// How many (of the maximum 64) lanes are used for input/output

	std::vector<IPort> in[2];					// Transistors to take the state from
	std::vector<OPort> out;						// Transistors to send the state to
};

Unit::Mode operator++(Unit::Mode m);			// Returns the "next" mode. Returns Mode('err') on failure. NOTE: Will not modify passed var!
Unit::Mode operator--(Unit::Mode m);			// Returns the "last" mode. Returns Mode('err') on failure. NOTE: Will not modify passed var!
char Char(Unit::Mode m, int i);					// Returns the symbol at the supplied position i

int EvaluationDepth(Component* comp);

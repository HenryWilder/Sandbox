#pragma once
#define __INHERITANCE__

/*******************************************
*
*	~Hierarchy~
*
*	Key:
*	{} = polymorphs to
*	() = number of connections
*	-> = input->output
*	n  = vector of things
*	*  = pure virtual
*
*	Port*
*	{
*		Gate         (    2 -> 1    )
*		Node         (    1 -> n    )
*		{
*			FlayNode ( 1(n) -> 1    )
*		}
*		CPort*
*		{
*			CNode    ( 1(n) -> n(n) )
*			Comp     ( 2(n) -> 1    )
*			Unit     ( 2(n) -> 1(n) )
*		}
*	}
*
*******************************************/
#pragma region Pure virtuals

// Base class connection point
// (Node*)(Port*) does NOT do the same thing as dynamic_cast<Node*>(Port*) !!
class Port {
protected:
	// Worldspace location of the Port
	Vector2 position;

public:
	// There is no default constructor. A world position is required.
	Port(Vector2 _pos) : position(_pos) {}

	// Enumeration of all Port child variants
	enum class PortType {
		Node, // One input, numerous outputs
		Gate, // Two inputs, one output
	};
	// Returns what child class varient this is.
	virtual PortType GetType() = 0;

	// Finds the earliest Port* that might have affected the signal in the chain.
	// Nodes do not affect signal, and can be effectively skipped over
	virtual Port* Peephole() = 0;
	// Returns the state of the given bit. (bit is only needed in Cable context)
	virtual bool GetValue(int bit = 0) = 0;

	// Returns the current world position of the Port
	Vector2 GetLocation() { return position; }
	// Sets the current world position of the Port to the provided value
	void SetLocation(Vector2 location) { position = location; }

	// Returns the next Port*.
	virtual Port* Next(size_t path) = 0;
	// Returns the previous Port*.
	virtual Port* Prev(size_t path) = 0;

	// Returns the Port* which got overwritten to make the push possible. Returns nullptr if push succeeded without overwrite
	virtual Port* Push_I(Port* input) = 0;
	// Returns the Port* which got overwritten to make the push possible. Returns nullptr if push succeeded without overwrite
	virtual Port* Push_O(Port* output) = 0;

	// Erase what from the input(s), if it is there. Does not remove self from their output list.
	virtual void _DRef_I(Port* what) = 0;
	// Erase what from the output(s), if it is there. Does not remove self from their input list.
	virtual void _DRef_O(Port* what) = 0;

	// Erase what from the input(s), if it is there. Returns true on success.
	virtual bool Erase_I(Port* what) = 0;
	// Erase what from the output(s), if it is there. Returns true on success.
	virtual bool Erase_O(Port* what) = 0;

	// Returns the number of inputs currently accessible
	virtual size_t I_Size() = 0;
	// Returns the number of outputs currently accessible
	virtual size_t O_Size() = 0;

	// Returns the number of inputs that could be used
	virtual size_t I_Capacity() = 0;
	// Returns the number of outputs that could be used
	virtual size_t O_Capacity() = 0;

	virtual std::vector<Port*> GetInputs() = 0;
	virtual std::vector<Port*> GetOutputs() = 0;
};

// Structure containing either a cable, or a vector of ports
// Calls the associated functions depending on which one it is
struct Bundle {
	Bundle(std::vector<Port*> _bundle) {
		tag = MultiWire;
		bundle = _bundle;
	}
	Bundle(CPort* _cable) {
		tag = MonoCable;
		bundle = {};
		cable = _cable;
	}

	enum { MultiWire, MonoCable } tag;
	union { nullptr_t null;  std::vector<Port*> bundle; CPort* cable; };

	void Resize(size_t newSize) {
		switch (tag) {
		case MultiWire: bundle.resize(newSize); break;
		case MonoCable: cable->Resize(newSize); break;
		}
	}
	size_t Size() {
		switch (tag) {
		case MultiWire: return bundle.size();
		case MonoCable: return cable->Size();
		}
	}
};
// Base class cable connection point
class CPort : public Port {
protected:
	size_t width;

public:
	CPort(Vector2 _pos) : Port(_pos), width() {}
	CPort(Vector2 _pos, size_t _width) : Port(_pos), width(_width) {}

	virtual void Resize(size_t _width) = 0;
	virtual size_t Size() = 0;
};

#pragma endregion
#pragma region Nodes

// Nodes can split, but not combine.
// Nodes are more akin to repeaters and do not affect the signal.
class Node : public Port {
protected:
	Port* i;
	std::vector<Port*> o;

public:
	friend PortList;

	Node(Vector2 _pos) : Port(_pos), i(nullptr), o({}) {}

	PortType GetType() override { return PortType::Node; }

	Port* Peephole() override { return (i ? i->Peephole() : nullptr); }

	bool GetValue(int bit = 0) override {
		Port* peek = Peephole(); // Find the first port who might actually affect the signal
		return (peek ? peek->GetValue() : false); // Node is start of circuit
	}

	Port* Next(size_t path) override { return o[path]; }
	Port* Prev(size_t path = 0) override { return i; }

	Port* Push_I(Port* input) override {
		Port* _i = i;
		i = input;
		if (_i) _i->Erase_O(this);
		return _i;
	}
	Port* Push_O(Port* output) override {
		o.push_back(output);
		return nullptr;
	}

	void _DRef_I(Port* what) override {
		if (i == what) i = nullptr;
	}
	void _DRef_O(Port* what) override {
		size_t i = 0;
		for (Port* port : o) {
			if (port == what) {
				o.erase(o.begin() + i);
				break;
			}
			++i;
		}
	}

	// Erase "what", if it is the input.
	// Returns true if the erasure was successful or input is already free.
	// Returns false if "what" was not the input.
	bool Erase_I(Port* what) override {
		if (i == what) {
			i = nullptr;
			what->_DRef_O(this);
			return true;
		}
		return false;
	}
	// Erase "what", if it is an output.
	// Returns true if the erasure was successful.
	// Returns false if "what" was not an output.
	bool Erase_O(Port* what) override {
		size_t i = 0;
		for (Port* port : o) {
			if (port == what) {
				o.erase(o.begin() + i);
				what->_DRef_I(this);
				return true;
			}
			++i;
		}
		return false;
	}

	size_t I_Size() override { return (i ? 1ull : 0ull); }
	size_t O_Size() override { return o.size(); }

	size_t I_Capacity() override { return 1ull; }
	size_t O_Capacity() override { return SIZE_MAX; }

	std::vector<Port*> GetInputs() override { return { i }; }
	std::vector<Port*> GetOutputs() override { return o; }
};

class FlayNode : public Node {
private:
	size_t index;

public:
	FlayNode(Vector2 _pos, size_t _index) : Node(_pos), index(_index) {}

	bool GetValue(int bit = 0) override { return i && i->GetValue(index); }
};

void Flay(Port* node) {
	if (node = dynamic_cast<Node*>(node)) {
		node = new
	}
}

// CableNodes can split, but not combine.
// CableNodes are more akin to repeaters and do not affect the signal.
// Unlike Nodes, CableNodes take in multiple inputs and convert them into a bundled signal.
// Nodes should automatically become CableNodes when given additional inputs.
// A CNode with 1 input or fewer should automatically become a regular Node again.
class CNode : public CPort {
protected:
	Bundle i;
	std::vector<Bundle> o;

public:
	friend PortList;

	CNode(Vector2 _pos) : CPort(_pos), i(), o({}) {}
	CNode(Vector2 _pos, int _width) : CPort(_pos), i(), o({}) {
		i.Resize(_width);
	}

	PortType GetType() override { return PortType::Node; }

	Port* Peephole() override { return (i ? i->Peephole() : nullptr); }

	bool GetValue(int bit = 0) override {
		Port* peek = Peephole(); // Find the first port who might actually affect the signal
		return (peek ? peek->GetValue() : false); // Node is start of circuit
	}

	Port* Next(size_t path) override { return o[path]; }
	Port* Prev(size_t path = 0) override { return i; }

	Port* Push_I(Port* input) override {
		Port* _i = i;
		i = input;
		if (_i) _i->Erase_O(this);
		return _i;
	}
	Port* Push_O(Port* output) override {
		o.push_back(output);
		return nullptr;
	}

	void _DRef_I(Port* what) override {
		if (i == what) i = nullptr;
	}
	void _DRef_O(Port* what) override {
		size_t i = 0;
		for (Port* port : o) {
			if (port == what) {
				o.erase(o.begin() + i);
				break;
			}
			++i;
		}
	}

	// Erase "what", if it is the input.
	// Returns true if the erasure was successful or input is already free.
	// Returns false if "what" was not the input.
	bool Erase_I(Port* what) override {
		if (i == what) {
			i = nullptr;
			what->_DRef_O(this);
			return true;
		}
		return false;
	}
	// Erase "what", if it is an output.
	// Returns true if the erasure was successful.
	// Returns false if "what" was not an output.
	bool Erase_O(Port* what) override {
		size_t i = 0;
		for (Port* port : o) {
			if (port == what) {
				o.erase(o.begin() + i);
				what->_DRef_I(this);
				return true;
			}
			++i;
		}
		return false;
	}

	size_t I_Size() override { return (i ? 1ull : 0ull); }
	size_t O_Size() override { return o.size(); }

	size_t I_Capacity() override { return 1ull; }
	size_t O_Capacity() override { return SIZE_MAX; }

	std::vector<Port*> GetInputs() override { return { i }; }
	std::vector<Port*> GetOutputs() override { return o; }
};

#pragma endregion
#pragma region Evaluators

// Gates can combine (exactly 2), but not split.
// Gates will almost certainly affect the signal
class Gate : public Port {
public:
	enum class Method : char { OR = '|', NOT = '!', AND = '&', XOR = '^' };

private:
	struct { Port* a; Port* b; } i;
	Port* o;
	bool b_evaluated, b_state;
	Method evm; // Evaluation Method

public:
	friend PortList;

	Gate(Vector2 _pos) : Port(_pos), i({ nullptr, nullptr }), o(nullptr), b_evaluated(false), b_state(false), evm(Method::OR) {}

	void SetMethod(Method evalMethod) { evm = evalMethod; }
	Method GetMethod() { return evm; }

	PortType GetType() override final { return PortType::Gate; }

	bool Evaluate() {
		bool a = (i.a ? i.a->GetValue() : false);
		bool b = (i.b ? i.b->GetValue() : false);
		switch (evm) {
		default:
		case Method::OR:  return (a || b);
		case Method::AND: return (a && b);
		case Method::NOT: return !(a || b);
		case Method::XOR: return (a ? !b : b);
		}
	}

	Port* Peephole() override { return this; }

	bool GetValue(int bit) override final {
		if (!b_evaluated) {
			b_state = Evaluate();
			b_evaluated = true;
		}
		return b_state;
	}
	void FrameEndReset() { b_evaluated = false; }

	Port* Next(size_t path) override { return o; }
	Port* Prev(size_t path) override { return (path ? i.b : i.a); }

	Port* Push_I(Port* input) override {
		if (i.a) {
			Port* _b = i.b;
			i.b = input;
			if (_b) _b->Erase_O(this);
			return _b;
		}
		else i.a = input;
		return nullptr;
	}
	Port* Push_O(Port* output) override {
		Port* _o = o;
		o = output;
		if (_o) _o->Erase_I(this);
		return _o;
	}

	void _DRef_I(Port* what) override {
		if (i.a == what) i.a = nullptr;
		if (i.b == what) i.b = nullptr;
	}
	void _DRef_O(Port* what) override {
		if (o == what) o = nullptr;
	}

	// Erase "what", if it is an input.
	// Returns true if the erasure was successful.
	// Returns false if "what" was not an input.
	bool Erase_I(Port* what) override {
		if (i.a == what) { i.a = nullptr; what->_DRef_O(this); return true; }
		if (i.b == what) { i.b = nullptr; what->_DRef_O(this); return true; }
		return false;
	}
	// Erase "what", if it is the output.
	// Returns true if the erasure was successful or if output is already free.
	// Returns false if "what" was not the output.
	bool Erase_O(Port* what) override {
		if (o == what) { o = nullptr; what->_DRef_I(this); return true; }
		return false;
	}

	size_t I_Size() override { return ((i.a ? 1ull : 0ull) + (i.b ? 1ull : 0ull)); }
	size_t O_Size() override { return (o ? 1ull : 0ull); }

	size_t I_Capacity() override { return 2ull; }
	size_t O_Capacity() override { return 1ull; }

	std::vector<Port*> GetInputs() override { return { i.a, i.b }; }
	std::vector<Port*> GetOutputs() override { return { o }; }
};

class Comp : public CPort {
public:
	enum class Method : char { GreaterThan = '>', LessThan = '<', Equals = '=', Not = '!' };

private:
	struct { Bundle a;  Bundle b; } i;
	Port* o;
};

class Unit : public CPort {
public:
	enum class Method : char {
		B_OR = '|', B_FLIP = '~', B_AND = '&', B_XOR = '^', B_SLEFT = '<', B_SRIGHT = '>',
		A_ADD = '+', A_SUB = '-', A_MULT = '*', A_DIV = '/', A_MOD = '%'
	};

private:
	struct { Bundle a;  Bundle b; } i;
	Bundle o;
};

#pragma endregion
#pragma endregion
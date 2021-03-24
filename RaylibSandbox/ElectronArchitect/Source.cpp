#include <vector>
#include <deque>
#include <queue>
#include <set>
#include <unordered_set>
#include <list>
#include <stack>
#include <bitset>
#include <unordered_map>
#include <raylib.h>
#include <raymath.h>

#pragma region Composits

class Object;



// Connects to one or more Objects
__interface ConnectionDelegate
{
public:
	virtual size_t Find(Object* what) = 0;			// Searches for the connection in the connector
	virtual bool Valid(size_t index) = 0;			// Tells whether the index is a valid connection id

	virtual Object* GetNode(size_t index) = 0;		// Returns a copy of the connection

	virtual void _UnRef(size_t index) = 0;			// Remove the connection without reference cleaning
	virtual void _UnRef(Object* what) = 0;			// Remove the connection without reference cleaning
	virtual void Push(Object* what) = 0;			// Add a connection--erases existing connection if needed
	virtual void Erase(Object* what) = 0;			// Remove the connection; cleans reference to self

	virtual size_t Width() = 0;						// The throughput capacity of the connection
	virtual size_t Size() = 0;						// How many throughpoints are currently valid
};

// Mono-width connector
class Wire : public ConnectionDelegate
{
private:
	Object* connection;

public:
	size_t Find(Object* what) override
	{
		if (connection == what)
		{
			return 0ull;
		}
	}
	bool Valid(size_t index) override
	{
		return (index == 0);
	}

	Object* GetNode(size_t index) override
	{
		return connection;
	}

	void _UnRef(size_t index) override
	{
		if (index == 0)
		{
			connection = nullptr;
		}
	}
	void _UnRef(Object* what) override
	{
		if (connection == what)
		{
			connection = nullptr;
		}
	}
	void Push(Object* what) override
	{
		if (connection)
		{
			what->_UnRef(this); // TODO
		}
		connection = what;
	}
	void Erase(Object* what) override
	{
		if (connection == what)
		{
			what->_UnRef(this); // TODO
			connection = nullptr;
		}
	}

	size_t Width() override
	{
		return 1ull;
	}
	size_t Size() override
	{
		return (connection ? 1ull : 0ull);
	}
};

// Variable-width connector
class Cable : public ConnectionDelegate
{
private:
	std::vector<Object*> connection;

public:
	size_t Find(Object* what) override
	{
		for (size_t i = 0ull; i < connection.size(); ++i)
		{
			if (connection[i] == what)
			{
				return i;
			}
		}
		return connection.size();
	}
	bool Valid(size_t index) override
	{
		return (index < connection.size());
	}

	std::vector<Object*> Get() override
	{
		return connection;
	}

	void _UnRef(size_t index) override
	{
		connection.erase(connection.begin() + index);
	}
	void _UnRef(Object* what) override
	{
		_UnRef(Find(what));
	}
	void Push(Object* what) override
	{
		connection.push_back(what);
	}
	void Erase(Object* what) override
	{
		size_t at = Find(what);
		if (Valid(at))
		{
			connection.erase(connection.begin() + at);
		}
	}

	size_t Width() override
	{
		return connection.size();
	}
	size_t Size() override
	{
		return connection.size();
	}
};



// Connects to one or more Objects by means of one or more ConnectionDelegate
__interface IODelegate
{
public:
	virtual std::vector<ConnectionDelegate*> GetConnector() = 0;

	virtual void _UnRef(ConnectionDelegate* what) = 0;
	virtual void Push(ConnectionDelegate* what) = 0;
	virtual void Erase(ConnectionDelegate* what) = 0;

	virtual size_t Size() = 0;
	virtual size_t Capacity() = 0;
};

// Exactly 1 conection
class IO_1 : public IODelegate
{
private:
	ConnectionDelegate* io;

public:
	std::vector<ConnectionDelegate*> Get()
	{
		return { io };
	}
};

// Exactly 2 connections of equal width
class IO_2 : public IODelegate
{
private:
	struct { ConnectionDelegate* a; ConnectionDelegate* b; } io;

public:
	std::vector<ConnectionDelegate*> Get()
	{
		return { io.a, io.b };
	}
};

// Various connections of equal width
class IO_n : public IODelegate
{
private:
	std::vector<ConnectionDelegate*> io;

public:
	std::vector<ConnectionDelegate*> Get()
	{
		return io;
	}
};



__interface EvaluationDelegate
{
public:
	virtual void Evaluate() = 0;									// Updates the evaluation for this frame
	virtual bool GetValue(size_t wire = 0, size_t bit = 0) = 0;		// Returns the value of the state at the index
	virtual void EvalReset() = 0;									// End of frame reset so next frame's evaluations can be performed
};
class NonEvaluating : public EvaluationDelegate
{
private:
	IODelegate* input;

public:
	NonEvaluating(IODelegate* _input) : input(_input) {}

	void Evaluate() {}
	bool GetValue(size_t wire = 0, size_t bit = 0)
	{
		input->Get()[wire]->Get()[wire]->GetValue(wire, bit);
	}
	void EvalReset() {}
};
class Logical : public EvaluationDelegate
{

};
class Comparative : public EvaluationDelegate
{

};
class Bitwise : public EvaluationDelegate
{

};
class Arithmetic : public EvaluationDelegate
{

};

// 
class Object
{
private:
	Vector2 pos;
	IODelegate* i;
	IODelegate* o;
	EvaluationDelegate* e;

public:
	enum class Type
	{
		Node, // Repeater
		Gate, // Logical
		Comp, // Comparative
		Bitw, // Bitwise
		Math, // Arithmetic
	};

	void Init(Type as)
	{
		switch (as)
		{
		case Type::Node:
			i = new IO_1();
			o = new IO_n();
			e = new NonEvaluating(i);
			break;

		case Type::Gate:
			i = new IO_2();
			o = new IO_1();
			e = new Logical(i);
			break;

		case Type::Comp:
			i = new IO_2();
			o = new IO_1();
			e = new Comparative(i);
			break;

		case Type::Bitw:
			i = new IO_2();
			o = new IO_1();
			e = new Bitwise(i);
			break;

		case Type::Math:
			i = new IO_2();
			o = new IO_1();
			e = new Arithmetic(i);
			break;
		}
	}
	void Free()
	{
		delete i;
		delete o;
		delete e;
	}
	void Make(Type into)
	{
		Free();
		Init(into);
	}

	Object(Vector2 _pos, Type _type) : pos(_pos) { Init(_type); }
	~Object() { Free(); }

	// IODelegate

	std::vector<ConnectionDelegate*> GetInputs()
	{
		return i->Get();
	}
	std::vector<ConnectionDelegate*> GetOutputs()
	{
		return o->Get();
	}

	// EvaluationDelegate

	void Evaluate()
	{
		e->Evaluate();
	}
	bool GetValue(size_t wire = 0ull, size_t bit = 0ull)
	{
		return e->GetValue(bit);
	}
	void EvalReset()
	{
		e->EvalReset();
	}
};

#pragma endregion

#pragma region Objects
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

#pragma region Containers

std::unordered_map<Port*, char> g_namedPtrs;

class PortList {
private:
	std::vector<Port*> ports;

public:
	auto begin() { return ports.begin(); }
	auto end() { return ports.end(); }

	Port* Get(size_t i) { return ports[i]; }

#if 0
	std::string ConstructTreeStr() {
		g_namedPtrs[nullptr] = '-';
		auto PortName = [](Port* ptr) {
			auto it = g_namedPtrs.find(ptr);
			if (it == g_namedPtrs.end()) {
				char name = (char)g_namedPtrs.size() - 1;
				switch (ptr->GetType())
				{
				case Port::PortType::Gate: name += 'A'; break;	// Gates are capital
				case Port::PortType::Node: name += 'a'; break;	// Nodes are lowercase
				default: name += '0'; break;					// Unknown are numbers
				}
				g_namedPtrs.insert({ ptr, name });
				it = g_namedPtrs.find(ptr);
			}
			return it->second;
		};
		std::string str;
		for (Port* port : ports)
		{
			std::string str2;
			switch (port->GetType())
			{
			case Port::PortType::Gate: {
				Gate* gate = (Gate*)port;
				str2 = TextFormat(R"(
		(Gate)
		InputA: %c,
		InputB: %c,
		Output: %c,)", PortName(gate->i.a), PortName(gate->i.b), PortName(gate->o));

			}	break;
			case Port::PortType::Node: {
				Node* node = (Node*)port;

				std::string str3;
				for (Port* elem : node->o) {
					str3 += TextFormat(" %c,", PortName(elem));
				}
				if (str3.empty()) str3 = " - ";
				else str3 += " ";

				str2 = TextFormat(R"(
		(Node)
		Input: %c,
		Outputs: {%s})", PortName(node->i), str3.c_str());
			}	break;
			}
			
			str += TextFormat("\n\t%c{%s\n\t}", PortName(port), (str2.empty() ? "\0" : str2.c_str()));
		}
		return (str.empty() ? " - " : str + "\n");
	};
#endif

	// ONLY call this if it's certain "what" has no external references!!
	// If "what" has the possibility of external references, please call Wipe() instead!
	void Erase(size_t index) { ports.erase(ports.begin() + index); }
	// ONLY call this if it's certain "what" has no external references!!
	// If "what" has the possibility of external references, please call Wipe() instead!
	void Erase(Port* what) {
		size_t i = 0;
		for (; i < ports.size(); ++i) { if (ports[i] == what) break; }
		Erase(i);
	}

	void Push(Port* port) { ports.push_back(port); }
	void Reserve(size_t reservation) { if ((ports.size() + reservation) > ports.capacity()) ports.reserve(ports.size() + reservation); }

	size_t Size() { return ports.size(); }

	Port* FindAtPos(Vector2 at) {
		for (Port* port : ports) {
			if (port && Vector2Distance(at, port->GetLocation()) < 1.0f)
				return port;
		}
		return nullptr;
	}
	Port* FindAtPos(Vector2 at, Port* avoid) {
		for (Port* port : ports) {
			if ((port) && (Vector2Distance(at, port->GetLocation()) < 1.0f) && (port != avoid))
				return port;
		}
		return nullptr;
	}
	Port* FindAtPos(Vector2 at, std::unordered_set<Port*>& avoid) {
		for (Port* port : ports) {
			if ((port) && (Vector2Distance(at, port->GetLocation()) < 1.0f) && (avoid.find(port) == avoid.end()))
				return port;
		}
		return nullptr;
	}
	Port* FindAtPos(Vector2 at, std::vector<Port*>& avoid) {
		// If it would be computationally cheeper to do so, convert the vector to an unordered_set so we can have O(c) find() operations.
		if ((avoid.size() > 10)) { // Must be trying to avoid more than 10 elements
			std::unordered_set<Port*> avoidSet;
			for (Port* port : avoid) { avoidSet.insert(port); }
			return FindAtPos(at, avoidSet);
		}
		else {
			auto VectorContains = [&avoid](Port* target) {
				for (Port* port : avoid) {
					if (port == target) return true;
				}
				return false;
			};
			for (Port* port : ports) {
				if ((port) && (Vector2Distance(at, port->GetLocation()) < 0.5f) && (!VectorContains(port)))
					return port;
			}
			return nullptr;
		}
	}
#if 0
#define NULLIFY_IF_FOUND(var) if (hitlist.find(var) != hitlist.end()) var = nullptr // If the passed variable has a value on the no-no list, euthanize it.
	void Wipe(std::unordered_set<Port*>& hitlist) { // unordered_set has constant-time (very fast) find(), making it perfect for "is this a wanted guy?"
		std::stack<size_t> srcs; // Stack of indices to the actual Port* objects in the vector (FIFO so that indices don't get invalidated while erasing)
		size_t i = 0ull; // Stores index so we can do a range-based for loop :>
		for (Port* port : ports) {
			if (hitlist.find(port) != hitlist.end()) srcs.push(i); // That one right there, officer.
			else { // This guy's gonna be gone anyway. Don't bother wasting the operations.
				switch (port->GetType()) // The different varients of Port* have to be handled differently
				{
				case Port::PortType::Gate: { // Port* is of the Gate class
					Gate* gate = (Gate*)port; // We are safe not to dynamic_cast here as we have already confirmed the type
					NULLIFY_IF_FOUND(gate->i.a);
					NULLIFY_IF_FOUND(gate->i.b);
					NULLIFY_IF_FOUND(gate->o);
				} break;

				case Port::PortType::Node: { // Port* is of the Node class
					Node* node = (Node*)port; // We are safe not to dynamic_cast here as we have already confirmed the type
					NULLIFY_IF_FOUND(node->i);
					std::stack<size_t> badOuts; // Stack of indices of outputs on the no-no list (FIFO so that indices don't get invalidated while erasing)
					for (size_t o = 0; o < node->O_Size(); ++o) {
						if (hitlist.find(node->o[o]) != hitlist.end()) badOuts.push(o); // If the element at the index is bad, put it on the badOuts stack
					}
					while (!badOuts.empty()) { // For as long as there are outputs needing to be gone'd,
						node->o.erase(node->o.begin() + badOuts.top()); // Erase the index of the top bad output
						badOuts.pop(); // Whatever is on the top of the badOuts stack is no longer useful. Stop storing it.
					}
				} break;
				}
			}
			++i; // Increment the index so it's up to date
		}
		while (!srcs.empty()) { // For as long as there are elements needing to be gone'd,
			ports.erase(ports.begin() + srcs.top()); // Erase the index of the top source
			srcs.pop(); // Whatever is on the top of the srcs stack is no longer useful. Stop storing it.
		}
	}
#undef NULLIFY_IF_FOUND // Undefine so this name doesn't leak

#define REPLACE(var) if (var == target) var = with
	void Replace(Port* target, Port* with) {
		size_t targetIndex = ports.size();
		size_t i = 0;
		bool b_withAlreadyExists = false;
		for (Port* port : ports) {
			if (port == with) b_withAlreadyExists = true;
			else if (port == target) targetIndex = i;
			else {
				switch (port->GetType())
				{
				case Port::PortType::Gate: {
					Gate* gate = (Gate*)port;
					REPLACE(gate->i.a);
					REPLACE(gate->i.b);
					REPLACE(gate->o);
				} break;
				case Port::PortType::Node: {
					Node* node = (Node*)port;
					REPLACE(node->i);
					for (Port*& o : node->o) {
						REPLACE(o);
					}
				} break;
				}
			}
			++i;
		}
		if (targetIndex < ports.size()) {
			if (b_withAlreadyExists) ports[targetIndex] = with;
			else ports.erase(ports.begin() + targetIndex);
		}
	}
#undef REPLACE // Undefine so this name doesn't leak
#endif
};
PortList g_allPorts;

// Creates a Node with the same information as the passed port (frees port internally!)
Node* ToNode(Port* port)
{
	if (port->GetType() == Port::PortType::Node) return (Node*)port; // Already is node

	Node* node = new Node(port->GetLocation());
	std::vector<Port*> inputs  = port->GetInputs();
	std::vector<Port*> outputs = port->GetOutputs();

	switch (port->GetType())
	{
	case Port::PortType::Gate: { // Turning a Gate into a Node
		// 2 inputs -> 1 input
		if (inputs[0]) {
			node->Push_I(inputs[0]);
			inputs[0]->Erase_O(port);
			inputs[0]->Push_O(node);
		}
		if (inputs[1]) inputs[1]->Erase_O(port); // Tell all the ex-input that they no longer have this port as an output

		// 1 output -> 1 output(n)
		if (outputs[0]) {
			node->Push_O(outputs[0]);
			outputs[0]->Erase_I(port);
			outputs[0]->Push_I(node);
		}
	}	break;
	}
	g_allPorts.Erase(port);
	delete port;
	g_allPorts.Push(node);
	return node;
}
// Creates a Gate with the same information as the passed port (frees port internally!)
Gate* ToGate(Port* port)
{
	if (port->GetType() == Port::PortType::Gate) return (Gate*)port; // Already is gate

	Gate* gate = new Gate(port->GetLocation());
	std::vector<Port*> inputs  = port->GetInputs();
	std::vector<Port*> outputs = port->GetOutputs();

	switch (port->GetType())
	{
	case Port::PortType::Node: { // Turning a Node into a Gate
		// 1 input -> 1 input
		if (inputs[0]) {
			gate->Push_I(inputs[0]);
			inputs[0]->Erase_O(port);
			inputs[0]->Push_O(gate);
		}

		// n outputs -> 1 output
		if (!outputs.empty()) {
			for (Port* out : outputs) {
				out->Erase_I(port); // Tell all the ex-outputs that they no longer have this port as an input
			}
			gate->Push_O(outputs[0]);
			outputs[0]->Push_I(gate);
		}

	}	break;
	}
	g_allPorts.Erase(port);
	delete port;
	g_allPorts.Push(gate);
	return gate;
}

struct ColorScheme {
	ColorScheme() : background(), grid(), selection(), icons() {}
	ColorScheme(Color _background, Color _grid, Color _selection, Color _icons) : background(_background), grid(_grid), selection(_selection), icons(_icons) {}

	Color background;
	Color grid;
	Color selection;
	Color icons;
};

#pragma endregion

#pragma region Other

bool Positive(float value) { return value >= 0.0f; }
float Sign(float value) {
	if (value > 0.0f) return 1.0f;
	if (value < 0.0f) return -1.0f;
	return 0.0f;
}
float Roundf(float value) { return (float)(int)(value + (0.5f * Sign(value))); }
int Round(float value) { return (int)(value + (0.5f * Sign(value))); }
Vector2 Roundf(Vector2 vec) { return { Roundf(vec.x), Roundf(vec.y) }; }

float Shift(float value, int shift) {
	if (shift == 0) return value;
	if (value > 1.0f) {
		if (shift > 0) return (float)(Round(value) <<  shift);
		if (shift < 0) return (float)(Round(value) >> -shift);
	}
	else {
		if (shift > 0) return (value * (1 << shift));
		if (shift < 0) return (value / (1 << -shift));
	}
	return value;
}
float Shift(float value, float shift) { return Shift(value, Round(shift)); }

// Returns end but moved to the best place route I decided
Vector2 QueenRules(Vector2 start, Vector2 end) {
	start = Roundf(start);
	end = Roundf(end);
	Vector2 dist = Vector2Subtract(end, start);
	float* shorter;
	float* longer;
	// Wider than tall
	if (abs(dist.x) > abs(dist.y)) {
		longer = &dist.x;
		shorter = &dist.y;
	}
	// Taller than wide
	else {
		longer = &dist.y;
		shorter = &dist.x;
	}
	if (abs(*shorter) > (abs(*longer) * 0.5f)) { // Closer to diagonal
		if (Positive(*longer) != Positive(*shorter)) *longer = -(*shorter);
		else *longer = *shorter;
	}
	else { // Closer to cardinal
		*shorter = 0.0f;
	}
	return Vector2Add(start, dist);
}

Shader g_shader_wire;
int g_uniform_wireActive;
Shader g_shader_cable;
Texture g_defaultTex;

void DrawCord(Vector2 start, Vector2 end, Color color) {
	DrawLineV(start, end, color);
}
void DrawCordEx(Vector2 start, Vector2 end, Color color, float thickness) {
	Rectangle src; {
		src.x = start.x;
		src.y = start.y;
		src.width = Vector2Distance(start, end);
		src.height = thickness;
	}
	float rot = Vector2Angle(start, end);

	DrawCircleV(start, thickness * 0.5f, color);
	DrawCircleV(end, thickness * 0.5f, color);

	DrawTexturePro(g_defaultTex, {0,0,1,1}, src, { 0.0f, 0.5f * src.height }, rot, WHITE);
}

#pragma endregion

//  __  __       _
// |  \/  |     (_)
// | \  / | __ _ _ _ __
// | |\/| |/ _` | | '_ \
// | |  | | (_| | | | | |
// |_|  |_|\__,_|_|_| |_|

#pragma region Main

int main() {
	int windowWidth = 1280;
	int windowHeight = 720;

	InitWindow(windowWidth, windowHeight, "Electron Architect");
	SetTargetFPS(30);

	g_shader_wire = LoadShader(0, "wire.frag");
	g_uniform_wireActive = GetShaderLocation(g_shader_wire, "b_active");
	int uniform_wireTime = GetShaderLocation(g_shader_wire, "time");
	int uniform_wireRes = GetShaderLocation(g_shader_wire, "resolution");
	{
		float resolution[2] = { (float)windowWidth, (float)windowHeight };
		SetShaderValue(g_shader_wire, uniform_wireRes, resolution, UNIFORM_VEC2);
	}
	g_shader_cable = LoadShader(0, "cable.frag");

	Image img = GenImageColor(1,1,WHITE);
	g_defaultTex = LoadTextureFromImage(img); // A 1x1 pixel texture of just white; used for drawing rectangles with shaders applied (DrawRectangle command doesn't give fragTexCoord)
	UnloadImage(img);

	// Array of textures for drawing gates
	Texture gateIcons[] = {
		LoadTexture("Gate_L-AND.png"),
		LoadTexture("Gate_L-NOT.png"),
		LoadTexture("Gate_L-OR.png"),
		LoadTexture("Gate_L-XOR.png"),
	};
	// Draws the icon for a gate of the chosen evaluation method (AND, OR, NOT, XOR) centered at the position, scaled by world units
	auto DrawGateIcon = [&gateIcons](Gate::Method m, Vector2 pt, float scale, Color tint) {
		int iconIndex;
		switch (m)
		{
		case Gate::Method::AND:
			iconIndex = 0;
			break;
		case Gate::Method::NOT:
			iconIndex = 1;
			break;
		case Gate::Method::OR:
			iconIndex = 2;
			break;
		case Gate::Method::XOR:
			iconIndex = 3;
			break;
		default:
			break;
		}
		Texture& tex = gateIcons[iconIndex];
		float halfScale = scale * 0.5f;
		DrawTexturePro(tex, { 0, 0, (float)tex.width, (float)tex.height }, { pt.x - halfScale, pt.y - halfScale, scale, scale }, { 0.0f, 0.0f }, 0.0f, tint);
	};

	Camera2D camera; {
		camera.offset = { };
		camera.target = { };
		camera.rotation = 0.0f;
		camera.zoom = 8.0f;
	}
	// Used for updating the position of the camera when zooming
	Vector2 cameraPos = {
		(float)windowWidth * 0.5f,
		(float)windowHeight * 0.5f
	};

	Vector2 mousePos, worldMousePos, mousePos_last, worldMousePos_last;
	mousePos = { 0,0 };
	worldMousePos = { 0,0 };

	Port* startPort = nullptr;	// The port which was hovered when drawing began (resets when m1 is released)
	Vector2 startPos{};			// Where the mouse was when drawing was started
	Vector2 endPos{};			// Where the mouse was when drawing was finished
	Rectangle collision;		// For selection

	// The worldspace position & size of the top-left screen pixel
	Rectangle worldPixel;
	// Worldspace rectangle from screen 0 to screen width/height
	Rectangle worldScreen;
	// Update worldPixel & worldScreen to reflect possible camera changes
	auto UpdateWorldPixel = [&] {
		Vector2 pos = GetScreenToWorld2D({ 0, 0 }, camera);
		Vector2 width = Vector2Subtract(GetScreenToWorld2D({ 1, 1 }, camera), pos);

		worldPixel.x = worldScreen.x = pos.x;
		worldPixel.y = worldScreen.y = pos.y;
		worldPixel.width  = width.x;
		worldPixel.height = width.y;

		width = Vector2Subtract(GetScreenToWorld2D({ (float)windowWidth, (float)windowHeight }, camera), pos);
		worldScreen.width  = width.x;
		worldScreen.height = width.y;
	};

	std::unordered_set<Port*> selection;
	Port* hovered = nullptr;

	enum class InputMode { Draw, Select, Edit } mode = InputMode::Draw;
	Port::PortType make = Port::PortType::Node;

	float time = 0.0f;

	while (!WindowShouldClose()) {
#pragma region Simulate

		time = (float)GetTime();
		SetShaderValue(g_shader_wire, uniform_wireTime, &time, UNIFORM_FLOAT);

		if (GetMouseWheelMove() != 0.0f) {
			float zoom = Shift(camera.zoom, GetMouseWheelMove());
			if (zoom >= 1.0f) {
				if (zoom < 127.0f) camera.zoom = zoom;
				else camera.zoom = 127.0f;
			}
			else camera.zoom = 1.0f;
			cameraPos = Vector2Scale(Roundf(Vector2Scale(cameraPos, 1.0f / camera.zoom)), camera.zoom); // Update camera position now that zoom has been changed
		}

		mousePos_last = mousePos;
		mousePos = GetMousePosition();

		worldMousePos_last = worldMousePos;
		worldMousePos = GetScreenToWorld2D(mousePos, camera);

		hovered = g_allPorts.FindAtPos(worldMousePos);

		switch (mode)
		{
		default:
		case InputMode::Draw: {
			// Middle mouse drag
			// Pan
			if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
				Vector2 mouse_delta = Vector2Subtract(mousePos, mousePos_last);
				Vector2 worldMouse_delta = Vector2Subtract(worldMousePos, worldMousePos_last);

				cameraPos = Vector2Scale(Roundf(Vector2Scale(Vector2Add(cameraPos, mouse_delta), 1.0f / camera.zoom)), camera.zoom);
			}
			// M1 Press
			// Start wire
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				startPort = hovered;
				if (startPort) startPos = startPort->GetLocation();
				else startPos = Roundf(worldMousePos);
			}
			// M1 Dragging
			// Update endpoint position
			if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
				endPos = Roundf(QueenRules(startPos, worldMousePos));
			}
			// M1 Release
			// Connect wire
			if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
				// If the start and end points aren't within a unit of each other
				if (Vector2Distance(endPos, startPos) > 0.9)
				{
			#define BIT_START 0
			#define BIT_END 1

					std::bitset<2> neededAllocs;

					Port* start = (neededAllocs.set(BIT_START, !startPort), (neededAllocs.test(BIT_START) ? new Node(startPos) : startPort));
					Port* end = (neededAllocs.set(BIT_END, !hovered), (neededAllocs.test(BIT_END) ? new Node(endPos) : hovered));

					g_allPorts.Reserve(neededAllocs.count());
					if (neededAllocs.test(BIT_START)) g_allPorts.Push(start);
					if (neededAllocs.test(BIT_END))   g_allPorts.Push(end);

					start->Push_O(end); // Push end as an output of start
					end->Push_I(start); // Push start as an input of end

			#undef BIT_START
			#undef BIT_END
				}
			}
			// M2 press
			// Switch modes
			if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
				mode = InputMode::Select;
			}
		} break;
		case InputMode::Select: {
			// M1 press
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				startPos = worldMousePos;
			}
			// M1 drag
			// Select
			if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
				if (worldMousePos.x < startPos.x) {
					collision.x = worldMousePos.x;
					collision.width = startPos.x - worldMousePos.x;
				}
				else {
					collision.x = startPos.x;
					collision.width = worldMousePos.x - startPos.x;
				}
				if (worldMousePos.y < startPos.y) {
					collision.y = worldMousePos.y;
					collision.height = startPos.y - worldMousePos.y;
				}
				else {
					collision.y = startPos.y;
					collision.height = worldMousePos.y - startPos.y;
				}

				for (Port* port : g_allPorts) {
					if (CheckCollisionPointRec(port->GetLocation(), collision)) {
						selection.insert(port);
					}
					else {
						if (selection.find(port) != selection.end()) selection.erase(port);
					}
				}
			}
			// M1 Release
			// Start editing selection
			if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
				if (!selection.empty()) mode = InputMode::Edit;
			}
			// M2 Press
			// Exit select mode
			if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
				selection.clear();
				mode = InputMode::Draw;
			}
		}	break;
		case InputMode::Edit: {
			// Middle release
			// Stop dragging
			if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON)) {
				for (Port* port : selection) {
					if (Port* existing = g_allPorts.FindAtPos(worldMousePos, selection)) {
						g_allPorts.Erase(existing); // First we remove the port from the world, so its source doesn't get overwritten
						g_allPorts.Replace(existing, port); // Then we overwrite every reference to it
						port->Push_I(existing->Prev(0)); // Replace port's input with existing's input
						delete existing; // And finally free it
					}
				}
				selection.clear();
			}
			// Middle dragging
			// Drag selection
			if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
				Vector2 mouse_delta = Vector2Subtract(mousePos, mousePos_last);
				Vector2 worldMouse_delta = Vector2Subtract(worldMousePos, worldMousePos_last);

				for (Port* port : selection) {
					port->SetLocation(Roundf(Vector2Add(port->GetLocation(), worldMouse_delta)));
				}
			}
			// Delete/backspace
			// Delete selected Port*s
			if (IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_BACKSPACE)) {
				g_allPorts.Wipe(selection);
				selection.clear();
				mode = InputMode::Select;
			}

			// Key
			{
				// Number
				// Change type of ports selected
				int pressed = GetCharPressed();
				if (pressed == '1' || pressed == '2') {
					Vector2 pos = Roundf(worldMousePos);
					std::unordered_set<Port*> updatedSelection;
					for (Port* port : selection) {
						switch (pressed)
						{
						case '1': // Set selected to nodes
							updatedSelection.insert(ToNode(port));
							break;
						case '2': // Set selected to gates
							updatedSelection.insert(ToGate(port));
							break;
						default:
							break;
						}
					}
					selection = updatedSelection;
				}
				// Letter
				// Change type of gate
				if (((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && (pressed == '|' || pressed == '&' || pressed == '!')) || pressed == '^') {
					for (Port* port : selection) {
						if (Gate* gate = dynamic_cast<Gate*>(port)) {
							switch (pressed)
							{
							case '|': // Set selected to nodes
								gate->SetMethod(Gate::Method::OR);
								break;
							case '&': // Set selected to gates
								gate->SetMethod(Gate::Method::AND);
								break;
							case '!': // Set selected to gates
								gate->SetMethod(Gate::Method::NOT);
								break;
							case '^': // Set selected to gates
								gate->SetMethod(Gate::Method::XOR);
								break;
							}
						}
					}
				}
			}

			// M2 Press
			if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
				selection.clear();
				mode = InputMode::Draw;
			}
		} break;
		}

		// Evaluate
		std::unordered_set<Port*> nullbodies; // Ports with no inputs nor outputs
		// Draw wires
		for (Port* port : g_allPorts) {
			if (port->I_Size() || port->O_Size()) port->GetValue(); // Evaluate
			else nullbodies.insert(port); // Eliminate
		}
		g_allPorts.Wipe(nullbodies);

		hovered = g_allPorts.FindAtPos(worldMousePos); // Might no longer be available

#pragma endregion

#pragma region Draw

		BeginDrawing(); {

			camera.offset = cameraPos;
			UpdateWorldPixel();

			switch (mode)
			{
			case InputMode::Draw:	ClearBackground(Color{ 0, 5, 10, 255 });	break;
			case InputMode::Select: ClearBackground(Color{ 20, 15, 0, 255 });	break;
			case InputMode::Edit:	ClearBackground(Color{ 20, 5, 30, 255 });	break;
			}

			BeginMode2D(camera); {

				{ // Draw the world grid
					float screenRight = worldPixel.x + worldScreen.width;
					float screenBottom = worldPixel.y + worldScreen.height;

					if (camera.zoom > 1.0f)
					{
						for (float y = worldPixel.y; y <= screenBottom; y += (worldPixel.height * camera.zoom)) {
							DrawLineV({ worldPixel.x, y }, { screenRight, y }, { 20,20,20,255 });
						}
						for (float x = worldPixel.x; x <= screenRight; x += (worldPixel.width * camera.zoom)) {
							DrawLineV({ x, worldPixel.y }, { x, screenBottom }, { 20,20,20,255 });
						}
					}
					else
					{
						DrawRectangleRec(worldScreen, { 20, 20, 20, 255 });
					}
				}

				if (mode == InputMode::Select && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
					DrawRectangleRec(collision, Color{ 255, 127, 0, 127 });
					DrawLineV({ startPos.x, startPos.y }, { worldMousePos.x, startPos.y }, Color{ 255, 127, 0, 255 });
					DrawLineV({ worldMousePos.x, startPos.y }, { worldMousePos.x, worldMousePos.y }, Color{ 255, 127, 0, 255 });
					DrawLineV({ worldMousePos.x, worldMousePos.y }, { startPos.x, worldMousePos.y }, Color{ 255, 127, 0, 255 });
					DrawLineV({ startPos.x, worldMousePos.y }, { startPos.x, startPos.y }, Color{ 255, 127, 0, 255 });
				}

				// Draw wires
				{
					if ((camera.zoom * 0.2f) >= (1.0f)) // The width of a wire must be greater than a pixel
					{
						std::queue<std::pair<Vector2, Vector2>> activeWires;
						std::queue<std::pair<Vector2, Vector2>> inactiveWires;

						// Fill out active/inactive wire queues
						for (Port* port : g_allPorts) {
							if (port) {
								Vector2 pt = port->GetLocation();
								float state = (port->GetValue() ? 0.0f : 1.0f);

								switch (port->GetType())
								{
								case Port::PortType::Gate:
									if (Port* next = port->Next(0)) { // next must not be nullptr
										std::pair<Vector2, Vector2> wire = { pt, next->GetLocation() };
										if (state) activeWires.push(wire);
										else inactiveWires.push(wire);
									}
									break;
								case Port::PortType::Node:
									for (Port* next : port->GetOutputs()) {
										std::pair<Vector2, Vector2> wire = { pt, next->GetLocation() };
										if (state) activeWires.push(wire);
										else inactiveWires.push(wire);
									}
									break;
								}
							}
						}

						// @ Draw the active wires
						float state = 1.0f;
						SetShaderValue(g_shader_wire, g_uniform_wireActive, &state, UNIFORM_FLOAT);
						BeginShaderMode(g_shader_wire);
						while (!activeWires.empty()) {
							DrawCordEx(activeWires.front().first, activeWires.front().second, WHITE, 0.2f);
							activeWires.pop();
						}
						EndShaderMode();

						// @ Draw the inactive wires
						state = 0.0f;
						SetShaderValue(g_shader_wire, g_uniform_wireActive, &state, UNIFORM_FLOAT);
						BeginShaderMode(g_shader_wire);
						while (!inactiveWires.empty()) {
							DrawCordEx(inactiveWires.front().first, inactiveWires.front().second, WHITE, 0.2f);
							inactiveWires.pop();
						}
						EndShaderMode();
					}
					else
					{
						for (Port* port : g_allPorts) {
							if (port) {
								Vector2 pt = port->GetLocation();
								bool state = port->GetValue();
								Color stateColor = (state ? Color{ 255, 0, 0, 255 } : WHITE);

								switch (port->GetType())
								{
								case Port::PortType::Gate:
									if (Port* next = port->Next(0)) { // next must not be nullptr
										DrawCord(pt, next->GetLocation(), stateColor);
									}
									break;
								case Port::PortType::Node:
									for (Port* next : port->GetOutputs()) {
										DrawCord(pt, next->GetLocation(), stateColor);
									}
									break;
								}
							}
						}
					}
				}

				if (hovered) {
					Vector2 pt = hovered->GetLocation();
					switch (hovered->GetType())
					{
					case Port::PortType::Gate:
						DrawGateIcon(((Gate*)hovered)->GetMethod(), pt, 3.0f, GOLD);
						break;

					case Port::PortType::Node:
						DrawCircleV(pt, .4f, GOLD);
						break;

					default:
						break;
					}
				}
				// Highlight selection
				for (Port* port : selection) {
					if (port) {
						Vector2 pt = port->GetLocation();
						switch (port->GetType())
						{
						case Port::PortType::Gate:
							DrawGateIcon(((Gate*)port)->GetMethod(), pt, 3.0f, YELLOW);
							break;

						case Port::PortType::Node:
							DrawCircleV(pt, .4f, YELLOW);
							break;

						default:
							break;
						}
					}
				}
				// Draw ports
				for (Port* port : g_allPorts) {
					if (port) {
						Vector2 pt = port->GetLocation();
						switch (port->GetType())
						{
						case Port::PortType::Gate: {
							DrawGateIcon(((Gate*)port)->GetMethod(), pt, 2.0f, BLUE);
						}	break;
						case Port::PortType::Node:
							DrawCircleV(pt, .1f, BLUE);
							break;

						default:
							break;
						}
					}
				}

				// Draw the wire the user is currently creating
				if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && (mode == InputMode::Draw)) {
					DrawCord(startPos, endPos, GRAY);
				}

			} EndMode2D();

#if _DEBUG
			DrawText(TextFormat("Mouse at: { %#1.1f, %#1.1f }\nTotal components: %i", worldMousePos.x, worldMousePos.y, g_allPorts.Size()), 0, 0, 8, WHITE);

			DrawText(TextFormat("Root{%s}", g_allPorts.ConstructTreeStr().c_str()), 0, 48, 8, WHITE);
			auto it = g_namedPtrs.find(hovered);
			if (it != g_namedPtrs.end())
				DrawText(TextFormat("Hovering %c", it->second), 86, 48, 8, WHITE);
#endif
			for (Port* port : g_allPorts) {
#if _DEBUG
				auto it = g_namedPtrs.find(port);
				if (it != g_namedPtrs.end()) {
					Vector2 pt = GetWorldToScreen2D(port->GetLocation(), camera);
					DrawText(TextFormat("%c", it->second), (int)pt.x + 1, (int)pt.y + 1, 8, MAGENTA);
				}
#endif
				if (Gate* gate = dynamic_cast<Gate*>(port)) gate->FrameEndReset();
			}
	} EndDrawing();

#pragma endregion
	}

	for (Port* port : g_allPorts) { if (port) delete port; }

	UnloadShader(g_shader_wire);
	UnloadShader(g_shader_cable);

	for (Texture& icon : gateIcons) {
		UnloadTexture(icon);
	}
	UnloadTexture(g_defaultTex);

	CloseWindow();
}

#pragma endregion
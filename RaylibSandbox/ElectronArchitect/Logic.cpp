#include "Logic.h"

WireShape operator++(WireShape shape) {
	switch (shape) {
	case WireShape::XFirst:		return WireShape::YFirst;
	case WireShape::YFirst:		return WireShape::DiagFirst;
	case WireShape::DiagFirst:	return WireShape::DiagLast;
	case WireShape::DiagLast:	return WireShape::XFirst;
	}
}
WireShape operator-(WireShape shape) {
	switch (shape) {
	case WireShape::XFirst:		return WireShape::DiagLast;
	case WireShape::YFirst:		return WireShape::XFirst;
	case WireShape::DiagFirst:	return WireShape::YFirst;
	case WireShape::DiagLast:	return WireShape::DiagFirst;
	}
}

Gate::Mode operator++(Gate::Mode m) {
	switch (m) {
	case Gate::Mode::OR:  return Gate::Mode::AND;
	case Gate::Mode::NOR: return Gate::Mode::AND;
	case Gate::Mode::AND: return Gate::Mode::XOR;
	case Gate::Mode::XOR: return Gate::Mode::OR;

	default: return Gate::Mode('err');
	}
}
Gate::Mode operator--(Gate::Mode m) {
	switch (m) {
	case Gate::Mode::OR:  return Gate::Mode::XOR;
	case Gate::Mode::NOR: return Gate::Mode::OR;
	case Gate::Mode::AND: return Gate::Mode::NOR;
	case Gate::Mode::XOR: return Gate::Mode::AND;

	default: return Gate::Mode('err');
	}
}

char Char(Gate::Mode m, int i) {
	char c = (char)(((int)m / i) % 256);
	if (c <= 0) return ' ';
	else return c;
}

int EvaluationDepth(Component* comp) {
	if (!comp) return 0; // Skip nullptrs

	std::vector<IPort> inputArr;
	if (Gate* gate = (Gate*)comp) inputArr = gate->in;
	else if (Unit* unit = (Unit*)comp) {
		// Unit's inputs are in a stack array with 64 members.
		// We only need the usable subset of that.
		inputArr.reserve(unit->LanesIn());
		for (int i = 0; i < unit->LanesIn(); ++i) {
			for (int ab = 0; ab < 2; ++ab) {
				inputArr.push_back(unit->in[ab][i]);
			}
		}
	}
	else return 0; // Skip Batteries

	int maxDepth = 0;
	for (IPort& port : inputArr) {
		int portDepth = EvaluationDepth(port.component);
		if (portDepth > maxDepth) maxDepth = portDepth;
	}
	return maxDepth;
}

char Char(Unit::Mode m, int i) {
	char c = (char)(((int)m / i) % 256);
	if (c <= 0) return ' ';
	else return c;
}

Unit::Mode operator++(Unit::Mode m) {
	switch (m) {
	case Unit::Mode::OR:  return Unit::Mode::AND;
	case Unit::Mode::AND: return Unit::Mode::XOR;
	case Unit::Mode::XOR: return Unit::Mode::FLP;
	case Unit::Mode::FLP: return Unit::Mode::LSH;
	case Unit::Mode::LSH: return Unit::Mode::RSH;
	case Unit::Mode::RSH: return Unit::Mode::OR;

	case Unit::Mode::LES: return Unit::Mode::MOR;
	case Unit::Mode::MOR: return Unit::Mode::EQU;
	case Unit::Mode::EQU: return Unit::Mode::LES;

	case Unit::Mode::ADD: return Unit::Mode::SUB;
	case Unit::Mode::SUB: return Unit::Mode::MUL;
	case Unit::Mode::MUL: return Unit::Mode::DIV;
	case Unit::Mode::DIV: return Unit::Mode::MOD;
	case Unit::Mode::MOD: return Unit::Mode::ADD;

	case Unit::Mode::TGL: return Unit::Mode::RS;
	case Unit::Mode::RS:  return Unit::Mode::RSG;
	case Unit::Mode::RSG: return Unit::Mode::TGL;

	default: return Unit::Mode('err');
	}
}

Unit::Mode operator--(Unit::Mode m) {
	switch (m) {
	case Unit::Mode::OR:  return Unit::Mode::RSH;
	case Unit::Mode::AND: return Unit::Mode::OR;
	case Unit::Mode::XOR: return Unit::Mode::AND;
	case Unit::Mode::FLP: return Unit::Mode::XOR;
	case Unit::Mode::LSH: return Unit::Mode::FLP;
	case Unit::Mode::RSH: return Unit::Mode::LSH;

	case Unit::Mode::LES: return Unit::Mode::EQU;
	case Unit::Mode::MOR: return Unit::Mode::LES;
	case Unit::Mode::EQU: return Unit::Mode::MOR;

	case Unit::Mode::ADD: return Unit::Mode::MOD;
	case Unit::Mode::SUB: return Unit::Mode::ADD;
	case Unit::Mode::MUL: return Unit::Mode::SUB;
	case Unit::Mode::DIV: return Unit::Mode::MUL;
	case Unit::Mode::MOD: return Unit::Mode::DIV;

	case Unit::Mode::TGL: return Unit::Mode::RSG;
	case Unit::Mode::RS:  return Unit::Mode::TGL;
	case Unit::Mode::RSG: return Unit::Mode::RS;

	default: return Unit::Mode('err');
	}
}

int Unit::LanesIn()
{
	return lanes_in;
}

int Unit::LanesOut()
{
	return lanes_out;
}

size_t Unit::Eval(Mode method, size_t a, size_t b)
{
	switch (method)
	{
	case Unit::Mode::OR:  return (a | b);
	case Unit::Mode::AND: return (a & b);
	case Unit::Mode::XOR: return (a ^ b);
	case Unit::Mode::FLP: return (~a);
	case Unit::Mode::LSH: return (a << b);
	case Unit::Mode::RSH: return (a >> b);

	case Unit::Mode::LES: return (a < b);
	case Unit::Mode::MOR: return (a > b);
	case Unit::Mode::EQU: return (a == b);

	case Unit::Mode::ADD: return (a + b);
	case Unit::Mode::SUB: return (a - b);
	case Unit::Mode::MUL: return (a * b);
	case Unit::Mode::DIV: return (a / b);
	case Unit::Mode::MOD: return (a % b);

	case Unit::Mode::TGL: return (!a);
	case Unit::Mode::RS:  return (a); // TODO These only make sense when operating on the registers
	case Unit::Mode::RSG: return (a); // TODO These only make sense when operating on the registers

	default: return a;
	}
}

bool Unit::GetBit(int bit) { // TODO: this does not check to see if it should be evaluated or not before returning the current state of the bit!
	return (state & (1 << bit));
}

void Unit::SetBit(int bit, bool set) {
	if (set) state |= (1 << bit);
	else state &= ~(1 << bit);
}

void Unit::SetFromNum(size_t bits) {
	int mask = 1;
	for (int i = 0; i < LanesOut(); ++i, mask <<= 1) {
		SetBit(i, (bits & mask));
	}
}

size_t Unit::InputsAsNum() {
	size_t bits = 0; // size_t (64 bits) is the maximum width we will allow for a Unit
	int mask = 1;
	for (int i = 0; i < LanesIn(); ++i, mask <<= 1) {
		if (in[i].GetState()) bits |= mask;
	}
	return bits;
}

size_t Unit::StateAsNum() {
	size_t bits = 0; // size_t (64 bits) is the maximum width we will allow for a Unit
	int mask = 1;
	for (int i = 0; i < LanesOut(); ++i, mask <<= 1) {
		if (GetBit(i)) bits |= mask;
	}
	return bits;
}

Unit::Mode Unit::GetMode() {
	return mode;
}

void Unit::SetMode(Mode _mode) {
	switch (mode = _mode)
	{
	default:
	case Mode::OR:  case Mode::AND: case Mode::XOR: case Mode::FLP: case Mode::LSH: case Mode::RSH:
	case Mode::ADD: case Mode::SUB: case Mode::MUL: case Mode::DIV: case Mode::MOD:
		// <size> in, <size> out
		lanes_in = 0;
		lanes_out = 0;
		break;

	case Mode::LES: case Mode::MOR: case Mode::EQU:
		// <size> in, 1	out
		lanes_in = 0;
		lanes_out = 1;
		break;

		// Latches
	case Mode::TGL:
		// 1 in, 1 out
		lanes_in = 1;
		lanes_out = 1;
		break;
	case Mode::RS: case Mode::RSG:
		// 2 in, 1 out
		lanes_in = 2;
		lanes_out = 1;
		break;
	}
}

void Unit::SetWidth(int _lanes, Mode _mode) {
	if (_mode == Mode('err')) _mode = mode;
	SetMode(_mode);
	if (!LanesIn()) lanes_in = _lanes;
	if (!LanesOut()) lanes_out = _lanes;
}

bool Unit::IsPolymorphic() {
	return true;
}

IOPort& IOPort::operator=(Gate* _gate) {
	if (tag == State_UnitPort) unitPort.port = 0;
	tag = State_Gate;
	gate = _gate;
	return *this;
}

IOPort& IOPort::operator=(std::pair<Unit*, int> _unitPort) {
	tag = State_UnitPort;
	unitPort = { _unitPort.first, _unitPort.second };
	return *this;
}

IOPort& IOPort::operator=(Batt* _batt) {
	if (tag == State_UnitPort) unitPort.port = 0;
	tag = State_Batt;
	batt = _batt;
	return *this;
}

bool Gate::GetState() {
	return state;
}

// Evaluate the inputs using the supplied method
bool Gate::Eval(Mode method, std::vector<bool> inputs)
{
	bool out   = (method == Mode::AND);
	bool b_xor = (method == Mode::XOR);

	for (bool b : inputs) {
		if (method == Mode::AND ? !b : b) {
			if (b_xor && out) return false;
			out = b;
			if (!b_xor) break;
		}
	}
	return (method == Mode::NOR ? !out : out);
}

bool Gate::IsPolymorphic() {
	return true;
}

// Returns the state of the port, whatever its tag may be.
bool IPort::GetState() {
	switch (tag) {
	case IOPort::State_UnitPort: return unitPort.unit->GetBit(unitPort.port);
	case IOPort::State_Gate: if (gate) { return gate->GetState(); }
	case IOPort::State_Batt: default: return false;
	}
}

bool Batt::IsPolymorphic() {
	return true;
}

IOPort::operator Gate* () {
	return dynamic_cast<Gate*>(component);
}
IOPort::operator Unit* () {
	return dynamic_cast<Unit*>(component);
}

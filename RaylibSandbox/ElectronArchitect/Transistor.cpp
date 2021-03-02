#include "Transistor.h"

bool Unit::GetBit(int at) { // TODO: this does not check to see if it should be evaluated or not before returning the current state of the bit!
	return state[at].second;
}

size_t Unit::AsNum() { // Using this for any case in which (state.size() != lanes) is UB
	size_t bits = 0;
	for (int i = 0, int mask = 1; i < lanes; ++i, mask <<= 1) {
		if (GetBit(i)) bits |= mask;
	}
	return bits;
}

void Unit::SetMode(Mode _mode) {
	switch (mode = _mode)
	{
	default:
	case Mode::OR:  case Mode::AND: case Mode::XOR: case Mode::FLP: case Mode::LSH: case Mode::RSH:
	case Mode::ADD: case Mode::SUB: case Mode::MUL: case Mode::DIV: case Mode::MOD:
		// <size> in, <size> out
		state.resize(lanes);
		out.resize(lanes);
		break;

	case Mode::LES: case Mode::MOR: case Mode::EQU:
		// <size> in, 1	out
		state.resize(lanes);
		out.resize(1);
		break;

		// Latches
	case Mode::TGL:
		// 1 in, 1 out
		state.resize(1);
		out.resize(1);
		break;
	case Mode::RS: case Mode::RSG:
		// 2 in, 1 out
		state.resize(2);
		out.resize(1);
		break;
	}
}

bool Unit::SetLanes(int _lanes) { // Returns true on success
	if (mode == Mode::TGL || mode == Mode::RS || mode == Mode::RSG) return false;
	else {
		lanes = _lanes;
		if (out.size() != 1) out.resize(lanes); // May need to re-evaluate this if there's ever a case added with (1 < out.size() < lanes).
		state.resize(lanes);
	}
}

#include <raymath.h>
#include "Transistors.h"

#pragma region LogicDelegate

	void LogicDelegate::CopyInputs(LogicDelegate* logic)
	{
		switch (logic->GetType())
		{
		case Eval::L_AND: this->CopyInputs(static_cast<LogicAND*>(logic)); break;
		case Eval::L_NOT: this->CopyInputs(static_cast<LogicNOT*>(logic)); break;
		case Eval::L_ORR: this->CopyInputs(static_cast<LogicORR*>(logic)); break;
		case Eval::L_XOR: this->CopyInputs(static_cast<LogicXOR*>(logic)); break;
		}
	}

#pragma endregion

#pragma region Gate

	Gate::Gate() : position(Vector2Zero()), evaluated(), evaluation(), logic(new LogicORR) {}
	Gate::Gate(Vector2 position) : position(position), evaluated(), evaluation(), logic(new LogicORR) {}
	Gate::Gate(Vector2 position, Eval type) : position(position), evaluated(), evaluation()
	{
		switch (type)
		{
		case Eval::L_AND: logic = new LogicAND; break;
		case Eval::L_NOT: logic = new LogicNOT; break;
		case Eval::L_ORR: logic = new LogicORR; break;
		case Eval::L_XOR: logic = new LogicXOR; break;
		}
	}
	Gate::~Gate()
	{
		delete logic;
	}
	Vector2 Gate::GetPosition()
	{
		return position;
	}
	void Gate::SetPosition(Vector2 pos)
	{
		position = pos;
	}
	bool Gate::Evaluate()
	{
		if (!evaluated)
		{
			evaluation = logic->Evaluate();
			evaluated = true;
		}
		return evaluation;
	}
	void Gate::Reset()
	{
		evaluated = false;
	}
	size_t Gate::Count_I() const
	{
		return logic->InputCount();
	}
	size_t Gate::Count_O() const
	{
		return o.size();
	}
	void Gate::Push_I(Gate* input)
	{
		return logic->PushInput(input);
	}
	void Gate::Erase_I(Gate* input)
	{
		return logic->EraseInput(input);
	}
	void Gate::Push_O(Gate* output)
	{
		o.insert(output);
	}
	void Gate::Erase_O(Gate* output)
	{
		o.erase(output);
	}
	Status Gate::CouldPush_I(Gate* input) const
	{
		return logic->CouldPushInput(input);
	}
	Status Gate::CouldErase_I(Gate* input) const
	{
		return logic->CouldEraseInput(input);
	}
	Status Gate::CouldPush_O(Gate* output) const
	{
		if (o.find(output) != o.end()) return Status::Complete;
		else return Status::Possible;
	}
	Status Gate::CouldErase_O(Gate* output) const
	{
		if (o.find(output) != o.end()) return Status::Possible;
		else return Status::Complete;
	}
	Eval Gate::GetType()
	{
		return logic->GetType();
	}
	void Gate::SetType(Eval type)
	{
		if (logic)
		{
			if (GetType() == type) return;
			LogicDelegate* past = logic;
			switch (type)
			{
			case Eval::L_AND: logic = new LogicAND; break;
			case Eval::L_NOT: logic = new LogicNOT; break;
			case Eval::L_ORR: logic = new LogicORR; break;
			case Eval::L_XOR: logic = new LogicXOR; break;
			}
			if (past)
			{
				logic->CopyInputs(past);
				past->ClearSelfReferences(this, logic->InputCount());
				delete past;
			}
		}
	}

#pragma endregion

bool Solder(Gate* input, Gate* output)
{
	Status oPushable = input->CouldPush_O(output);
	Status iPushable = input->CouldPush_I(input);
	if (oPushable != Status::CantDoIt && iPushable != Status::CantDoIt)
	{
		if (oPushable != Status::Complete) input->Push_O(output);
		if (iPushable != Status::Complete) output->Push_I(input);
		return true;
	}
	else return false;
}
void DeSolder(Gate* input, Gate* output)
{
	Status oEraseable = input->CouldErase_O(output);
	Status iEraseable = input->CouldErase_I(input);
	if (oEraseable != Status::CantDoIt && iEraseable != Status::CantDoIt)
	{
		if (oEraseable != Status::Complete) input->Erase_O(output);
		if (iEraseable != Status::Complete) output->Erase_I(input);
	}
}
void Gate::Destroy()
{
	logic->ClearSelfReferences(this);
	for (Gate* output : o)
	{
		Status stat = output->CouldErase_I(this);
		if (stat == Status::Possible) output->Erase_I(this);
	}
}

#pragma region LogicAND

	bool LogicAND::Evaluate() const
	{
		for (Gate* gate : i)
		{
			if (!gate->Evaluate()) return false;
		}
		return true;
	}
	size_t LogicAND::InputCount() const
	{
		return i.size();
	}
	void LogicAND::PushInput(Gate* input)
	{
		i.insert(input);
	}
	void LogicAND::EraseInput(Gate* input)
	{
		i.erase(input);
	}
	Status LogicAND::CouldPushInput(Gate* input) const
	{
		if (i.find(input) != i.end()) return Status::Complete;
		else return Status::Possible;
	}
	Status LogicAND::CouldEraseInput(Gate* input) const
	{
		if (i.find(input) != i.end()) return Status::Possible;
		else return Status::Complete;
	}
	Eval LogicAND::GetType() const
	{
		return Eval::L_AND;
	}
	void LogicAND::CopyInputs(LogicAND* src)
	{
		this->i = src->i;
	}
	void LogicAND::CopyInputs(LogicNOT* src)
	{
		if (src->i) this->i = { src->i };
		else this->i = { };
	}
	void LogicAND::CopyInputs(LogicORR* src)
	{
		this->i = src->i;
	}
	void LogicAND::CopyInputs(LogicXOR* src)
	{
		if (src->i.a)
		{
			if (src->i.b) this->i = { src->i.a, src->i.b };
			else this->i = { src->i.a };
		}
		else
		{
			if (src->i.b) this->i = { src->i.b };
			else this->i = { };
		}
	}
	void LogicAND::ClearSelfReferences(Gate* self, size_t start)
	{
#ifdef _DEBUG
		_ASSERT_EXPR(
			start > 0ull,
			"Called ClearSelfReferences(Gate*, size_t) with start offset when start offset was zero! Please use ClearSelfReferences(Gate*) instead when clearing without an offset!");
		_ASSERT_EXPR(
			start < i.size(),
			"Tried to use an offset beyond the bounds of the logic unit's capacity!");
#endif
		auto it = i.begin();
		// iterate to start
		if (start)
		{
			while (it != i.end() && start)
			{
				++it;
				--start;
			}
		}
		while (it != i.end())
		{
			Status stat = (*it)->CouldErase_O(self);
			if (stat == Status::Possible) (*it)->Erase_O(self);
			++it;
		}
	}
	void LogicAND::ClearSelfReferences(Gate* self)
	{
		for (Gate* input : i)
		{
			if (input->CouldErase_O(self) == Status::Possible) input->Erase_O(self);
		}
	}

#pragma endregion

#pragma region LogicNOT

	bool LogicNOT::Evaluate() const
	{
		return !i->Evaluate();
	}
	size_t LogicNOT::InputCount() const
	{
		return (i != nullptr ? 1ull : 0ull);
	}
	void LogicNOT::PushInput(Gate* input)
	{
#ifdef _DEBUG
		_ASSERT_EXPR(
			CouldPushInput(input) != Status::CantDoIt,
			"Tried to push input without input being free! This can lead to issues where past inputs still see this as an output!");
#endif
		i = input;
	}
	void LogicNOT::EraseInput(Gate* input)
	{
#ifdef _DEBUG
		_ASSERT_EXPR(
			CouldEraseInput(input) != Status::CantDoIt,
			"Tried to erase input without checking that the input matches! This can lead to issues where past inputs still see this as an output!");
#endif
		i = nullptr;
	}
	Status LogicNOT::CouldPushInput(Gate* input) const
	{
		if (!input) return Status::CantDoIt; // Can't push nullptr
		if (i)
		{
			if (i == input) return Status::Complete; // Input already matches
			else return Status::CantDoIt; // Can't overwrite
		}
		else return Status::Possible; // Can push (i is nullptr)
	}
	Status LogicNOT::CouldEraseInput(Gate* input) const
	{
		if (!input) return Status::CantDoIt; // Can't erase nullptr
		if (i)
		{
			if (i == input) return Status::Possible; // Can erase (input found)
			else return Status::CantDoIt; // Can't erase (input didn't match existing)
		}
		else return Status::Complete; // Already erased (input is nullptr)
	}
	Eval LogicNOT::GetType() const
	{
		return Eval::L_NOT;
	}
	void LogicNOT::CopyInputs(LogicAND* src)
	{
		if (!src->i.empty()) this->i = *src->i.begin();
		else this->i = nullptr;
	}
	void LogicNOT::CopyInputs(LogicNOT* src)
	{
		this->i = src->i;
	}
	void LogicNOT::CopyInputs(LogicORR* src)
	{
		if (!src->i.empty()) this->i = *src->i.begin();
		else this->i = nullptr;
	}
	void LogicNOT::CopyInputs(LogicXOR* src)
	{
		if (src->i.a) this->i = src->i.a;
		else this->i = src->i.b;
	}
	void LogicNOT::ClearSelfReferences(Gate* self, size_t start)
	{
#ifdef _DEBUG
		_ASSERT_EXPR(
			start > 0ull,
			"Called ClearSelfReferences(Gate*, size_t) with start offset when start offset was zero! Please use ClearSelfReferences(Gate*) instead when clearing without an offset!");
		_ASSERT_EXPR(
			false,
			"Tried to use an offset beyond the bounds of the logic unit's capacity!"); // Don't bother using ClearSelfReferences() with offset on a LogicNOT at all.
#endif
		if (i && i->CouldErase_O(self) == Status::Possible)
		{
			i->Erase_O(self);
		}
	}
	void LogicNOT::ClearSelfReferences(Gate* self)
	{
		if (i && i->CouldErase_O(self) == Status::Possible) i->Erase_O(self);
	}

#pragma endregion

#pragma region LogicORR

	bool LogicORR::Evaluate() const
	{
		for (Gate* gate : i)
		{
			if (gate->Evaluate()) return true;
		}
		return false;
	}
	size_t LogicORR::InputCount() const
	{
		return i.size();
	}
	void LogicORR::PushInput(Gate* input)
	{
		i.insert(input);
	}
	void LogicORR::EraseInput(Gate* input)
	{
		i.erase(input);
	}
	Status LogicORR::CouldPushInput(Gate* input) const
	{
		if (i.find(input) != i.end()) return Status::Complete;
		else return Status::Possible;
	}
	Status LogicORR::CouldEraseInput(Gate* input) const
	{
		if (i.find(input) != i.end()) return Status::Possible;
		else return Status::Complete;
	}
	Eval LogicORR::GetType() const
	{
		return Eval::L_ORR;
	}
	void LogicORR::CopyInputs(LogicAND* src)
	{
		this->i = src->i;
	}
	void LogicORR::CopyInputs(LogicNOT* src)
	{
		if (src->i) this->i = { src->i };
		else this->i = { };
	}
	void LogicORR::CopyInputs(LogicORR* src)
	{
		this->i = src->i;
	}
	void LogicORR::CopyInputs(LogicXOR* src)
	{
		if (src->i.a)
		{
			if (src->i.b) this->i = { src->i.a, src->i.b };
			else this->i = { src->i.a };
		}
		else
		{
			if (src->i.b) this->i = { src->i.b };
			else this->i = { };
		}
	}
	void LogicORR::ClearSelfReferences(Gate* self, size_t start)
	{
#ifdef _DEBUG
		_ASSERT_EXPR(
			start > 0ull,
			"Called ClearSelfReferences(Gate*, size_t) with start offset when start offset was zero! Please use ClearSelfReferences(Gate*) instead when clearing without an offset!");
		_ASSERT_EXPR(
			start < i.size(),
			"Tried to use an offset beyond the bounds of the logic unit's capacity!");
#endif
		auto it = i.begin();
		// iterate to start
		if (start)
		{
			while (it != i.end() && start)
			{
				++it;
				--start;
			}
		}
		while (it != i.end())
		{
			Status stat = (*it)->CouldErase_O(self);
			if (stat == Status::Possible) (*it)->Erase_O(self);
			++it;
		}
	}
	void LogicORR::ClearSelfReferences(Gate* self)
	{
		for (Gate* input : i)
		{
			if (input->CouldErase_O(self) == Status::Possible) input->Erase_O(self);
		}
	}

#pragma endregion

#pragma region LogicXOR

	bool LogicXOR::Evaluate() const
	{
		return (i.a->Evaluate() ? !i.b->Evaluate() : i.a->Evaluate());
	}
	size_t LogicXOR::InputCount() const
	{
		return ((i.a != nullptr ? 1ull : 0ull) + (i.b != nullptr ? 1ull : 0ull));
	}
	void LogicXOR::PushInput(Gate* input)
	{
#ifdef _DEBUG
		_ASSERT_EXPR(
			CouldPushInput(input) != Status::CantDoIt,
			"Tried to push input without checking that an input is free to push to! This can lead to issues where past inputs still see this as an output!");
#endif
		if (!i.a) i.a = input;
		else if (!i.b && (input != i.a)) i.b = input;
	}
	void LogicXOR::EraseInput(Gate* input)
	{
#ifdef _DEBUG
		_ASSERT_EXPR(
			CouldEraseInput(input) != Status::CantDoIt,
			"Tried to erase input without checking that the input matches! This can lead to issues where past inputs still see this as an output!");
#endif
		if (i.a == input) i.a = nullptr;
		else if (i.b == input) i.b = nullptr;
	}
	Status LogicXOR::CouldPushInput(Gate* input) const
	{
		if (i.a == input || i.b == input) return Status::Complete;
		else if (!i.a || !i.b) return Status::Possible;
		else return Status::CantDoIt;
	}
	Status LogicXOR::CouldEraseInput(Gate* input) const
	{
		if (i.a == input || i.b == input) return Status::Possible;
		else if (i.a != input && i.b != input) return Status::Complete;
		else return Status::CantDoIt;
	}
	Eval LogicXOR::GetType() const
	{
		return Eval::L_XOR;
	}
	void LogicXOR::CopyInputs(LogicAND* src)
	{
		if (src->i.size() > 0) this->i.a = *src->i.begin();
		else this->i.a = nullptr;
		if (src->i.size() > 1) this->i.b = *++src->i.begin();
		else this->i.b = nullptr;
	}
	void LogicXOR::CopyInputs(LogicNOT* src)
	{
		this->i.a = src->i;
		this->i.b = nullptr;
	}
	void LogicXOR::CopyInputs(LogicORR* src)
	{
		if (src->i.size() > 0) this->i.a = *src->i.begin();
		else this->i.a = nullptr;
		if (src->i.size() > 1) this->i.b = *++src->i.begin();
		else this->i.b = nullptr;
	}
	void LogicXOR::CopyInputs(LogicXOR* src)
	{
		this->i = src->i;
	}
	void LogicXOR::ClearSelfReferences(Gate* self, size_t start)
	{
#ifdef _DEBUG
		_ASSERT_EXPR(
			start > 0ull,
			"Called ClearSelfReferences(Gate*, size_t) with start offset when start offset was zero! Please use ClearSelfReferences(Gate*) instead when clearing without an offset!");
		_ASSERT_EXPR(
			start < 2ull,
			"Tried to use an offset beyond the bounds of the logic unit's capacity!");
#endif
		if (start == 0ull; i.a && i.a->CouldErase_O(self) == Status::Possible) i.a->Erase_O(self);
		if (start <= 1ull; i.b && i.b->CouldErase_O(self) == Status::Possible) i.b->Erase_O(self);
	}
	void LogicXOR::ClearSelfReferences(Gate* self)
	{
		if (i.a && i.a->CouldErase_O(self) == Status::Possible) i.a->Erase_O(self);
		if (i.b && i.b->CouldErase_O(self) == Status::Possible) i.b->Erase_O(self);
	}

#pragma endregion
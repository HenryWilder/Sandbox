#pragma once
#include <set>
#include <raylib.h>

enum class Eval : char
{
	L_AND = '&',
	L_NOT = '!',
	L_ORR = '|',
	L_XOR = '^',
};

enum class Status : bool
{
	CantDoIt = false, // Can't do it (breaks rules)
	Complete = false, // Already did it (don't need to)
	Possible = true, // Can do it
};

class Gate;
class LogicDelegate;
class LogicAND;
class LogicNOT;
class LogicORR;
class LogicXOR;

class LogicDelegate
{
public:
	virtual bool Evaluate() const = 0;

	virtual size_t InputCount() const = 0;

	virtual void PushInput(Gate* input) = 0;
	virtual void EraseInput(Gate* input) = 0;

	virtual Status CouldPushInput(Gate* input) const = 0;
	virtual Status CouldEraseInput(Gate* input) const = 0;

	virtual Eval GetType() const = 0;

	virtual void CopyInputs(LogicAND*) = 0;
	virtual void CopyInputs(LogicNOT*) = 0;
	virtual void CopyInputs(LogicORR*) = 0;
	virtual void CopyInputs(LogicXOR*) = 0;
	void CopyInputs(LogicDelegate* logic);

	virtual void ClearSelfReferences(Gate* self, size_t start) = 0;
	virtual void ClearSelfReferences(Gate* self) = 0;
};

class Gate
{
private:
	Vector2 position;
	bool evaluated;
	bool evaluation;
	LogicDelegate* logic;
	std::set<Gate*> o;

public:
	Gate();
	Gate(Vector2 position);
	Gate(Vector2 position, Eval type);
	~Gate();

	Vector2 GetPosition();
	void SetPosition(Vector2 pos);

	bool Evaluate();
	void Reset();

	size_t Count_I() const;
	size_t Count_O() const;

	void Push_I(Gate* input);
	void Erase_I(Gate* input);

	void Push_O(Gate* output);
	void Erase_O(Gate* output);

	Status CouldPush_I(Gate* input) const;
	Status CouldErase_I(Gate* input) const;

	Status CouldPush_O(Gate* output) const;
	Status CouldErase_O(Gate* output) const;

	Eval GetType();
	void SetType(Eval type);

	void Destroy();
};

bool Solder(Gate* input, Gate* output);
void DeSolder(Gate* input, Gate* output);

class LogicAND : public LogicDelegate
{
private:
	std::set<Gate*> i;

public:
	friend class LogicNOT;
	friend class LogicORR;
	friend class LogicXOR;

	bool Evaluate() const override;

	size_t InputCount() const override;

	void PushInput(Gate* input) override;
	void EraseInput(Gate* input) override;

	Status CouldPushInput(Gate* input) const override;
	Status CouldEraseInput(Gate* input) const override;

	Eval GetType() const override;

	void CopyInputs(LogicAND*) override;
	void CopyInputs(LogicNOT*) override;
	void CopyInputs(LogicORR*) override;
	void CopyInputs(LogicXOR*) override;

	void ClearSelfReferences(Gate* self, size_t start) override;
	void ClearSelfReferences(Gate* self) override;
};

class LogicNOT : public LogicDelegate
{
private:
	Gate* i;

public:
	friend class LogicAND;
	friend class LogicORR;
	friend class LogicXOR;

	bool Evaluate() const override;

	size_t InputCount() const override;

	void PushInput(Gate* input) override;
	void EraseInput(Gate* input) override;

	Status CouldPushInput(Gate* input) const override;
	Status CouldEraseInput(Gate* input) const override;

	Eval GetType() const override;

	void CopyInputs(LogicAND*) override;
	void CopyInputs(LogicNOT*) override;
	void CopyInputs(LogicORR*) override;
	void CopyInputs(LogicXOR*) override;

	void ClearSelfReferences(Gate* self, size_t start) override;
	void ClearSelfReferences(Gate* self) override;
};

class LogicORR : public LogicDelegate
{
private:
	std::set<Gate*> i;

public:
	friend class LogicAND;
	friend class LogicNOT;
	friend class LogicXOR;

	bool Evaluate() const override;

	size_t InputCount() const override;

	void PushInput(Gate* input) override;
	void EraseInput(Gate* input) override;

	Status CouldPushInput(Gate* input) const override;
	Status CouldEraseInput(Gate* input) const override;

	Eval GetType() const override;

	void CopyInputs(LogicAND*) override;
	void CopyInputs(LogicNOT*) override;
	void CopyInputs(LogicORR*) override;
	void CopyInputs(LogicXOR*) override;

	void ClearSelfReferences(Gate* self, size_t start) override;
	void ClearSelfReferences(Gate* self) override;
};

class LogicXOR : public LogicDelegate
{
private:
	struct { Gate* a; Gate* b; } i;

public:
	friend class LogicAND;
	friend class LogicNOT;
	friend class LogicORR;

	bool Evaluate() const override;

	size_t InputCount() const override;

	void PushInput(Gate* input) override;
	void EraseInput(Gate* input) override;

	Status CouldPushInput(Gate* input) const override;
	Status CouldEraseInput(Gate* input) const override;

	Eval GetType() const override;

	void CopyInputs(LogicAND*) override;
	void CopyInputs(LogicNOT*) override;
	void CopyInputs(LogicORR*) override;
	void CopyInputs(LogicXOR*) override;

	void ClearSelfReferences(Gate* self, size_t start) override;
	void ClearSelfReferences(Gate* self) override;
};
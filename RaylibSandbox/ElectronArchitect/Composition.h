#pragma once
#define __COMPOSITION__

class Object;



// Either a wire or cable
__interface ConnectionDelegate
{
public:
	virtual size_t Find(Object* what) = 0;			// Searches for the connection in the connector
	virtual bool Valid(size_t index) = 0;			// Tells whether the index is a valid connection id

	virtual std::vector<Object*> GetNode() = 0;		// Returns a copy of the connection

	virtual void _UnRef(size_t index) = 0;			// Remove the connection without reference cleaning
	virtual void _UnRef(Object* what) = 0;			// Remove the connection without reference cleaning
	virtual void Push(Object* what) = 0;			// Add a connection--erases existing connection if needed
	virtual void Erase(Object* what) = 0;			// Remove the connection; cleans reference to self

	virtual size_t Width() = 0;						// The throughput capacity of the connection
	virtual size_t Size() = 0;						// How many throughpoints are currently valid
};

// Single-object connector
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

	std::vector<Object*> GetNode() override
	{
		return { connection };
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

// Multi-object connector
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

	std::vector<Object*> GetNode() override
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



// One or multiple cables or wires
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
class IO_Direct : public IODelegate
{
private:
	ConnectionDelegate* io;

public:
	std::vector<ConnectionDelegate*> GetConnector() override
	{
		return { io };
	}

	void _UnRef(ConnectionDelegate* what) override
	{
		if (io == what) io = nullptr;
	}
	void Push(ConnectionDelegate* what) override
	{
		if (io)
		{
			std::vector<Object*> nodes = io->GetNode();
			for (size_t i = 0; i < nodes.size(); ++i)
			{
				IODelegate* connections;
				connections = nodes[i]->
			}
		FOUND:
		}
	}
	void Erase(ConnectionDelegate* what) override;

	size_t Size() override;
	size_t Capacity() override;
};

// Exactly 2 connections
class IO_Pair : public IODelegate
{
private:
	struct { ConnectionDelegate* a; ConnectionDelegate* b; } io;

public:
	std::vector<ConnectionDelegate*> Get()
	{
		return { io.a, io.b };
	}
};

// Various connections
class IO_Vector : public IODelegate
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
		input->GetConnector()[wire]->GetNode(bit)->GetValue(wire, bit);
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



// Handler
class Object
{
private:
	Vector2 pos;			// Position of the object
	IODelegate* i;			// Source for evaluation data
	IODelegate* o;			// Where to send values
	EvaluationDelegate* e;	// Method for evaluating

public:
	Vector2 GetLocation()
	{
		return pos;
	}

	enum class Type
	{
		Node, // Repeater
		Gate, // Logical
		Comp, // Comparative
		Bitw, // Bitwise
		Math, // Arithmetic
	};

	void Init_Node()
	{
		i = IO_Direct();
	}
	void Init_Gate()
	{

	}
	void Init_Comp()
	{

	}
	void Init_Unit()
	{

	}
	void Free()
	{
		delete i;
		i = nullptr;
		delete o;
		o = nullptr;
		delete e;
		e = nullptr;
	}
	void Make(Type into)
	{
		Free();
		Init(into);
	}

	Object(Vector2 _pos) : pos(_pos), i(), o(), e() {}

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
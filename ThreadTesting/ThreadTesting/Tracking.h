#pragma once
#include <vector>
#include <string>
#include <chrono>

long long GetTime();

int GetTimeMS();

namespace tracking
{
	typedef void* Address; // Literally just a pointer, we aren't passing any data through this.

	// Single request to read/write at a specific memory address
	struct Request
	{
		Request();

		int m_time; // 4 bytes - When this request ocurred
	};
	struct Pull : Request
	{
		Pull() : Request() {};
	};
	struct Push : Request
	{
		Push(char newValue) : Request(), m_value{ newValue } {};
		char m_value; // 1 byte - If it is a push, this will be the new value. Note that it is also fake and has little to do with the actual value.
	};

	// A variable and all its read/write requests
	// All variables used in the program should be declared before being used
	struct Variable
	{
		Variable(Address); // Creates a Variable struct from an input variable address

		Address m_address; // The location of this variable
		std::vector<Request> m_log; // An array of all push/pull requests related to this memory

		size_t LogPullReq(); // Returns index of new request in m_log
		size_t LogPushReq(char newValue); // Returns index of new request in m_log
	};

	struct Operation
	{
		Operation(const std::string name, Variable* const* vars, const char* newValues);
		Operation(const std::string name, Variable* const* vars, const int varCount);
		Operation(const std::string name);

		std::string m_opName; // The name of the operation for displaying what's going on
		int m_time; // When the operation happened
		std::vector<Variable*> m_variables; // Pointers to memory interacted with in this operation (size 0 or nullptr for n/a)
		std::vector<size_t> m_relatedRequests; // Indexes in the Memory's m_log vector describing which request(s) this function is related to
	};

	struct Memory
	{
		std::vector<Variable> vars;
		Variable* Find(Address search);
		void Declare(Variable var);
	};

	struct Thread
	{
		std::vector<Operation> m_operations; // All operations used in this thread

		void TrackOp(const std::string name, Variable* const* vars, const char* newValues);
		void TrackOp(const std::string name, Variable* const* vars, const int varCount);
		void TrackOp(const std::string name);
	};

	struct Data
	{
		Thread producer;
		Thread consumer;
		Memory global;
		Memory producerLocal;
		Memory consumerLocal;

		void DeclareGlobal(Address var) { global.Declare(Variable(var)); }
		void DeclareProdLocal(Address var) { producerLocal.Declare(Variable(var)); }
		void DeclareConsLocal(Address var) { consumerLocal.Declare(Variable(var));}

		void Print();
	};
}

extern tracking::Data g_activity;

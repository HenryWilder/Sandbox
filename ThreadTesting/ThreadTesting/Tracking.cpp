#include "Tracking.h"

long long GetTime()
{
	return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

int GetTimeMS()
{
	return int(GetTime() / 1000000);
}

namespace tracking
{
	Request::Request()
	{
		m_time = GetTimeMS();
	}

	Variable::Variable(Address var)
	{
		m_address = var;
	}

	size_t Variable::LogPullReq()
	{
		m_log.push_back(Pull());
		return m_log.size() - 1;
	}

	size_t Variable::LogPushReq(char newValue)
	{
		m_log.push_back(Push(newValue));
		return m_log.size() - 1;
	}

	Operation::Operation(const std::string name, Variable* const* vars, const char* newValues)
	{
		m_time = GetTimeMS();
		m_opName = name;
		int v = 0;
		do {
			m_relatedRequests.push_back(vars[v]->LogPushReq(newValues[v]));
			m_variables.push_back(vars[v]);
		} while (newValues[v++]);
	}

	Operation::Operation(const std::string name, Variable* const* vars, const int varCount)
	{
		m_time = GetTimeMS();
		m_opName = name;
		for (int v = 0; v < varCount; ++v) {
			m_relatedRequests.push_back(vars[v]->LogPullReq());
			m_variables.push_back(vars[v]);
		}
	}

	Operation::Operation(const std::string name)
	{
		m_time = GetTimeMS();
		m_opName = name;
	}

	void Thread::TrackOp(const std::string name, Variable* const* vars, const char* newValues)
	{
		m_operations.push_back(Operation(name, vars, newValues));
	}
	void Thread::TrackOp(const std::string name, Variable* const* vars, const int varCount)
	{
		m_operations.push_back(Operation(name, vars, varCount));
	}
	void Thread::TrackOp(const std::string name)
	{
		m_operations.push_back(Operation(name));
	}

	void Data::Print()
	{

	}
	Variable* Memory::Find(Address search)
	{
		return nullptr;
	}
	void Memory::Declare(Address var)
	{
	}
}

tracking::Data g_activity;

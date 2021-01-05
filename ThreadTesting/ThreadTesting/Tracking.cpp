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
		size_t lastIndx = m_log.size();
		m_log.push_back(Pull());
		return lastIndx;
	}

	size_t Variable::LogPushReq(char newValue)
	{
		size_t lastIndx = m_log.size();
		m_log.push_back(Push(newValue));
		return lastIndx;
	}

	Operation::Operation(const std::string name, Variable* var, const char newValue)
	{
		m_time = GetTimeMS();
		m_opName = name;
		m_relatedRequest = (var->LogPushReq(newValue));
		m_variable = (var);
	}

	Operation::Operation(const std::string name, Variable* var)
	{
		m_time = GetTimeMS();
		m_opName = name;
		m_relatedRequest = (var->LogPullReq());
		m_variable = (var);
	}

	Operation::Operation(const std::string name)
	{
		m_time = GetTimeMS();
		m_opName = name;
	}

	Variable* Memory::Find(Address search)
	{
		for (int i = 0; i < vars.size(); ++i)
		{
			if (search == vars[i].m_address) return &(vars[i]);
		}
		return nullptr;
	}
	Variable* Memory::Declare(Address var)
	{
		vars.push_back(Variable(var));
		return &(vars.at(vars.size() - 1));
	}

	void Thread::TrackOpPush(const std::string name, Variable* var, const char newValue)
	{
		m_operations.push_back(Operation(name, var, newValue));
	}
	void Thread::TrackOpPull(const std::string name, Variable* var)
	{
		m_operations.push_back(Operation(name, var));
	}
	void Thread::TrackOp(const std::string name)
	{
		m_operations.push_back(Operation(name));
	}

	void Data::Print()
	{

	}
}

tracking::Data g_activity;

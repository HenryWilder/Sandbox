#pragma once
#include <fstream>
#include <vector>
#include "Functions.h"
#include "VersionConversion.h"

extern int g_version;

struct CustomVars
{
	std::vector<char> var_names; // Array of var names
	std::vector<int> var_vals; // Array of values (in same order as names)

	int _Locate(const char symbol) const
	{
		for (int i = 0; i < var_names.size(); ++i)
		{
			if (symbol == var_names.at(i)) return i;
		}
		return INT_MAX;
	}

	bool IsVar(const char symbol) const
	{
		for (char name : var_names)
		{
			if (symbol == name) return true;
		}
		return false;
	}
	void DeclareVar(const char symbol, const int value = 0)
	{
		var_names.push_back(symbol);
		var_vals.push_back(value);
	}
	const int GetVar(const char symbol) const
	{
		return var_vals.at(_Locate(symbol));
	}
	void SetVar(const char symbol, const int value)
	{
		var_vals.at(_Locate(symbol)) = value;
	}
};
extern CustomVars vars;

const char* FormatText(const char* text, ...);

namespace DEBUGMSGNS
{
	typedef enum {
		MSG_DEFAULT = 15,
		MSG_DEBUG_PRODUCT = 3,
		MSG_DEBUG = 8,
		MSG_DEBUG_LINEINDIC = 9,
		MSG_WARNING = 6,
		MSG_ERROR = 4,
	} MSG_TYPE;

	extern HANDLE hConsole;

	void DebugMessage(MSG_TYPE color, const char* text, ...);
}

extern std::ifstream file;

int Variable(bool get = true);
int Param();
int Function();

int InterpretFile(const char*);

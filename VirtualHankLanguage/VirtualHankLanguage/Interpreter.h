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

	bool IsVar(const char symbol) const;
	void DeclareVar(const char symbol, const int value = 0);
	const int GetVar(const char symbol) const;
	void SetVar(const char symbol, const int value);
};
extern CustomVars vars;
namespace DEBUGMSGNS
{
	typedef enum {
		MSG_DEFAULT = 7,
		MSG_DEBUG_EXTRA = 8,
		MSG_DEBUG = 1,
		MSG_DEBUG_PRODUCT = 3,
		MSG_DEBUG_LINEINDIC = 11,
		MSG_WARNING = 6,
		MSG_ERROR = 4,
	} MSG_TYPE; // Messages
	typedef enum {
		HLT_VAR = 11, // Variable
		HLT_LIT = 2, // Literal
		HLT_FUNC = 6, // Function
		HLT_STR = 13, // String
	} HLT_COLOR; // Highlighting

	extern HANDLE hConsole;

	extern const bool hideExtranious;
	extern const bool majorOnly;

	void DebugMessage(MSG_TYPE type, HLT_COLOR color, const char* text, ...);
	void DebugMessage(MSG_TYPE color, const char* text, ...);
}

extern std::ifstream file;

int Variable(bool get = true);
int Param();
int Function();

int InterpretFile(const char*);

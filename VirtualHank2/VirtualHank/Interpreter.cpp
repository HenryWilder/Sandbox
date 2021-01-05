<<<<<<< HEAD
#include "Interpreter.h"

int g_version = 0;

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

int InterpretFile(const char* filename)
{
	/***************************************************************************
	* 
	*   VERSION 0 KEY
	*   =============
	* 
	*   Syntax
	*   ======
	*   Variables
	*   : v [var symbol] [init value]	// Declaration (only integers are supported in this version)
	*   : [var symbol] [new value]		// Assignment
	*	ex:
	*	: v x 123			// Declaration
	*	: x 456				// Assignment
	* 
	*   Function call
	*   : [function symbol]														// No parameters
	*   : [function symbol] <v/l> [var symbol/value]							// 1 Parameter
	*   : [function symbol] <v/l> [var symbol/value] <v/l> [var symbol/value]   // 2 Parameters
	*   ex:
	*   : m l 123 l 456		// literals
	*   : m v x v y			// variables
	* 
	*   Keywords
	*   ========
	*   ':' Start of function
	* 
	*   'l' Literal value - conversion updates
	*       can modify this as a number
	* 
	*   'v' Variable - read this as a name of
	*       a variable
	* 
	*   Functions
	*   =========
	*   'm' Move mouse to position
	*       2 params (int x, int y)
	* 
	*   'c' Simulate mouse click
	*       0 params
	* 
	*   'e' End of document, stop
	*       0 params
	* 
	***************************************************************************/

	std::ifstream file = std::ifstream(filename);

	int versionNumber;
	file >> versionNumber;

	if (versionNumber < g_version)
	{
		// TODO: Backwards-compatibility & conversion updates
	}

	CustomVars vars;

	while (true) {
		file.ignore(256, ':'); // Locate the next line

		char symbol = 0;
		file >> symbol;

		if (symbol == 'e') break; // End of program

		// TODO: Function calls

		if (vars.IsVar(symbol))
		{
			// TODO: Implement variable assignment/declaration 
		}
	}

	file.close();
}
=======
#include "Interpreter.h"

int g_version = 0;

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

int InterpretFile(const char* filename)
{
	/***************************************************************************
	* 
	*   VERSION 0 KEY
	*   =============
	* 
	*   Syntax
	*   ======
	*   Variables
	*   : v [var symbol] [init value]	// Declaration (only integers are supported in this version)
	*   : [var symbol] [new value]		// Assignment
	*	ex:
	*	: v x 123			// Declaration
	*	: x 456				// Assignment
	* 
	*   Function call
	*   : [function symbol]														// No parameters
	*   : [function symbol] <v/l> [var symbol/value]							// 1 Parameter
	*   : [function symbol] <v/l> [var symbol/value] <v/l> [var symbol/value]   // 2 Parameters
	*   ex:
	*   : m l 123 l 456		// literals
	*   : m v x v y			// variables
	* 
	*   Keywords
	*   ========
	*   ':' Start of function
	* 
	*   'l' Literal value - conversion updates
	*       can modify this as a number
	* 
	*   'v' Variable - read this as a name of
	*       a variable
	* 
	*   Functions
	*   =========
	*   'm' Move mouse to position
	*       2 params (int x, int y)
	* 
	*   'c' Simulate mouse click
	*       0 params
	* 
	*   'e' End of document, stop
	*       0 params
	* 
	***************************************************************************/

	std::ifstream file = std::ifstream(filename);

	int versionNumber;
	file >> versionNumber;

	if (versionNumber < g_version)
	{
		// TODO: Backwards-compatibility & conversion updates
	}

	CustomVars vars;

	while (true) {
		file.ignore(256, ':'); // Locate the next line

		char symbol = 0;
		file >> symbol;

		if (symbol == 'e') break; // End of program

		// TODO: Function calls

		if (vars.IsVar(symbol))
		{
			// TODO: Implement variable assignment/declaration 
		}
	}

	file.close();
}
>>>>>>> 0641cdd506e8bd139e5868b54fb5c1396d32d3b8

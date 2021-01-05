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

typedef enum {
	MSG_DEFAULT = 15,
	MSG_DEBUG = 8,
	MSG_DEBUG_LINEINDIC = 9,
	MSG_WARNING = 6,
	MSG_ERROR = 4,
} MSG_TYPE;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void DebugMessage(MSG_TYPE color, const char* text, ...)
{
	int index = 0;
	char buffer[1024] = {};

	va_list args;
	va_start(args, text);
	vsprintf_s(buffer, text, args);
	va_end(args);

	SetConsoleTextAttribute(hConsole, color);
	printf(buffer);
	SetConsoleTextAttribute(hConsole, MSG_DEFAULT);
}

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
	*   ':' Start of function - Referred to as a "line"
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
	DebugMessage(MSG_DEBUG, "Executing Virtual Hank Instruction Document \"%s\" using version number %i.\n", filename, versionNumber);

	if (versionNumber < g_version)
	{
		// TODO: Backwards-compatibility & conversion updates
	}

	CustomVars vars;

	int line = 1; // For printing errors

	while (true) // For each line
	{
		++line; // I want the lines to be { 1, 2, 3, ... } instead of { 0, 1, 2, ... }
		DebugMessage(MSG_DEBUG_LINEINDIC, "Line %i:\n", line);

		file.ignore(256, ':'); // Locate the next line

		char keyword;
		char symbol;
		file >> keyword >> symbol;

		switch (keyword)
		{
		case 'f': // Function calls
		{
			switch (symbol)
			{
			case 'p':
			{
				std::string str;
				file >> str;
				printf("%s\n", str.c_str());
				DebugMessage(MSG_DEBUG, "Printed the string \"%s\".\n", str.c_str());
			}
				break;
			case 'e':
				DebugMessage(MSG_DEBUG, "Reached end of program at line %i.\n", line);
				goto END; // End of program
				break;
			}
		}
			break;

		case 'v': // Variable declares/assigns
		{
			char valType;
			file >> valType;
			int value;
			switch (valType)
			{
			case 'v': // Init with variable
			{
				char symbol1;
				file >> symbol1;
				if (vars.IsVar(symbol1))
				{
					value = vars.GetVar(symbol1);
					DebugMessage(MSG_DEBUG, "Using variable %c (currently %i) as initializer for variable %c.\n", symbol1, value, symbol);
				}
				else
				{
					DebugMessage(MSG_ERROR, "ERROR: UNDECLARED VARIABLE! Variable name %c was not previously declared in this document. Inline rvalue variable declaration is only allowed for functions.\n", symbol1);
					DebugMessage(MSG_ERROR, "Program exiting with failure.\n");
					return 2;
				}
			}
				break;
			default:
				file >> value;
				DebugMessage(MSG_WARNING, "WARNING: Inferred value type. Expected 'l', 'v', or 'f'. Proceeding with assumed 'l' resulting in value %i.\n", value);
				break;
			case 'l': // Init with literal
				file >> value;
				break;
			}

			if (vars.IsVar(symbol)) // Variable exists, change value
			{
				DebugMessage(MSG_DEBUG, "Set the variable %c with new value %i (Previously %i).\n", symbol, value, vars.GetVar(symbol));
				vars.SetVar(symbol, value);
			}
			else // Variable does not exist, initialize
			{
				vars.DeclareVar(symbol, value);
				DebugMessage(MSG_DEBUG, "Declared new variable %c with init value %i.\n", symbol, value);
			}
		}
			break;

		default:
			DebugMessage(MSG_ERROR, "ERROR: INVALID SYNTAX! Valid keyword could not be found for line %i.\n", line);
			DebugMessage(MSG_ERROR, "Expected 'f' or 'v', read '%c' instead.\n", keyword);
			DebugMessage(MSG_ERROR, "Program exiting with failure.\n");
			return 1;
			break;
		}
	}

END:

	file.close();

	return 0;
}

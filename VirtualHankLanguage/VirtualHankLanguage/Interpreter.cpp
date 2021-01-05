#include "Interpreter.h"

int g_version = 0;

int CustomVars::_Locate(const char symbol) const
{
	for (int i = 0; i < var_names.size(); ++i)
	{
		if (symbol == var_names.at(i)) return i;
	}
	return INT_MAX;
}
bool CustomVars::IsVar(const char symbol) const
{
	for (char name : var_names)
	{
		if (symbol == name) return true;
	}
	return false;
}
void CustomVars::DeclareVar(const char symbol, const int value)
{
	var_names.push_back(symbol);
	var_vals.push_back(value);
}
const int CustomVars::GetVar(const char symbol) const
{
	return var_vals.at(_Locate(symbol));
}
void CustomVars::SetVar(const char symbol, const int value)
{
	var_vals.at(_Locate(symbol)) = value;
}

CustomVars vars;

namespace DEBUGMSGNS
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	const bool hideExtranious = true;
	const bool majorOnly = false;

	void DebugMessage(MSG_TYPE type, HLT_COLOR color, const char* message, ...)
	{
#ifdef _RELEASE
		if (color == MSG_ERROR)
#endif
		{
			if (!(hideExtranious && type == MSG_DEBUG_EXTRA) && !(majorOnly && type == MSG_DEBUG))
			{
				char buffer[1024] = {};
				va_list args;
				va_start(args, message);
				vsprintf_s(buffer, message, args);
				va_end(args);

				SetConsoleTextAttribute(hConsole, color);
				printf(buffer);
				SetConsoleTextAttribute(hConsole, MSG_DEFAULT);
			}
		}
	}

	void DebugMessage(MSG_TYPE color, const char* message, ...)
	{
#ifdef _RELEASE
		if (color == MSG_ERROR)
#endif
		{
			if (!(hideExtranious && color == MSG_DEBUG_EXTRA) && !(majorOnly && color == MSG_DEBUG))
			{
				char buffer[1024] = {};
				va_list args;
				va_start(args, message);
				vsprintf_s(buffer, message, args);
				va_end(args);

				SetConsoleTextAttribute(hConsole, color);
				printf(buffer);
				if (color == MSG_ERROR) printf("Program exiting with failure.\n");
				SetConsoleTextAttribute(hConsole, MSG_DEFAULT);
			}
		}
	}

	void PrintLine()
	{
		int len = file.tellg();
		while (file.tellg() < std::ios_base::end) {
			while (isspace(file.peek()))
			{
				printf(" ");
				file >> std::ws;
			}
			if (file.peek() == ':') break;
			if (file.peek() == 'e')
			{
				DebugMessage(MSG_DEBUG_LINEINDIC, HLT_FUNC, "e");
				break;
			}
			char symbol = '#';
			file >> symbol;
			DebugMessage(MSG_DEBUG_LINEINDIC, HLT_FUNC, "%c", symbol);
		}
		file.seekg(len, std::ios_base::beg);
		DebugMessage(MSG_DEBUG_LINEINDIC, "\n");
	}
}

using namespace DEBUGMSGNS;

std::ifstream file;

// If "get" == true, do not assume we are setting the variable unless it is undeclared.
int Variable(bool get)
{
	char symbol;
	file >> symbol;
	DebugMessage(MSG_DEBUG_EXTRA, "Read the variable symbol \"%c\".\n", symbol);

	if (!(get && vars.IsVar(symbol)))
	{
		
		char keyword;
		file >> keyword;
		// Determine value to set the var to
		int value = INT_MAX;
		switch (keyword)
		{
		case 'v': // Initialize variable with another variable
		{
			file >> std::ws;
			char symbol1 = file.peek();
			value = Variable();
			DebugMessage(MSG_DEBUG, "Used the variable ");
			DebugMessage(MSG_DEBUG, HLT_VAR, "%c", symbol1);
			DebugMessage(MSG_DEBUG, " (currently ");
			DebugMessage(MSG_DEBUG, HLT_LIT, "%i", value);
			DebugMessage(MSG_DEBUG, ") as the value for variable ", value);
			DebugMessage(MSG_DEBUG, HLT_VAR, "%c", symbol);
			DebugMessage(MSG_DEBUG, ".\n");
			break;
		}
		case 'f': // Initialize variable with function return
			DebugMessage(MSG_WARNING, "WARNING: Initializing a variable with a function is not yet supported");
			// TODO: Set vars with functions
			break;
		default: // No keyword for initialization
			DebugMessage(MSG_WARNING, "WARNING: INVALID SYNTAX FOR DECLARING A VAR! Expected 'l', 'v', or 'f'. Read '%c' instead.\n", keyword);
			break;
		case 'l': // Initialize variable with literal
			file >> value;
			DebugMessage(MSG_DEBUG, "Used the literal ");
			DebugMessage(MSG_DEBUG, HLT_LIT, "%i", value);
			DebugMessage(MSG_DEBUG, " as the value for variable ");
			DebugMessage(MSG_DEBUG, HLT_VAR, "%c", symbol);
			DebugMessage(MSG_DEBUG, ".\n");
			break;
		}

		if (vars.IsVar(symbol)) // Variable exists
		{
			DebugMessage(MSG_DEBUG_PRODUCT, "Set the variable %c with new value %i (Previously %i).\n", symbol, value, vars.GetVar(symbol));
			vars.SetVar(symbol, value);
		}
		else // Variable does not exist, initialize
		{
			vars.DeclareVar(symbol, value);
			DebugMessage(MSG_DEBUG_PRODUCT, "Declared new variable %c with init value %i.\n", symbol, value);
		}
	}
	return vars.GetVar(symbol);
}

int Param()
{
	char keyword;
	file >> keyword;
	DebugMessage(MSG_DEBUG_EXTRA, "Read the parameter keyword \"%c\"\n", keyword);
	int value = INT_MAX;
	switch (keyword) {
	case 'l':
		file >> value;
		break;
	case 'v':
		value = Variable();
		break;
	case 'f': // TODO: Functions as parameters for other functions
		DebugMessage(MSG_WARNING, "WARNING: Using a function as a parameter for another function is not yet supported");
		break;
	default:
		DebugMessage(MSG_ERROR, "WARNING: Using a function as a parameter for another function is not yet supported");
		break;
	}
	return value;
}

int Function()
{
	char symbol;
	file >> symbol;
	DebugMessage(MSG_DEBUG_EXTRA, "Read the function \"%c\" symbol for ", symbol);

	switch (symbol)
	{
	case 'w': // Wait
	{
		DebugMessage(MSG_DEBUG_EXTRA, "Wait.\n");
		int time = Param();
		DebugMessage(MSG_DEBUG_PRODUCT, "Waiting %i ms...\n", time);
		Sleep(time);
		DebugMessage(MSG_DEBUG, "Finished wait.\n");
		break;
	}
	case 'p': // Print
	{
		DebugMessage(MSG_DEBUG_EXTRA, "Print.\n");
		std::string str;
		file >> str;
		printf("%s\n", str.c_str());
		DebugMessage(MSG_DEBUG_PRODUCT, "Printed the string ");
		DebugMessage(MSG_DEBUG_PRODUCT, HLT_STR, "\"%s\"", str.c_str());
		DebugMessage(MSG_DEBUG_PRODUCT, ".\n");
		break;
	}
	break;
	// TODO: Add more functions
	default:
		DebugMessage(MSG_DEBUG_EXTRA, "Undefined behavior.\n");
		break;
	}
	return 0;
}

int Control()
{
	char symbol;
	file >> symbol;
	switch (symbol)
	{
	case 'i': // If
		/*********************************
		*
		*	If statement syntax:
		*
		*	:c i <value> [0 = false, anything = true]
		*	{ :[code to execute] };
		*
		**********************************/
		//TODO
		Param();
		break;
	case 'f': // For
		/*********************************
		* 
		*	For loop syntax:
		* 
		*	:c f v [loop var] <value> [starting value] <value> [end value] <value> [amount to increment by each loop]
		*	{ :[code to repeat] };
		*	
		* 
		**********************************/
		Variable(false); // Variable
		Param(); // End value
		Param(); // Increment amount
		// TODO
		break;
	case 'w': // While
		// TODO
		break;
	case 'b': // Break
		file.ignore(256, ';');
		break;
	default:
		DebugMessage(MSG_ERROR, "ERROR: CONTROL KEYWORD USED WITHOUT VALID CONTROL SYMBOL.\n");
	}
	return 0;
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
	* 
	*   Keywords
	*   ========
	*   ':' Start of function - Referred to as a "line" in debug
	*
	*   'l' Literal	 - read as an integer value
	*
	*   'v' Variable - read as the name of a variable
	*	Note: This will declare a new variable if none with the specified name exists. The variable must be initialized in this case.
	* 
	*	'f' Function - read as the name of a function
	* 
	*	'c' Control - read as the name of a control statement
	* 
	*	'e' Signifies the end of the document
	*
	*   Functions in the current version
	*   ================================
	*   'm' Move mouse to position
	*       2 params (int x, int y)
	*
	*   'c' Simulate mouse click
	*       0 params
	* 
	*	Variables
	*	=========
	*	Note: only integer vars are supported in this version.
	*   :v [var symbol] <v/l> [init value]	// Declaration
	*   :v [var symbol] <v/l> [new value]	// Assignment
	*	:f [function symbol] v [var symbol] // Var as parameter
	*	ex:
	*	:v x 123			// Declaration
	*	:v x 456			// Assignment
	*	:f w v x			// Parameter
	*
	*   Function call
	*   :[function symbol]														// No parameters
	*   :[function symbol] <v/l> [var symbol/value]								// 1 Parameter
	*   :[function symbol] <v/l> [var symbol/value] <v/l> [var symbol/value]	// 2 Parameters
	*   ex:
	*   :m l 123 l 456		// literals
	*   :m v x v y			// variables
	*	Note: string parameters cannot use variables (all vars are ints) and expect a literal string without any keywords.
	*
	***************************************************************************/

	file = std::ifstream(filename); // Set the file

	int versionNumber;
	file >> versionNumber;
	DebugMessage(MSG_DEBUG, "Executing Virtual Hank Instruction Document \"%s\" using version number %i.\n", filename, versionNumber);

	if (versionNumber < g_version)
	{
		UpdateFile(filename, versionNumber);
	}

	int line = 0;

	while (true) // For each line
	{
		file.ignore(256, ':');
		++line; // I want the lines to be { 1, 2, 3, ... } instead of { 0, 1, 2, ... }
		DebugMessage(MSG_DEBUG_LINEINDIC, "\n[LINE %i]: ", line);
		PrintLine();

		char keyword;
		file >> keyword;

		DebugMessage(MSG_DEBUG_EXTRA, "Read the keyword \"%c\" for ", keyword);

		if (keyword == 'e') break; // Exit keyword

		switch (keyword)
		{
		case 'v': // Variable declares/assigns
			DebugMessage(MSG_DEBUG_EXTRA, "Variable declaration/assignment.\n");
			Variable(false);
			break;
		case 'f': // Function calls
			DebugMessage(MSG_DEBUG_EXTRA, "Function call.\n");
			Function();
			break;
		case 'c':
			DebugMessage(MSG_DEBUG_EXTRA, "Control statement.\n");
			Control();
			break;
		case 'n':
			// TODO: namespace case
		default:
			DebugMessage(MSG_DEBUG_EXTRA, "Undefined behavior.\n");
			DebugMessage(MSG_ERROR, "ERROR: INVALID SYNTAX! Line keyword could not be found. Expected 'f' or 'v'. Read '%c' instead.\n", keyword);
			return 1;
			break;
		}
	}

	DebugMessage(MSG_DEBUG, "End of document.\n");

	file.close();

	return 0;
}

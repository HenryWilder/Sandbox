#include "Interpreter.h"

int g_version = 1;
CustomVars vars;

const char* FormatText(const char* text, ...)
{
	char buffer[1024] = {};
	va_list args;
	va_start(args, text);
	vsprintf_s(buffer, text, args);
	va_end(args);
	return buffer;
}

namespace DEBUGMSGNS
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	void DebugMessage(MSG_TYPE color, const char* message, ...)
	{
	#ifdef _RELEASE
		if (color == MSG_ERROR)
	#endif
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

using namespace DEBUGMSGNS;

std::ifstream file;

// If "get" == true, do not assume we are setting the variable unless it is undeclared.
int Variable(bool get)
{
	char symbol;
	file >> symbol;
	DebugMessage(MSG_DEBUG, "Read the variable symbol \"%c\".\n", symbol);

	if (!(get && vars.IsVar(symbol)))
	{
		char keyword;
		file >> keyword;
		// Determine value to set the var to
		int value;
		switch (keyword) // TODO: Initialize variable with function result
		{
		case 'l': // Initialize variable with literal
			file >> value;
			DebugMessage(MSG_DEBUG, "Using literal %i as the value for variable %c.\n", value, symbol);
			break;
		case 'v': // Initialize variable with another variable
			value = Variable();
			DebugMessage(MSG_DEBUG, "Using a variable with value %i as the value for variable %c.\n", value, symbol);
			break;
		case 'f': // Initialize variable with function return
			DebugMessage(MSG_WARNING, "WARNING: Initializing a variable with a function is not yet supported");
			// TODO: Set vars with functions
		default: // No keyword for initialization
			DebugMessage(MSG_ERROR, "ERROR: INVALID SYNTAX FOR DECLARING A VAR! Expected 'l', 'v', or 'f'. Read '%c' instead.\n", keyword);
			value = INT_MAX;
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
	DebugMessage(MSG_DEBUG, "Read the parameter keyword \"%c\"\n", keyword);
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
	}
	return value;
}

int Function()
{
	char symbol;
	file >> symbol;
	DebugMessage(MSG_DEBUG, "Read the function \"%c\" symbol for ", symbol);

	switch (symbol) // TODO: Add more functions
	{
	case 'w': // Wait
	{
		DebugMessage(MSG_DEBUG, "Wait.\n");
		int time = Param();
		DebugMessage(MSG_DEBUG_PRODUCT, "Waiting %i ms...\n", time);
		Sleep(time);
		DebugMessage(MSG_DEBUG, "Finished wait.\n");
		break;
	}
	case 'p': // Print
	{
		DebugMessage(MSG_DEBUG, "Print.\n");
		std::string str;
		file >> str;
		printf("%s\n", str.c_str());
		DebugMessage(MSG_DEBUG_PRODUCT, "Printed the string \"%s\".\n", str.c_str());
		break;
	}
	break;
	default:
		DebugMessage(MSG_DEBUG, "Undetermined behavior.\n");
		break;
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
	*   Variables
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
	***************************************************************************/

	file = std::ifstream(filename); // Set the file

	int versionNumber;
	file >> versionNumber;
	DebugMessage(MSG_DEBUG, "Executing Virtual Hank Instruction Document \"%s\" using version number %i.\n", filename, versionNumber);

	if (versionNumber < g_version)
	{
		UpdateFile(filename, versionNumber);
	}

	int line = 1; // For printing errors

	while (true) // For each line
	{
		++line; // I want the lines to be { 1, 2, 3, ... } instead of { 0, 1, 2, ... }
		DebugMessage(MSG_DEBUG_LINEINDIC, "Line %i:\n", line);

		file.ignore(256, ':'); // Jump to the next line

		char keyword;
		file >> keyword;

		DebugMessage(MSG_DEBUG, "Read the keyword \"%c\" for ", keyword);

		if (keyword == 'e') break; // Exit keyword

		switch (keyword)
		{
		case 'f': // Function calls
			DebugMessage(MSG_DEBUG, "Function call.\n");
			Function();
			break;
		case 'v': // Variable declares/assigns
			DebugMessage(MSG_DEBUG, "Variable declaration/assignment.\n");
			Variable(false);
			break;
		default:
			DebugMessage(MSG_DEBUG, "Undefined behavior.\n");
			DebugMessage(MSG_ERROR, "ERROR: INVALID SYNTAX! Line keyword could not be found. Expected 'f' or 'v'. Read '%c' instead.\n", keyword);
			return 1;
			break;
		}
	}

	DebugMessage(MSG_DEBUG, "End of document.\n");

	file.close();

	return 0;
}

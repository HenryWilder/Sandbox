#include "Interpreter.h"

int g_version = 0;

std::ifstream file;

const FunctionSymbol g_FunctionList[] = {
	"Print",
	"Wait",
	"Click"
};
enum class FuncToken : char
{
	Print = 0,
	Wait,
	Click,
	ERR,
};
FuncToken Tokenize(FunctionSymbol function)
{
	for (int i = 0; i < sizeof(g_FunctionList); ++i) { if (function == g_FunctionList[i]) return FuncToken(i); }
	return FuncToken::ERR;
}

const CtrlSymbol g_CtrlStatements[] = {
	"if",
	"for",
	"while",
	"break",
};
enum class CtrlToken : char
{
	ctrl_if = 0,
	ctrl_for,
	ctrl_while,
	ctrl_break,
	ERR,
};
CtrlToken Tokenize(CtrlSymbol statement)
{
	for (int i = 0; i < sizeof(g_CtrlStatements); ++i) { if (statement == g_CtrlStatements[i]) return CtrlToken(i); }
	return CtrlToken::ERR;
}

const KeywordSymbol g_Keywords[] = {
	"var",
	"lit",
	"func",
	"ctrl",
	"end",
	"namspc" // TODO
};
enum class KeywordToken : char
{
	var = 0,
	lit,
	func,
	ctrl,
	end,
	namspc,
	ERR,
};
KeywordToken Tokenize(KeywordSymbol keyword)
{
	for (int i = 0; i < sizeof(g_Keywords); ++i) { if (keyword == g_Keywords[i]) return KeywordToken(i); }
	return KeywordToken::ERR;
}

size_t CustomVars::_Locate(VariableSymbol symbol) const
{
	for (size_t i = 0; i < var_names.size(); ++i)
	{
		if (StringEquals(symbol.c_str(), var_names.at(i).c_str())) return i;
	}
	return var_names.size();
}
bool CustomVars::IsVar(VariableSymbol symbol) const
{
	return (std::find(var_names.begin(), var_names.end(), symbol) != var_names.end());
}
void CustomVars::DeclareVar(VariableSymbol symbol, const int value)
{
	var_names.push_back(symbol);
	var_vals.push_back(value);
}
const int CustomVars::GetVar(VariableSymbol symbol) const
{
	return var_vals.at(_Locate(symbol));
}
void CustomVars::SetVar(VariableSymbol symbol, const int value)
{
	var_vals.at(_Locate(symbol)) = value;
}

CustomVars vars;

namespace DEBUGMSGNS
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	const bool hideExtranious = true;
	const bool majorOnly = false;

	void DebugMessageHLT(MSG_TYPE type, HLT_COLOR color, const char* message, ...)
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
	/*
	void DebugMessage(MSG_TYPE color, const std::string message, const DebugDataBundle& data)
	{
		int i = 0;
		size_t sectionOffset = 0;
		size_t sectionLength = 0;
		SetConsoleTextAttribute(hConsole, color);
		while (sectionOffset + sectionLength < message.rfind('%'))
		{
			sectionLength = message.find('%', sectionOffset) - sectionOffset;
			printf("%s", message.substr(sectionOffset, sectionLength).c_str());
			sectionOffset += sectionLength;
			data.m_dataChunks.at(i++)->PrintData();
			SetConsoleTextAttribute(hConsole, color);
		}
		printf("%s", message.substr(sectionOffset, message.length() - sectionOffset).c_str());
		SetConsoleTextAttribute(hConsole, MSG_DEFAULT);
	}
	*/

	bool PrintLine(bool returnToStart)
	{
		std::streampos pos = file.tellg();
		bool endOfDoc = false;
		while (true)
		{
			char next = file.peek();
			if (next == ':') break; // Stop printing at start of next line

			// Consume whitespace
			while (next == ' ' || next == '\n')
			{
				file >> std::ws;
				next = file.peek();
				printf(" ");
			}

			if (next == ':') break; // Stop printing at start of next line

			// Get next symbol
			std::string symbol;
			file >> symbol;
			
			// End of document will have no newline, so we need to exit the loop when we reach it
			if (StringEquals(symbol.c_str(), "end"))
			{
				DebugMessageHLT(MSG_DEBUG_LINEINDIC, HLT_FUNC, "end");
				endOfDoc = true;
				break;
			}
			else
			{
				HLT_COLOR highlight = HLT_VAR;
				if (Tokenize(KeywordSymbol(symbol)) != KeywordToken::ERR) highlight = HLT_FUNC;
				if (Tokenize(FunctionSymbol(symbol)) != FuncToken::ERR) highlight = HLT_LIT;
				if (Tokenize(CtrlSymbol(symbol)) != CtrlToken::ERR) highlight = HLT_STR;

				// Print symbol
				DebugMessageHLT(MSG_DEBUG_LINEINDIC, highlight, "%s", symbol.c_str());
			}
		}

		if (returnToStart) file.seekg(pos, std::ios_base::beg);
		DebugMessage(MSG_DEBUG_LINEINDIC, "\n");
		return endOfDoc;
	}
}

using namespace DEBUGMSGNS;

// If "get" == true, do not assume we are setting the variable unless it is undeclared.
int Variable(bool get)
{
	VariableSymbol symbol;
	file >> symbol;
	DebugMessage(MSG_DEBUG_EXTRA, "Read the variable symbol \"%s\", which has ", symbol.c_str());
	if (vars.IsVar(symbol)) DebugMessage(MSG_DEBUG_EXTRA, "a value of %i.\n", vars.GetVar(symbol));
	else DebugMessage(MSG_DEBUG_EXTRA, "not yet been declared. Now declaring it as a new var.\n");

	if (!(get && vars.IsVar(symbol)))
	{
		
		KeywordSymbol keyword;
		file >> keyword;
		DebugMessage(MSG_DEBUG_EXTRA, "Read the keyword %s, ", keyword.c_str());
		KeywordToken keywordTokn = Tokenize(keyword);
		// Determine value to set the var to
		int value = INT_MAX;
		switch (keywordTokn)
		{
		case KeywordToken::var: // Initialize variable with another variable
		{
			file >> std::ws;
			VariableSymbol symbol1;
			// Look at the next variable symbol without consuming it
			{
				std::streampos pos = file.tellg();
				file >> symbol1;
				DebugMessage(MSG_DEBUG_EXTRA, "followed by the Variable symbol, \"%s\".\n", symbol1.c_str());
				file.seekg(pos);
			}
			value = Variable();

			/*
			DebugDataBit<VariableSymbol> s1(symbol1);
			DebugDataBit<int> v(value);
			DebugDataBit<VariableSymbol> s(symbol);
			DebugDataBundle bundle({ &s1,&v,&s });
			DebugMessage(MSG_DEBUG, "Used the variable % (currently %) as the value for variable %", DebugDataBundle(&s1,&v,&s));

			PrintDebugMSG(MSG_DEBUG, "Used the variable % (currently %) as the value for variable %", symbol1, value, symbol);
			*/

			DebugMessage(MSG_DEBUG, "Used the variable ");
			DebugMessageHLT(MSG_DEBUG, HLT_VAR, "%s", symbol1.c_str());
			DebugMessage(MSG_DEBUG, " (currently ");
			DebugMessageHLT(MSG_DEBUG, HLT_LIT, "%i", value);
			DebugMessage(MSG_DEBUG, ") as the value for variable ");
			DebugMessageHLT(MSG_DEBUG, HLT_VAR, "%s", symbol.c_str());
			DebugMessage(MSG_DEBUG, ".\n");
			break;
		}
		case KeywordToken::func: // Initialize variable with function return
			DebugMessage(MSG_DEBUG_EXTRA, "for Function.\n");
			DebugMessage(MSG_WARNING, "WARNING: Initializing a variable with a function is not yet supported");
			// TODO: Set vars with functions
			break;
		case KeywordToken::lit: // Initialize variable with literal
			file >> value;
			DebugMessage(MSG_DEBUG_EXTRA, "followed by the value %i.\n", value);
			DebugMessage(MSG_DEBUG, "Used the literal ");
			DebugMessageHLT(MSG_DEBUG, HLT_LIT, "%i", value);
			DebugMessage(MSG_DEBUG, " as the value for variable ");
			DebugMessageHLT(MSG_DEBUG, HLT_VAR, "%s", symbol.c_str());
			DebugMessage(MSG_DEBUG, ".\n");
			break;
		default: // No keyword for initialization
			DebugMessage(MSG_DEBUG_EXTRA, "for an error to occur.\n");
			DebugMessage(MSG_WARNING, "WARNING: INVALID SYNTAX FOR DECLARING A VAR! Expected 'lit', 'var', or 'func'. Read '%s' instead.\n", keyword.c_str());
			break;
		}

		if (vars.IsVar(symbol)) // Variable exists
		{
			DebugMessage(MSG_DEBUG_PRODUCT, "Set the variable %s with new value %i (Previously %i).\n", symbol.c_str(), value, vars.GetVar(symbol));
			vars.SetVar(symbol, value);
		}
		else // Variable does not exist, initialize
		{
			vars.DeclareVar(symbol, value);
			DebugMessage(MSG_DEBUG_PRODUCT, "Declared new variable %s with init value %i.\n", symbol.c_str(), value);
		}
	}
	DebugMessage(MSG_DEBUG_EXTRA, "Returning the value of %s (currently %i).\n", symbol.c_str(), vars.GetVar(symbol));
	return vars.GetVar(symbol);
}

int Param()
{
	KeywordSymbol keyword;
	file >> keyword;
	DebugMessage(MSG_DEBUG_EXTRA, "Read the parameter keyword \"%s\"\n", keyword.c_str());
	KeywordToken keywordTokn = Tokenize(keyword);
	int value = INT_MAX;
	switch (keywordTokn) {
	case KeywordToken::lit:
		file >> value;
		DebugMessage(MSG_DEBUG_EXTRA, "Used the literal value %i as a parameter\n", value);
		break;
	case KeywordToken::var:
		value = Variable();
		break;
	case KeywordToken::func: // TODO: Functions as parameters for other functions
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
	FunctionSymbol symbol;
	file >> symbol;
	DebugMessage(MSG_DEBUG_EXTRA, "Read the function \"%s\" symbol for ", symbol.c_str());

	FuncToken token = Tokenize(symbol);

	switch (token)
	{
	case FuncToken::Wait: // Wait
	{
		DebugMessage(MSG_DEBUG_EXTRA, "Wait.\n");
		int time = Param();
		DebugMessage(MSG_DEBUG_PRODUCT, "Waiting %i ms...\n", time);
		Sleep(time);
		DebugMessage(MSG_DEBUG, "Finished wait.\n");
		break;
	}
	case FuncToken::Print: // Print
	{
		DebugMessage(MSG_DEBUG_EXTRA, "Print.\n");
		std::string str;
		file >> str;
		printf("%s\n", str.c_str());
		DebugMessage(MSG_DEBUG_PRODUCT, "Printed the string ");
		DebugMessageHLT(MSG_DEBUG_PRODUCT, HLT_STR, "\"%s\"", str.c_str());
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

// TODO
int Control()
{
	CtrlSymbol symbol;
	file >> symbol;
	CtrlToken ctrlSymbol = Tokenize(symbol);
	switch (ctrlSymbol)
	{
	case CtrlToken::ctrl_if: // If
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
	case CtrlToken::ctrl_for: // For
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
	case CtrlToken::ctrl_while: // While
		// TODO
		break;
	case CtrlToken::ctrl_break: // Break
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
	*   "lit" Literal	 - read as an integer value
	*
	*   "var" Variable - read as the name of a variable
	*	Note: This will declare a new variable if none with the specified name exists. The variable must be initialized in this case.
	* 
	*	"func" Function - read as the name of a function
	* 
	*	"ctrl" Control - read as the name of a control statement
	* 
	*	"end" Signifies the end of the document
	*
	*   Functions in the current version
	*   ================================
	*   "Mouse"
	*		Move mouse to position (x, y)
	*       2 params (int x, int y)
	*
	*   "Click"
	*		Simulate mouse click
	*       0 params
	* 
	*	"Wait"
	*		Wait x milliseconds
	*		1 param (int x)
	* 
	*	"Print"
	*		Print the string to the console
	*		1 param (const char* string)
	* 
	*	Variables
	*	=========
	*	Note: only integer vars are supported in this version.
	*   :var [var symbol] <var/lit> [init value]	// Declaration
	*   :var [var symbol] <var/lit> [new value]		// Assignment
	*	:func [function symbol] var [var symbol]	// Var as parameter
	*	ex:
	*	:var x lit 123			// Declaration
	*	:var x lit 456			// Assignment
	*	:func w var x			// Parameter
	*
	*   Function call
	*   :[function symbol]																// No parameters
	*   :[function symbol] <var/lit> [var symbol/value]									// 1 Parameter
	*   :[function symbol] <var/lit> [var symbol/value] <var/lit> [var symbol/value]	// 2 Parameters
	*   ex:
	*   :Mouse lit 123 lit 456		// literals
	*   :Mouse var x var y			// variables
	*   :Print "Foo"				// string
	*	Note: string parameters cannot use variables (all vars are ints) and expect
	*	a string without the lit keywords.
	*
	***************************************************************************/

	file = std::ifstream(filename); // Set the file

	// Print whole document
	{
		printf("%s\n", filename);
		int line = 1;
		while (true)
		{
			bool end = PrintLine(false);
			file.ignore(256, ':');
			if (end) break;
			else printf("[ %i ] :", line++);
		}
		printf("\n");
		file.seekg(0);
	}

	file.ignore(256,' ');
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

		KeywordSymbol keyword;
		file >> keyword;

		DebugMessage(MSG_DEBUG_EXTRA, "Read the keyword \"%s\" for ", keyword.c_str());

		KeywordToken keywordTokn = Tokenize(keyword);

		if (keywordTokn == KeywordToken::end) break; // Exit keyword
		else
		{
			switch (keywordTokn)
			{
			case KeywordToken::var: // Variable declares/assigns
				DebugMessage(MSG_DEBUG_EXTRA, "Variable declaration/assignment.\n");
				Variable(false);
				break;
			case KeywordToken::func: // Function calls
				DebugMessage(MSG_DEBUG_EXTRA, "Function call.\n");
				Function();
				break;
			case KeywordToken::ctrl:
				DebugMessage(MSG_DEBUG_EXTRA, "Control statement.\n");
				Control();
				break;
			case KeywordToken::namspc:
				// TODO: namespace case
			default:
				DebugMessage(MSG_DEBUG_EXTRA, "Undefined behavior.\n");
				DebugMessage(MSG_ERROR, "ERROR: INVALID SYNTAX! Line keyword could not be found. Expected 'func' or 'var'. Read '%s' instead.\n", keyword);
				return 1;
				break;
			}
		}
	}

	DebugMessage(MSG_DEBUG, "End of document.\n");

	file.close();

	return 0;
}

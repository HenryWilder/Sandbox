#include "Interpreter.h"
#include <unordered_map>

int g_version = 0;

std::ifstream file;

// Todo: fill with function pointers to functions
const std::unordered_map<FunctionSymbol, FunctionPointer> g_FunctionSymbolList{
	{ "Print",		FunctionPointer{} },
	{ "Wait",		FunctionPointer{} },
	{ "MouseTo",	FunctionPointer{} },
	{ "Click",		FunctionPointer{} },
	{ "Keypress",	FunctionPointer{} },
	{ "Open",		FunctionPointer{} },
};

const FuncBase g_FunctionClassList[] =
{
	FuncBase(), // ERR
	FuncBase(),	// Print
	FuncBase(),	// Wait
	FuncBase(),	// MouseTo
	FuncBase(),	// Click
	FuncBase(),	// Keypress
	FuncBase(),	// Open

};
enum class FuncToken : char
{
	ERR = NULL,
	Print,
	Wait,
	MouseTo,
	Click,
	Keypress,
	Open,
};
FuncToken Tokenize(FunctionSymbol function)
{
	for (int i = 0; i < sizeof(g_FunctionSymbolList); ++i) { if (function == g_FunctionSymbolList[i]) return FuncToken(i); }
	return FuncToken::ERR;
}
bool ValidFunction(const FunctionSymbol function) { return (Tokenize(function) != FuncToken::ERR); }
bool ValidFunction(const std::string function) { return ValidFunction(FunctionSymbol(function)); }
bool ValidFunction(const char* function) { return ValidFunction(FunctionSymbol(function)); }

const KeywordSymbol g_Keywords[] =
{
	"\\^%#&__KEYWORD_TYPE_ERR__",
	"var",
	"lit",
	"func",
	"ctrl",
	"end",
	"namspc",
};
enum class KeywordToken : char
{
	ERR = NULL,
	var,
	lit,
	func,
	ctrl,
	end,
	namspc,
};
KeywordToken Tokenize(const KeywordSymbol keyword) // BROKEN: This keeps coming back for some reason... Says keyword is an "error-type" or something.
{
	for (int i = 0; i < sizeof(g_Keywords); ++i) { if (keyword == g_Keywords[i]) return KeywordToken(i); }
	return KeywordToken::ERR;
}
bool ValidKeyword(const KeywordSymbol keyword) { return (Tokenize(keyword) != KeywordToken::ERR); }
bool ValidKeyword(const std::string keyword) { return ValidKeyword(KeywordSymbol(keyword)); }
bool ValidKeyword(const char* keyword) { return ValidKeyword(KeywordSymbol(keyword)); }

const CtrlSymbol g_CtrlStatements[] =
{
	"\\$%^@__CTRL_TYPE_ERR__",
	"if",
	"for",
	"while",
	"break",
};
enum class CtrlToken : char
{
	ERR = NULL,
	ctrl_if,
	ctrl_for,
	ctrl_while,
	ctrl_break,
};
CtrlToken Tokenize(const CtrlSymbol statement)
{
	for (int i = 0; i < sizeof(g_CtrlStatements); ++i) { if (statement == g_CtrlStatements[i]) return CtrlToken(i); }
	return CtrlToken::ERR;
}
bool ValidCtrl(const CtrlSymbol statement) { return (Tokenize(statement) != CtrlToken::ERR); }
bool ValidCtrl(const std::string statement) { return ValidCtrl(CtrlSymbol(statement)); }
bool ValidCtrl(const char* statement) { return ValidCtrl(CtrlSymbol(statement)); }

bool IsNumber(const char first)
{
	if (first == '-') return true;
	for (char numChar = '0'; numChar <= '9'; ++numChar) {
		if (first == numChar) return true;
	}
	return false;
}
bool IsNumber(const char* input) { return IsNumber(input[0]); }
bool IsNumber(const std::string input) { return IsNumber(input.at(0)); }

size_t CustomVars::_Locate(const VariableSymbol& symbol) const
{
	for (size_t i = 0; i < m_vars.size(); ++i)
	{
		if (StringEquals(symbol.c_str(), m_vars.at(i)->c_str())) return i;
	}
	return m_vars.size();
}
bool CustomVars::IsVar(const VariableSymbol& symbol) const
{
	return (_Locate(symbol) != m_vars.size());
}

// SPAGHETTI: This code is directly ripped from the deconstructor. Let's try and consolidate them.
void CustomVars::ClearAll()
{
	for (VariableSymbol*& var : m_vars) { delete var; }
	if (m_vars.size()) m_vars.clear();
}

CustomVars vars;

// Seems a bit odd that there
bool ValidVar(const VariableSymbol& varName) { return vars.IsVar(varName); }
bool ValidVar(const std::string& varName) { return ValidVar(VariableSymbol(varName)); }
bool ValidVar(const char*& varName) { return ValidVar(VariableSymbol(varName)); }

enum class SymbolType
{
	/**********************************
	*
	*	Explaination for order
	*	======================
	*	0)	ERR: null, so that errors will always return *false*
	*	1)	String literal: Extremely easy to test. Just check if the
	*		first char is a quotemark.
	*	2)	Value literal: Almost as quick to test: just check if the
	*		first char is '-', '0', '1', '2', '3', '4', '5', '6', '7', '8', or '9'.
	*	3)	Keyword: Keywords are extremely important, and are more likely to be
	*		put before any other type of symbol.
	*	4)	Control: There is a very short list of control statements.
	*	5)	Function: The list of functions is predetermined
	*		before the program ever runs.
	*	6)	Variable: Vars are last, because they are user-defined names and
	*		could be anything. They also reside in a dynamic array.
	*
	**********************************/

	ERR = NULL,		// Anything not fitting into the following lists
	StringLiteral,	// Uses quotemarks
	ValueLiteral,	// Starts with a number
	Keyword,		// In list of keywords
	Control,		// In list of control keywords
	Function,		// In list of functions
	Variable,		// In list of existing vars
};
SymbolType FindSymbolType(std::string symbol)
{
	if (symbol.at(0) == '\"') return SymbolType::StringLiteral;
	else if (IsNumber(symbol)) return SymbolType::ValueLiteral;
	else if (ValidKeyword(symbol)) return SymbolType::Keyword;
	else if (ValidCtrl(symbol)) return SymbolType::Control;
	else if (ValidFunction(symbol)) return SymbolType::Function;
	else if (ValidVar(symbol)) return SymbolType::Variable;
	else return SymbolType::ERR;
}

namespace DEBUGMSGNS
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	enum class MSGTYPE // Messages
	{
		MSG_DEFAULT = 7,
		MSG_DEBUG_EXTRA = 8,
		MSG_DEBUG = 1,
		MSG_DEBUG_PRODUCT = 3,
		MSG_DEBUG_LINEINDIC = 11,
		MSG_WARNING = 14,
		MSG_WARNING_MINOR = 6,
		MSG_ERROR = 4,
	};
	enum class HLT // Highlighting
	{
		HLT_NONE = 7,
		HLT_VAR = 11,	// Variable
		HLT_KEYW = 1,	// Keyword
		HLT_CTRL = 4,	// Control
		HLT_LIT = 2,	// Literal
		HLT_FUNC = 6,	// Function
		HLT_STR = 13,	// String
	};

	const bool hideExtranious = false;
	const bool majorOnly = false;

	// @HACK: The debug message functions complain about being "ambiguous" sometimes due to their variadic nature.
	// Usually when that happens I just [ctrl + x] cut the function name from the definition and paste it back in when the squiggly underline goes away.
	// IDK if that's the best way to do it, but that usually seems to fix the issue.
	void DebugMessageHLT(MSGTYPE type, HLT color, const char* message, ...)
	{
#ifdef _RELEASE
		if (color == MSGTYPE::MSG_ERROR)
#endif
		{
			if (!(hideExtranious && type == MSGTYPE::MSG_DEBUG_EXTRA) && !(majorOnly && type == MSGTYPE::MSG_DEBUG))
			{
				char buffer[1024] = {};
				va_list args;
				va_start(args, message);
				vsprintf_s(buffer, message, args);
				va_end(args);

				SetConsoleTextAttribute(hConsole, (int)color);
				printf(buffer);
				SetConsoleTextAttribute(hConsole, (int)MSGTYPE::MSG_DEFAULT);
			}
		}
	}

	void DebugMessage(MSGTYPE color, const char* message, ...)
	{
#ifdef _RELEASE
		if (color == MSGTYPE::MSG_ERROR)
#endif
		{
			if (!(hideExtranious && color == MSGTYPE::MSG_DEBUG_EXTRA) && !(majorOnly && color == MSGTYPE::MSG_DEBUG))
			{
				char buffer[1024] = {};
				va_list args;
				va_start(args, message);
				vsprintf_s(buffer, message, args);
				va_end(args);

				SetConsoleTextAttribute(hConsole, (int)color);
				printf(buffer);
				if (color == MSGTYPE::MSG_ERROR) printf("Program exiting with failure.\n");
				SetConsoleTextAttribute(hConsole, (int)MSGTYPE::MSG_DEFAULT);
			}
		}
	}

	// BROKEN: Was gonna make a more reasonable way of doing debug messages than with variadic templates, but uh...
	/*
	void DebugMessage(MSGTYPE color, const std::string message, const DebugDataBundle& data)
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
		SetConsoleTextAttribute(hConsole, MSGTYPE::MSG_DEFAULT);
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
			while (isspace(next))
			{
				file >> std::ws;
				next = file.peek();
				printf(" ");
			}

			if (next == ':') break; // Stop printing at start of next line

			if (next == '#')
			{
				if (!returnToStart) // TODO: Can we figure out why this needs to be inverted?
				{
					printf(" "); // TODO: I would love if the number of tabs in the document matched the number of tabs in the output.
					SetConsoleTextAttribute(hConsole, 8);
					while (true) // TODO: Maybe this would be better as a function?
					{
						next = file.peek();
						if (next == ':') break; // Stop printing at start of next line

						// Consume whitespace
						while (isspace(next))
						{
							file >> std::ws;
							next = file.peek();
							printf(" ");
						}

						if (next == ':') break; // Stop printing at start of next line
						std::string comment;
						file >> comment;
						printf("%s", comment.c_str());
					}
				}
				break;
			}

			// Get next symbol
			std::string symbol;
			file >> symbol;
			
			// End of document will have no newline, so we need to exit the loop when we reach it
			if (StringEquals(symbol.c_str(), "end"))
			{
				DebugMessageHLT(MSGTYPE::MSG_DEBUG_LINEINDIC, HLT::HLT_FUNC, "end");
				endOfDoc = true;
				break;
			}
			else
			{
				HLT highlight = HLT::HLT_NONE;
				switch (FindSymbolType(symbol))
				{
				case SymbolType::Keyword:
					
					if (Tokenize((KeywordSymbol)(symbol)) == KeywordToken::var)
					{
						std::streampos preVar = file.tellg();
						VariableSymbol varName;
						file >> varName;
						if (!ValidVar(varName)) vars.DeclareVar(varName);
						file.seekg(preVar);
					}
					
					highlight = HLT::HLT_KEYW;
					break;
				case SymbolType::Function:
					highlight = HLT::HLT_FUNC;
					break;
				case SymbolType::ValueLiteral:
					highlight = HLT::HLT_LIT;
					break;
				case SymbolType::Control:
					highlight = HLT::HLT_CTRL;
					break;
				case SymbolType::StringLiteral:
					highlight = HLT::HLT_STR;
					break;
				case SymbolType::Variable:
					highlight = HLT::HLT_VAR;
					break;
				}

				// Print symbol
				DebugMessageHLT(MSGTYPE::MSG_DEBUG_LINEINDIC, highlight, "%s", symbol.c_str());
			}
		}

		if (returnToStart) file.seekg(pos, std::ios_base::beg);
		DebugMessage(MSGTYPE::MSG_DEBUG_LINEINDIC, "\n");
		return endOfDoc;
	}
}

using namespace DEBUGMSGNS;

// If "get" == true, do not assume we are setting the variable unless it is undeclared.
int Variable(bool get)
{
	VariableSymbol symbol;
	file >> symbol;
	DebugMessage(MSGTYPE::MSG_DEBUG_EXTRA, "Read the variable symbol \"%s\", which has ", symbol.c_str());
	if (ValidVar(symbol)) DebugMessage(MSGTYPE::MSG_DEBUG_EXTRA, "a value of %i.\n", vars.GetVar(symbol));
	else DebugMessage(MSGTYPE::MSG_DEBUG_EXTRA, "not yet been declared. Now declaring it as a new var.\n");

	if (!(get && vars.IsVar(symbol))) // If the variable already exists, and we do not want to set it, skip this section.
	{
		std::streampos goBack = file.tellg();
		std::string symbol1;
		file >> symbol1;
		file.seekg(goBack);
		DebugMessage(MSGTYPE::MSG_DEBUG_EXTRA, "Read the symbol %s, ", symbol1.c_str());
		// Determine value to set the var to
		int value = INT_MAX;
		switch (FindSymbolType(symbol1))
		{
		case SymbolType::Variable: // Initialize variable with another variable
			{
				DebugMessage(MSGTYPE::MSG_DEBUG_EXTRA, "which is a variable");
				value = Variable();

				DebugMessage(MSGTYPE::MSG_DEBUG, "Used the variable ");
				DebugMessageHLT(MSGTYPE::MSG_DEBUG, HLT::HLT_VAR, "%s", symbol1.c_str());
				DebugMessage(MSGTYPE::MSG_DEBUG, " (currently ");
				DebugMessageHLT(MSGTYPE::MSG_DEBUG, HLT::HLT_LIT, "%i", value);
				DebugMessage(MSGTYPE::MSG_DEBUG, ") as the value for variable ");
				DebugMessageHLT(MSGTYPE::MSG_DEBUG, HLT::HLT_VAR, "%s", symbol.c_str());
				DebugMessage(MSGTYPE::MSG_DEBUG, ".\n");
			}
			break;

		case SymbolType::Function: // Initialize variable with function return
			DebugMessage(MSGTYPE::MSG_DEBUG_EXTRA, "for Function.\n");
			DebugMessage(MSGTYPE::MSG_WARNING, "WARNING: Initializing a variable with a function is not yet supported");
			// TODO: Set vars with functions
			break;

		case SymbolType::ValueLiteral: // Initialize variable with literal
			file >> value;
			DebugMessage(MSGTYPE::MSG_DEBUG_EXTRA, "for the literal value %i.\n", value);
			DebugMessage(MSGTYPE::MSG_DEBUG, "Used the literal ");
			DebugMessageHLT(MSGTYPE::MSG_DEBUG, HLT::HLT_LIT, "%i", value);
			DebugMessage(MSGTYPE::MSG_DEBUG, " as the value for variable ");
			DebugMessageHLT(MSGTYPE::MSG_DEBUG, HLT::HLT_VAR, "%s", symbol.c_str());
			DebugMessage(MSGTYPE::MSG_DEBUG, ".\n");
			break;

		default: // No keyword for initialization
			DebugMessage(MSGTYPE::MSG_DEBUG_EXTRA, "for an error to occur.\n");
			DebugMessage(MSGTYPE::MSG_WARNING, "WARNING: INVALID SYNTAX FOR DECLARING A VAR! Expected 'lit', 'var', or 'func'. Read '%s' instead.\n", symbol1.c_str());
			break;
		}

		if (vars.IsVar(symbol)) // Variable exists
		{
			DebugMessage(MSGTYPE::MSG_DEBUG_PRODUCT, "Set the variable %s with new value %i (Previously %i).\n", symbol.c_str(), value, vars.GetVar(symbol));
			vars.SetVar(symbol, value);
		}
		else // Variable does not exist, initialize
		{
			vars.DeclareVar(symbol, value);
			DebugMessage(MSGTYPE::MSG_DEBUG_PRODUCT, "Declared new variable %s with init value %i.\n", symbol.c_str(), value);
		}
	}
	DebugMessage(MSGTYPE::MSG_DEBUG_EXTRA, "Returning the value of %s (currently %i).\n", symbol.c_str(), vars.GetVar(symbol));
	return vars.GetVar(symbol);
}

int Param()
{
	std::streampos goBack = file.tellg();
	std::string symbol;
	file >> symbol;
	DebugMessage(MSGTYPE::MSG_DEBUG_EXTRA, "Read the parameter keyword \"%s\"\n", symbol.c_str());
	int value = INT_MAX;
	switch (FindSymbolType(symbol)) {
	case SymbolType::ValueLiteral:
		file.seekg(goBack);
		file >> value;
		DebugMessage(MSGTYPE::MSG_DEBUG_EXTRA, "Used the literal value %i as a parameter\n", value);
		break;
	case SymbolType::Variable:
		value = Variable();
		break;
	case SymbolType::Keyword: // TODO: Handle declaration of new var as a param
		DebugMessage(MSGTYPE::MSG_ERROR, "WARNING: Declaring a new variable in a parameter for a function is not yet supported");
		break;
	case SymbolType::Function: // TODO: Functions as parameters for other functions
		DebugMessage(MSGTYPE::MSG_WARNING, "WARNING: Using a function as a parameter for another function is not yet supported");
		break;
	default:
		DebugMessage(MSGTYPE::MSG_ERROR, "WARNING: Using a function as a parameter for another function is not yet supported");
		break;
	}
	return value;
}

int Function()
{
	FunctionSymbol symbol;
	file >> symbol;
	DebugMessage(MSGTYPE::MSG_DEBUG_EXTRA, "Read the function \"%s\" symbol for ", symbol.c_str());

	FuncToken token = Tokenize(symbol);

	switch (token)
	{
	case FuncToken::Wait: // Wait
	{
		DebugMessage(MSGTYPE::MSG_DEBUG_EXTRA, "Wait.\n");
		int time = Param();
		DebugMessage(MSGTYPE::MSG_DEBUG_PRODUCT, "Waiting %i ms...\n", time);
		Sleep(time);
		DebugMessage(MSGTYPE::MSG_DEBUG, "Finished wait.\n");
		break;
	}
	case FuncToken::Print: // Print
	{
		DebugMessage(MSGTYPE::MSG_DEBUG_EXTRA, "Print.\n");
		std::string str;
		file >> str;
		printf("%s\n", str.c_str());
		DebugMessage(MSGTYPE::MSG_DEBUG_PRODUCT, "Printed the string ");
		DebugMessageHLT(MSGTYPE::MSG_DEBUG_PRODUCT, HLT::HLT_STR, "\"%s\"", str.c_str());
		DebugMessage(MSGTYPE::MSG_DEBUG_PRODUCT, ".\n");
		break;
	}
	break;
	// TODO: Add more functions
	default:
		DebugMessage(MSGTYPE::MSG_DEBUG_EXTRA, "Undefined behavior.\n");
		break;
	}
	return 0;
}

// TODO: Implement control statments
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
		break;
	case CtrlToken::ctrl_while: // While
		break;
	case CtrlToken::ctrl_break: // Break
		file.ignore(256, ';');
		break;
	default:
		DebugMessage(MSGTYPE::MSG_ERROR, "ERROR: CONTROL KEYWORD USED WITHOUT VALID CONTROL SYMBOL.\n");
	}
	return 0;
}

int InterpretFile(const char* filename)
{
	// TODO: Update this
	/***************************************************************************
	*
	*   VERSION 0 KEY
	*   =============
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
	*   "MouseTo"
	*		Move mouse to position (x, y).
	*       2 params (int x, int y)
	*
	*   "Click"
	*		Simulate mouse click.
	*       0 params
	* 
	*	"Keypress"
	*		Simulate a keystroke
	*		1 param (char key)
	* 
	*	"Wait"
	*		Wait x milliseconds.
	*		1 param (int x)
	* 
	*	"Print"
	*		Print the string to the console.
	*		1 param (const char* string)
	*		NOTE: String params have 2 options:
	*			a) "small" Print only the next string (string must have underscores in place of spaces, and "\_" to insert an underscore)
	*			b) "multi" Keep printing each line one at a time until the terminating string "\endPrt" is reached. Spaces are allowed.
	*				First whitespace is skipped.
	*	"Open"
	*		Open the specified file/program.
	*		2 params (const char* programName, const char* commandline)
	*		SPECIAL CASES:
	*			1. "IE" Opens Internet Explorer *specifically*. // This is all that's supported at the moment
	*			2. //TODO: Add support for more programs
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
	*   :Print small Foo			// string
	*	Note: string parameters cannot use variables (all vars are ints) and expect
	*	a string without the lit keywords.
	*
	***************************************************************************/

	file = std::ifstream(filename); // Set the file

	// Print whole document
	/*
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
		// Cleanup
		vars.ClearAll();
		file.seekg(0);
	}
	*/

	file.ignore(256,' ');
	int versionNumber;
	file >> versionNumber;
	DebugMessage(MSGTYPE::MSG_DEBUG, "Executing Virtual Hank Instruction Document \"%s\" using version number %i.\n", filename, versionNumber);

	if (versionNumber < g_version)
	{
		UpdateFile(filename, versionNumber);
	}

	int line = 0;

	while (true) // For each line
	{
		file.ignore(256, ':');
		++line; // I want the lines to be { 1, 2, 3, ... } instead of { 0, 1, 2, ... }
		DebugMessage(MSGTYPE::MSG_DEBUG_LINEINDIC, "\n[LINE %i]: ", line);
		//PrintLine();

		std::string startingSymbol;
		file >> startingSymbol;

		DebugMessage(MSGTYPE::MSG_DEBUG_EXTRA, "Read the line-starting symbol \"%s\" which is ", startingSymbol.c_str());

		SymbolType type = FindSymbolType(startingSymbol);
		switch (type)
		{
		case SymbolType::Function:
			DebugMessage(MSGTYPE::MSG_DEBUG_EXTRA, "a function.\n");
			Function();
			break;

		case SymbolType::Variable:
			DebugMessage(MSGTYPE::MSG_DEBUG_EXTRA, "an existing variable.\n");
			Variable(false); // We are almost certainly reassigning the variable if its name is at the start of a line
			break;

		case SymbolType::Keyword:
		{
			switch (Tokenize((KeywordSymbol)(startingSymbol.c_str())))
			{
			case KeywordToken::end: // HACK: Not really sure why "end" is being handled like this but ok
				goto END;
				break;

			case KeywordToken::var:
				DebugMessage(MSGTYPE::MSG_DEBUG_EXTRA, "for declaring a new var.\n");
				Variable();
				break;
			}
			break;
		}
		case SymbolType::Control:
			DebugMessage(MSGTYPE::MSG_DEBUG_EXTRA, "a control keyword.\n");
			Control();
			break;
		default: // Any literal type or otherwise unknown symbol
			DebugMessage(MSGTYPE::MSG_DEBUG_EXTRA, "undefined.\n");
			DebugMessage(MSGTYPE::MSG_ERROR, "ERROR: INVALID SYNTAX! Couldn't find a definition for \"%s\".\n", startingSymbol.c_str());
			break;
		}
	}

END:

	DebugMessage(MSGTYPE::MSG_DEBUG, "End of document.\n");

	file.close();

	return 0;
}

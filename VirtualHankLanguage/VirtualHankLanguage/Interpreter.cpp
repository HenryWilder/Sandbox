#include <sstream>
#include <unordered_map>
#include "GUI.h"
#include "Interpreter.h"

#define DebugMessage(...)
#define DebugMessageHLT(...)

int g_version = 0;

bool IsNumber(char first)
{
	return ((first >= '0' && first <= '9') || first == '-');
}
bool IsNumber(const char* input) { return IsNumber(input[0]); }
bool IsNumber(const std::string& input) { return IsNumber(input[0]); }

enum class FuncToken : char
{
	Print,
	Wait,
	MouseTo,
	Click,
	Keypress,
	Open,
};
const std::unordered_map<std::string, FuncToken> g_FunctionSymbolList
{
	{ "Print",		FuncToken::Print	},
	{ "Wait",		FuncToken::Wait		},
	{ "MouseTo",	FuncToken::MouseTo	},
	{ "Click",		FuncToken::Click	},
	{ "Keypress",	FuncToken::Keypress },
	{ "Open",		FuncToken::Open		},
};

enum class KeywordToken : char
{
	var,
};
const std::unordered_map<std::string, KeywordToken> g_Keywords
{
	{ "var",	KeywordToken::var,		},
};

enum class CtrlToken : char
{
	ctrl_if,
	ctrl_for,
	ctrl_while,
	ctrl_break,
};
const std::unordered_map<std::string, CtrlToken> g_CtrlStatements
{
	{ "if",		CtrlToken::ctrl_if		},
	{ "for",	CtrlToken::ctrl_for		},
	{ "while",	CtrlToken::ctrl_while	},
	{ "break",	CtrlToken::ctrl_break	},
};

std::unordered_map<std::string, float> g_vars;

float var(const std::string& symbol, float value)
{
	g_vars[symbol] = value;
	SayDebug("The value of " + symbol + " is " + std::to_string(value));
	return value;
}
float var(const std::string& symbol)
{
	float val = g_vars[symbol];
	SayDebug("The value of " + symbol + " is " + std::to_string(val));
	return val;
}

enum class SymbolType
{
	ERR,			// Anything not fitting into the following lists
	StringLiteral,	// Uses quotemarks
	ValueLiteral,	// Starts with a number
	Keyword,		// In list of keywords
	Control,		// In list of control keywords
	Function,		// In list of functions
	Variable,		// In list of existing g_vars
};
SymbolType FindSymbolType(const std::string& symbol)
{
#define CONTAINS(container, element) container.find(element) != container.end()

	if (!symbol.empty())
	{
		if (symbol[0] == '\"')								return SymbolType::StringLiteral;
		else if (IsNumber(symbol))							return SymbolType::ValueLiteral;
		else if (CONTAINS(g_Keywords, symbol))				return SymbolType::Keyword;
		else if (CONTAINS(g_CtrlStatements, symbol))		return SymbolType::Control;
		else if (CONTAINS(g_FunctionSymbolList, symbol))	return SymbolType::Function;
		else if (CONTAINS(g_vars, symbol))					return SymbolType::Variable;
		else return SymbolType::ERR;
	}

}

float Param(std::stringstream& stream);

// We already have the name of the var
float Variable(std::stringstream& stream, const std::string& symbol)
{
	SayDebug("Read the variable symbol \"" + symbol + "\"");
	try
	{
		float value = Param(stream);
		return var(symbol, value);
	}
	catch (const char* msg)
	{
		if (msg == "eof") return var(symbol);
	}
}
// "var" was read
float Variable(std::stringstream& stream)
{
	std::string symbol;
	stream >> symbol;
	return Variable(stream, symbol);
}

float Param(std::stringstream& stream)
{
	if (stream.eof()) throw "eof";
	std::string symbol;
	stream >> symbol;
	SayDebug("Read the parameter \"" + symbol + "\"");
	float value;
	switch (FindSymbolType(symbol))
	{
	case SymbolType::ValueLiteral:
		value = std::stof(symbol);
		SayDebug("Used the literal value " + std::to_string(value) + " as a parameter");
		break;
	case SymbolType::Keyword:
		if (symbol == "var")
		{
			value = Variable(stream);
			SayDebug("Created a new variable as a parameter");
		}
		else throw "Tried using a keyword as a value";
		break;
	case SymbolType::Variable:
		value = Variable(stream, symbol);
		SayDebug("Used the variable value " + std::to_string(value) + " as a parameter");
		break;
	case SymbolType::Function: // TODO: Functions as parameters for other functions
		throw "Tried using a function as a parameter for another function";
		break;
	default:
		throw "Could not deduce symbol type!";
		break;
	}
	return value;
}

// TODO: Implement control statments
int Control(std::stringstream& stream)
{
	// TODO
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
	*   ':' Start of function - Referred to as a "lineStream" in debug
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
	*			b) "multi" Keep printing each lineStream one at a time until the terminating string "\endPrt" is reached. Spaces are allowed.
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
	*	Note: only integer g_vars are supported in this version.
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
	*	Note: string parameters cannot use variables (all g_vars are ints) and expect
	*	a string without the lit keywords.
	*
	***************************************************************************/

	std::ifstream file(filename); // Open the file

	file.ignore(256,' ');
	int versionNumber;
	file >> versionNumber;
	SayDebug("Executing Virtual Hank Instruction Document " + std::string(filename) + " using version number " + std::to_string(versionNumber));

	if (versionNumber < g_version)
	{
		UpdateFile(filename, versionNumber); // TODO
	}

	int lineNumber = 0;

	file.ignore(256, '\n');
	std::string _line;
	while (std::getline(file, _line, ';')) // For each lineStream
	{
		++lineNumber; // I want the lines to be { 1, 2, 3, ... } instead of { 0, 1, 2, ... }
		SayDebug("Now evaluating line " + std::to_string(lineNumber));
		std::stringstream lineStream(_line);
		{
			size_t start = _line.find_first_not_of(" \n\t");
			SayDebug("<[code]( " + _line.substr(start) + " )>");
		}

		std::string startingSymbol;
		lineStream >> startingSymbol;

		switch (SymbolType type = FindSymbolType(startingSymbol))
		{
		case SymbolType::Function:
			{
				auto it = g_FunctionSymbolList.find(startingSymbol);
				if (it != g_FunctionSymbolList.end())
				{
					switch (it->second)
					{
					case FuncToken::Print:
					{
						size_t start = _line.find('\"');
						start++;
						size_t length = _line.find('\"', start) - start;
						std::string str = _line.substr(start, length);
						SayComm(str);
						SayDebug(str);
					}
					break;

					case FuncToken::Wait:
					{
						try
						{
							float time_ms = Param(lineStream);
							Sleep((DWORD)time_ms);
						}
						catch (const char* msg)
						{
							SayDebug(msg, DebugColor::Critical);
						}
					}
					break;

					case FuncToken::MouseTo:
						// TODO
						break;
					case FuncToken::Click:
						// TODO
						break;
					case FuncToken::Keypress:
						// TODO
						break;
					case FuncToken::Open:
						// TODO
						break;
					}
				}
				else
				{
					SayDebug("Function name not found in function list!", DebugColor::Critical);
				}
			}
			break;

		case SymbolType::Variable:
			{
				Variable(lineStream, startingSymbol); // We are almost certainly reassigning the variable if its name is at the start of a lineStream
			}
			break;

		case SymbolType::Keyword:
			{
				switch (g_Keywords.find(startingSymbol)->second)
				{
				case KeywordToken::var:
					Variable(lineStream);
					break;
				}
			}
			break;

		case SymbolType::Control:
			{
				Control(lineStream);
			}
			break;

		default: // Any literal type or otherwise unknown symbol
			{
				SayDebug("ERROR: INVALID SYNTAX! Couldn't find a definition!", DebugColor::Critical);
				goto END;
			}
			break;
		}

		//for (auto var : g_vars)
		//{
		//	SayDebug("Var " + var.first + " has value of " + std::to_string(var.second));
		//}
	}

END:

	SayDebug("Reached end of document.");

	file.close();

	return 0;
}

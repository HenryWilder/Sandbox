#if 0
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
	
}
#endif
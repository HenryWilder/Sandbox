#pragma once
#include <fstream>
#include <vector>
#include <utility>
#include "Functions.h"
#include "VersionConversion.h"

extern int g_version;

extern std::ifstream file;

// @TODO: Implement void FunctionPointers for advanced shit
typedef void* FunctionPointer; // Do not fear me... I give an alias to the void

struct Symbol : public std::string
{
	Symbol() : std::string() {};
	Symbol(std::string const& token) : std::string(token) {}
	Symbol(const char* const& token) : std::string(token) {}

	operator std::string() const { return (*(std::string*)(this)); }
	operator const char* () const { return this->c_str(); }
};

struct StringLiteral : public std::string
{
	StringLiteral() : std::string() {}
	StringLiteral(std::string const& token) : std::string(token) {}
	StringLiteral(const char* const& token) : std::string(token) {}

	operator std::string() const { return (*(std::string*)(this)); }
	operator const char* () const { return this->c_str(); }
};

// TODO: Make this work
class FuncBase
{
private:
	FunctionPointer m_function;
	int m_paramCount;

public:
	FuncBase() : m_function(nullptr), m_paramCount() {}
	FuncBase(FunctionPointer funcPtr, int params) : m_function(funcPtr), m_paramCount(params) {}

	void Execute() const
	{
		funcPtr();
	}
};

// @SPAGHETTI: I feel like this could possibly be done cleaner with templates? Maybe? or a base type could work too.
struct FunctionSymbol : public std::string
{
	FunctionSymbol() : std::string("") {};
	FunctionSymbol(std::string const& token) : std::string(token) {}
	FunctionSymbol(const char* const& token) : std::string(token) {}

	operator std::string() const { return (*(std::string*)(this)); }
	operator const char* () const { return this->c_str(); }
};
extern const FunctionSymbol g_FunctionSymbolList[];
extern const FuncBase g_FunctionClassList[];
enum class FuncToken : char;
FuncToken Tokenize(FunctionSymbol);
bool ValidFunction(const FunctionSymbol);
bool ValidFunction(const std::string);
bool ValidFunction(const char*);

struct KeywordSymbol : public std::string
{
	KeywordSymbol() : std::string() {};
	KeywordSymbol(std::string const& token) : std::string(token) {}
	KeywordSymbol(const char* const& token) : std::string(token) {}

	operator std::string() const { return (*(std::string*)(this)); }
	operator const char* () const { return this->c_str(); }
};
extern const KeywordSymbol g_Keywords[];
enum class KeywordToken : char;
KeywordToken Tokenize(const KeywordSymbol);
bool ValidKeyword(const KeywordSymbol);
bool ValidKeyword(const std::string);
bool ValidKeyword(const char*);

struct CtrlSymbol : public std::string
{
	CtrlSymbol() : std::string() {};
	CtrlSymbol(std::string const& token) : std::string(token) {}
	CtrlSymbol(const char* const& token) : std::string(token) {}

	operator std::string() const { return (*(std::string*)(this)); }
	operator const char* () const { return this->c_str(); }
};
extern const CtrlSymbol g_CtrlStatements[];
enum class CtrlToken : char;
CtrlToken Tokenize(const CtrlSymbol);
bool ValidCtrl(const CtrlSymbol);
bool ValidCtrl(const std::string);
bool ValidCtrl(const char*);

bool IsNumber(const char first);
bool IsNumber(const char* input);
bool IsNumber(const std::string input);

// @TODO: Try implementing more types than just "int".
struct VariableSymbol : public std::string
{
	VariableSymbol() : std::string() {};
	VariableSymbol(std::string const& token) : std::string(token) {}
	VariableSymbol(const char* const& token) : std::string(token) {}

	operator std::string() const { return (*(std::string*)(this)); }
	operator const char* () const { return this->c_str(); }

private:
	// HACK: Dynamic_cast<>() requires at least one virtual function to work... so here it is I guess?...
	// Remove it if you ever find a use for a virtual function in this class.
	virtual void Dummy() const {}
};
template<typename Type = int>
class VariableSymbolSpec : public VariableSymbol
{
public:
	VariableSymbolSpec() : VariableSymbol(), m_value() {}
	VariableSymbolSpec(std::string const& token) : VariableSymbol( token ), m_value() {}
	VariableSymbolSpec(VariableSymbol const& token) : VariableSymbol( token ), m_value() {}
	VariableSymbolSpec(std::string const& token, Type value) : VariableSymbol( token ), m_value( value ) {}
	VariableSymbolSpec(VariableSymbol const& token, Type value) : VariableSymbol( token ), m_value( value ) {}
private:
	Type m_value;
	// HACK: See VariableSymbol
	virtual void Dummy() const {}

public:
	const int GetValue() const { return m_value; }
	void SetValue(const int& value) { m_value = value; }
};

// SPAGHETTI: A lot of the VariableSymbol parameters could probably be better replaced with std::strings, to make things less confused.
struct CustomVars
{
private:
	std::vector<VariableSymbol*> m_vars; // Array of var names
	size_t _Locate(const VariableSymbol& symbol) const;

public:
	~CustomVars()
	{
		for (VariableSymbol*& var : m_vars)
		{
			delete var;
		}
		if (m_vars.size()) m_vars.clear();
	}

	bool IsVar(const VariableSymbol& symbol) const;

	template<typename Type = int>
	void DeclareVar(const VariableSymbol& symbol, const Type& value = NULL)
	{
		VariableSymbolSpec<Type>* var = new VariableSymbolSpec<Type>(symbol, value);
		m_vars.push_back(var);
		//printf("\nDeclared var\n");
	}
	template<typename Type = int>
	const Type GetVar(const VariableSymbol& symbol) const
	{
		VariableSymbol* const& element = m_vars.at(_Locate(symbol));
		VariableSymbolSpec<Type>* var = dynamic_cast<VariableSymbolSpec<Type>*>(element);
		//printf("\nReturned var\n");
		if (var) return var->GetValue();
		else return ERROR;
	}
	template<typename Type = int>
	void SetVar(const VariableSymbol& symbol, const Type& value)
	{
		VariableSymbolSpec<Type>* var = dynamic_cast<VariableSymbolSpec<Type>*>(m_vars.at(_Locate(symbol)));
		if (var) var->SetValue(value);
		else printf("ENCOUNTERED AN ERROR SETTING VAR");
		//printf("\nSet a var\n");
	}

	void ClearAll(); // For debugging
};
extern CustomVars vars;
bool ValidVar(const VariableSymbol&);
bool ValidVar(const std::string&);
bool ValidVar(const char*&);

enum class SymbolType;
SymbolType FindSymbolType(std::string);

namespace DEBUGMSGNS
{
	extern HANDLE hConsole;

	enum class MSGTYPE; // Messages
	enum class HLT; // Highlighting

	// BROKEN: Was gonna make a more reasonable way of doing debug messages than with variadic templates, but uh...
	/*
	struct DebugDataChunk
	{
		virtual void ChunkColor() const = 0;
		virtual void PrintData() const = 0;
	};
	template<typename T>
	struct DebugDataBit : DebugDataChunk
	{
		DebugDataBit() : data{ NULL } {};
		DebugDataBit(T _data) : data{ _data } {};

		T data;

		void ChunkColor() const
		{
			SetConsoleTextAttribute(hConsole, HLT_LIT);
		}

		void PrintData() const override
		{
			ChunkColor();
			printf("%i", data);
		}
	};
	template<>
	struct DebugDataBit<StringLiteral> : DebugDataChunk
	{
		//DebugDataBit() : data{ "" } {};
		DebugDataBit(StringLiteral _data) : data{ _data } {};

		StringLiteral data;

		void ChunkColor() const
		{
			SetConsoleTextAttribute(hConsole, HLT_STR);
		}

		void PrintData() const override
		{
			ChunkColor();
			printf("\"%s\"", data.c_str());
		}
	};
	template<>
	struct DebugDataBit<VariableSymbol> : DebugDataChunk
	{
		//DebugDataBit() : data{ "" } {};
		DebugDataBit(VariableSymbol _data) : data{ _data } {};

		VariableSymbol data;

		void ChunkColor() const
		{
			SetConsoleTextAttribute(hConsole, HLT_VAR);
		}

		void PrintData() const override
		{
			ChunkColor();
			printf("%s", data.c_str());
		}
	};
	template<>
	struct DebugDataBit<FunctionSymbol> : DebugDataChunk
	{
		//DebugDataBit() : data{ "" } {};
		DebugDataBit(FunctionSymbol _data) : data{ _data } {};

		FunctionSymbol data;

		void ChunkColor() const
		{
			SetConsoleTextAttribute(hConsole, HLT_FUNC);
		}

		void PrintData() const override
		{
			ChunkColor();
			printf("%s", data.c_str());
		}
	};
	template<>
	struct DebugDataBit<KeywordSymbol> : DebugDataChunk
	{
		//DebugDataBit() : data{ "" } {};
		DebugDataBit(KeywordSymbol _data) : data{ _data } {};

		KeywordSymbol data;

		void ChunkColor() const
		{
			SetConsoleTextAttribute(hConsole, HLT_FUNC);
		}

		void PrintData() const override
		{
			ChunkColor();
			printf("%s", data.c_str());
		}
	};

	struct DebugDataBundle
	{
		DebugDataBundle() : m_dataChunks{ } {};
		DebugDataBundle(std::vector<DebugDataChunk*> _chunkArray) : m_dataChunks{ _chunkArray } {};
		template<typename...Args>
		DebugDataBundle(Args..._chunkPtrs) 
		{
			m_dataChunks = { _chunkPtrs... };
		}

		std::vector<DebugDataChunk*> m_dataChunks;

		template <typename T>
		void Add(T data)
		{
			DebugDataBit<T> chunk(data);
			Add(&chunk);
		}
		template<> void Add<DebugDataChunk*>(DebugDataChunk* chunk)
		{
			m_dataChunks.push_back(chunk);
		}
	};
	*/

	extern const bool hideExtranious;
	extern const bool majorOnly;

	void DebugMessageHLT(MSGTYPE, HLT, const char*, ...);
	void DebugMessage(MSGTYPE, const char*, ...);
	/*
	void DebugMessage(MSGTYPE color, const std::string message, const DebugDataBundle& data);

	template <typename T>
	void ConstructDataBit(DebugDataBundle& bundle, T a) { bundle.Add(a); }
	
	void ConstructDataBundle(DebugDataBundle& bundle) {}
	
	template <typename...Args>
	void ConstructDataBundle(DebugDataBundle& bundle, Args...args)
	{
		int dummy[] = { ((void)bundle.Add(bundle, std::forward<Args>(args))))... };
	}
	
	template <typename...Args>
	void PrintDebugMSG(MSGTYPE type, std::string message, Args...args)
	{
		DebugDataBundle bundle;
		ConstructDataBundle(bundle, args...);
		DebugMessage(MSG_DEBUG, message, bundle);
	}
	*/
	bool PrintLine(bool returnToStart = true);
}

int Variable(bool get = true);
int Param();
int Function();

int InterpretFile(const char*);

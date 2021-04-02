#pragma once
#include <fstream>
#include <vector>
#include <unordered_map>
#include <utility>
#include "Functions.h"
#include "VersionConversion.h"

extern int g_version;

extern std::ifstream file;

enum class FuncToken : char;
extern const std::unordered_map<std::string, FuncToken> g_FunctionSymbolList;
enum class KeywordToken : char;
extern const std::unordered_map<std::string, KeywordToken> g_Keywords;
enum class CtrlToken : char;
extern const std::unordered_map<std::string, CtrlToken> g_CtrlStatements;
enum class SymbolType;

class CustomVars
{
private:
	std::unordered_map<std::string, float> m_vars;

public:
	bool IsVar(const std::string& symbol) const;

	void DeclareVar(const std::string& symbol);
	void DeclareVar(const std::string& symbol, float value);
	float GetVar(const std::string& symbol) const;
	void SetVar(const std::string& symbol, float value);
};
extern CustomVars g_vars;

enum class SymbolType;
SymbolType FindSymbolType(const std::string&);

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

float Variable(bool get = true);
float Param();
float Function();

int InterpretFile(const char*);

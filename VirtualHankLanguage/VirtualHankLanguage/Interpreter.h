#pragma once
#include <fstream>
#include <vector>
#include <utility>
#include "Functions.h"
#include "VersionConversion.h"

extern int g_version;

extern std::ifstream file;

struct StringLiteral : public std::string {};
struct VariableSymbol : public std::string {};
struct FunctionSymbol : public std::string {};
struct KeywordSymbol : public std::string {};

struct CustomVars
{
private:
	std::vector<VariableSymbol> var_names; // Array of var names
	std::vector<int> var_vals; // Array of values (in same order as names)
	size_t _Locate(const VariableSymbol symbol) const;

public:
	bool IsVar(const VariableSymbol symbol) const;
	void DeclareVar(const VariableSymbol symbol, const int value = 0);
	const int GetVar(const VariableSymbol symbol) const;
	void SetVar(const VariableSymbol symbol, const int value);
};
extern CustomVars vars;

namespace DEBUGMSGNS
{
	extern HANDLE hConsole;

	typedef enum {
		MSG_DEFAULT = 7,
		MSG_DEBUG_EXTRA = 8,
		MSG_DEBUG = 1,
		MSG_DEBUG_PRODUCT = 3,
		MSG_DEBUG_LINEINDIC = 11,
		MSG_WARNING = 14,
		MSG_WARNING_MINOR = 6,
		MSG_ERROR = 4,
	} MSG_TYPE; // Messages
	typedef enum {
		HLT_VAR = 11, // Variable
		HLT_LIT = 2, // Literal
		HLT_FUNC = 6, // Function
		HLT_STR = 13, // String
	} HLT_COLOR; // Highlighting

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

	extern const bool hideExtranious;
	extern const bool majorOnly;

	//void DebugMessage(MSG_TYPE type, HLT_COLOR color, const char* text, ...);
	//void DebugMessage(MSG_TYPE color, const char* text, ...);
	void DebugMessage(MSG_TYPE color, const std::string message, const DebugDataBundle& data);

	template <typename T>
	void ConstructDataBit(DebugDataBundle& bundle, T a) { bundle.Add(a); }

	void ConstructDataBundle(DebugDataBundle& bundle) {}

	template <typename T, typename ...Args>
	void ConstructDataBundle(DebugDataBundle& bundle, T argument, Args... args)
	{
		ConstructDataBit(bundle, argument);
		//ConstructDataBundle(bundle, args...);
		int dummy[] = { 0, ((void)ConstructDataBundle(bundle, std::forward<Args>(args)), 0) ... };
	}

	template <typename ...Args>
	void PrintDebugMSG(MSG_TYPE type, std::string message, Args... args)
	{
		DebugDataBundle bundle;
		//int dummy[] = { 0, ((void) ConstructDataBundle(bundle, std::forward<Args>(args)), 0) ... }; // Maybe the cause of the compile looping?
		ConstructDataBundle(bundle, std::forward<Args>(args), ...);
		DebugMessage(MSG_DEBUG, message, bundle);
	}
	void PrintLine();
}

int Variable(bool get = true);
int Param();
int Function();

int InterpretFile(const char*);

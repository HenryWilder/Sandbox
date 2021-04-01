#pragma once
#include<vector>
#include<string>
#include<unordered_map>

#define MAX_DIGITS

namespace debug
{
	using namespace std;

	struct VectorOfValues
	{
		VectorOfValues(vector<short>* vec);
		VectorOfValues(vector<int>* vec);
		VectorOfValues(vector<long>* vec);
		VectorOfValues(vector<long long>* vec);
		VectorOfValues(vector<unsigned short>* vec);
		VectorOfValues(vector<unsigned int>* vec);
		VectorOfValues(vector<unsigned long>* vec);
		VectorOfValues(vector<unsigned long long>* vec);

		enum
		{
			SHORT,
			INT,
			LONG,
			LLONG,

			USHORT,
			UINT,
			ULONG,
			ULLONG,
		} tag;

		union
		{
			vector<short>* s;
			vector<int>* i;
			vector<long>* l;
			vector<long long>* ll;

			vector<unsigned short>* us;
			vector<unsigned int>* ui;
			vector<unsigned long>* ul;
			vector<unsigned long long>* ull;
		};
		unsigned long long operator[](size_t index);
	};
	struct VectorOfFractions
	{
		VectorOfFractions(vector<float>* vec);
		VectorOfFractions(vector<double>* vec);
		VectorOfFractions(vector<long double>* vec);

		enum
		{
			FLOAT,
			DOUBLE,
			LDOUBLE
		} tag;

		union
		{
			vector<float>* f;
			vector<double>* d;
			vector<long double>* ld;
		};
		long double operator[](size_t index);
	};

	// Vector of strings
	const char* VectorToString(std::vector<const char*>& vec)
	{
		size_t size = 2; // " }" is the same length as the dropped ", " so it is not factored in.
		for (const char* elem : vec)
		{
			size_t c = 0;
			while (elem[c]) { ++c; }
			size += c + 2;
		}
		char* const str = new char[size + 1](); // + 1 for the null-terminating char
		str[0] = '{';
		str[1] = ' ';
		size_t pos = 2;
		for (size_t i = 0; i < vec.size(); ++i)
		{
			for (size_t c = 0; vec[i][c]; ++c)
			{
				str[pos++] = vec[i][c];
			}
			str[pos++] = ',';
			str[pos++] = ' ';
		}
		str[size - 2] = ' ';
		str[size - 1] = '}';
		str[size] = '\0'; // Null-terminator
		return str;
	}
	// Vector of characters
	const char* VectorToString(std::vector<char>& vec)
	{
		string str;
		str.reserve(vec.size()*);
		for (T& value : vec)
		{
			str
		}
		return str;
	}
	// Vector of values
	const char* VectorToString(VectorOfValues vec)
	{
		string str;
		str.reserve(vec.size()*);
		for (T& value : vec)
		{
			str
		}
		return str;
	}
	// Vector of floats
	const char* VectorToString(std::vector<float>& vec)
	{
		string str;
		str.reserve(vec.size()*);
		for (T& value : vec)
		{
			str
		}
		return str;
	}
	// Vector of pointers
	const char* VectorToString(std::vector<void*>& vec)
	{
		string str;
		str.reserve(vec.size()*);
		for (T& value : vec)
		{
			str
		}
		return str;
	}
	
	struct Timer
	{
		chrono::steady_clock::time_point start;
	};
	Timer StartTimer();
	long long CheckTimer(Timer timer);
}
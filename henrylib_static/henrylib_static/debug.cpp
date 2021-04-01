#include "pch.h"
#include "debug.h"
#include<chrono>

namespace debug
{
	Timer StartTimer()
	{
		return Timer{ chrono::steady_clock::now() };
	}
	long long CheckTimer(Timer timer)
	{
		return chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - timer.start).count();
	}
	VectorOfValues::VectorOfValues(vector<short>* vec)
	{
		tag = SHORT;
		s = vec;
	}
	VectorOfValues::VectorOfValues(vector<int>* vec)
	{
		tag = INT;
		i = vec;
	}
	VectorOfValues::VectorOfValues(vector<long>* vec)
	{
		tag = LONG;
		l = vec;
	}
	VectorOfValues::VectorOfValues(vector<long long>* vec)
	{
		tag = LLONG;
		ll = vec;
	}
	VectorOfValues::VectorOfValues(vector<unsigned short>* vec)
	{
		tag = USHORT;
		us = vec;
	}
	VectorOfValues::VectorOfValues(vector<unsigned int>* vec)
	{
		tag = UINT;
		ui = vec;
	}
	VectorOfValues::VectorOfValues(vector<unsigned long>* vec)
	{
		tag = ULONG;
		ul = vec;
	}
	VectorOfValues::VectorOfValues(vector<unsigned long long>* vec)
	{
		tag = ULLONG;
		ull = vec;
	}
	unsigned long long VectorOfValues::operator[](size_t index)
	{
		switch (tag)
		{
		case SHORT:  (unsigned long long)((*s)[index]); break;
		case INT:    (unsigned long long)((*i)[index]); break;
		case LONG:   (unsigned long long)((*l)[index]); break;
		case LLONG:  (unsigned long long)((*ll)[index]); break;
		case USHORT: (unsigned long long)((*us)[index]); break;
		case UINT:   (unsigned long long)((*ui)[index]); break;
		case ULONG:  (unsigned long long)((*ul)[index]); break;
		case ULLONG: (unsigned long long)((*ull)[index]); break;
		}
	}
	long double VectorOfFractions::operator[](size_t index)
	{
		switch (tag)
		{
		case FLOAT:   (unsigned long long)((*f)[index]); break;
		case DOUBLE:  (unsigned long long)((*d)[index]); break;
		case LDOUBLE: (unsigned long long)((*ld)[index]); break;
		}
	}
}
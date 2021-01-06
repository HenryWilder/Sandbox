#pragma once
#include <any>

// Custom classes & functions to make up for VS's slackin'.
namespace Cstm
{
	template <typename T>
	union test
	{
		unsigned char ch[sizeof(T)];
		T variable;
	};

	class Any
	{
	private:
		void* data;
		typedef T type;
	public:
		T Get()
		{
			return static_cast<T>(data);
		}
	};
}

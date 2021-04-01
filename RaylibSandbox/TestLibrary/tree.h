#pragma once
#include "position.h"

namespace amy
{
	template<typename T, bool largeType = (sizeof(T) > sizeof(void*))> class wrapper {};
	template<typename T> class wrapper<T, false>
	{
	private:
		T value;

	public:
		wrapper() : value() {}
		wrapper(const T& _value) : value(_value) {}

		operator T() { return value; }
	};
	template<typename T> class wrapper<T, true>
	{
	private:
		T* value;

	public:
		wrapper() : value() {}
		wrapper(const T& _value) : value(&_value) {}

		operator T() { return *value; }
	};

	template<typename T, int dimensions>
	class tree
	{
		using child		= tree*;
		using object	= wrapper<T>;

		child children[1 << dimensions];
		object* elements;
	};
}
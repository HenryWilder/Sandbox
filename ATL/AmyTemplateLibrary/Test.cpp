#include "array_dynamic.h"

int main()
{
	amy::DynArray<int> test; // No error

	test.Reserve(4); // No error

	test.Push_Back(1); // No error
	test.Push_Back(2); // No error
	test.Push_Back(3); // No error
	test.Push_Back(4); // No error
	test.Push_Back(5); // No error

	return 0;
}
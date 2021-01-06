#include <iostream>
#include "Unit.h"
#include "LinkedList.h"
#include <variant>
#include <cassert>

typedef char TypeToUse;

int main()
{
	std::variant<int, float> a;


	LinkedList<TypeToUse> list = LinkedList<TypeToUse>();

	printf("New node [%#02i] added at memory address <%p> with value (%#2i)\n", 0, list.head, list.head->value);
	for (TypeToUse i = 1; i < 10; ++i)
	{
		LinkedList<TypeToUse>::Node* node = list.Add(i);
		printf("New node [%#02i] added at memory address <%p> with value (%#2i)\n", i, node, i);
	}
	printf("\nList populated. Now retrieving.\n");
	for (TypeToUse i = 0; i < 10; ++i)
	{
		LinkedList<TypeToUse>::Node* node = list.At(i);
		printf("Node [%#02i] which is at memory address <%p> has value (%#2i)\n", i, node, node->value);
	}
	list.Insert(22, 4);
	printf("\nNew element inserted after position 4. Reprinting.\n");
	for (TypeToUse i = 0; i < 11; ++i)
	{
		LinkedList<TypeToUse>::Node* node = list.At(i);
		printf("Node [%#02i] which is at memory address <%p> has value (%#2i)\n", i, node, node->value);
	}
	list.Remove(3);
	printf("\nElement 3 removed. Reprinting.\n");
	for (TypeToUse i = 0; i < 10; ++i)
	{
		LinkedList<TypeToUse>::Node* node = list.At(i);
		printf("Node [%#02i] which is at memory address <%p> has value (%#2i)\n", i, node, node->value);
	}
	{
		LinkedList<TypeToUse>::Node* node = list.Find(8);
		printf("Searching for a node with value (8).\n");
		printf("Result from search found the node at memory address <%p> after <%p> and before <%p>\n", node, node->prev, node->next);
	}
}

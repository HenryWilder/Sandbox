#pragma once

template<typename Type>
struct LinkedList
{
	struct Node
	{
		Node() : next{ nullptr }, prev{ nullptr }, value{ 0 } {};
		Node(Type _value) : next{ nullptr }, prev{ nullptr }, value{ _value } {};

		Node* next;
		Node* prev;
		Type value;
	};

	LinkedList() {
		head = new Node;
		tail = head;
	}
	~LinkedList()
	{
		Node* tempCurr = head->next;
		Node* tempNex = tempCurr->next;
		delete head;
		while (tempNex)
		{
			tempNex = tempCurr->next;
			delete tempCurr;
			tempCurr = tempNex;
		}
		if (tempCurr) delete tempCurr;
		if (tempNex) delete tempNex;
	}

	Node* At(size_t pos)
	{
		Node* search = head;
		for (size_t i = 0; i < pos; ++i)
		{
			search = search->next;
			if (search == nullptr) break;
		}
		return search;
	}
	Node* Add(Type value)
	{
		tail->next = new Node(value);
		tail->next->prev = tail;
		tail = tail->next;
		return tail;
	}
	Node* Insert(Type value, Node* after)
	{
		Node* temp = after->next;
		after->next = new Node(value);
		after->next->prev = after;
		after->next->next = temp;
		return after->next;
	}
	Node* Insert(Type value, size_t at) { return Insert(value, At(at)); }
	void Remove(Type value)
	{
		Node* nodeToRemove = Find(value);
		nodeToRemove->prev->next = nodeToRemove->next;
		nodeToRemove->next->prev = nodeToRemove->prev;
		delete nodeToRemove;
	}
	Node* Find(Type value)
	{
		Node* search = head;
		while (search->next)
		{
			if (search->value == value) return search;
			else search = search->next;
		}
		return nullptr; // If none was found
	}

	Node* head;
	Node* tail;
};

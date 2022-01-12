#include <string>
#include <fstream>
#include <map>
#include <vector>

// Acts like a stack which can output a copy of the stack as a whole
template<class Ty>
class FullDepositStack
{
private:
	std::vector<Ty> m_value;

public:
	char& top()
	{
		return m_value.back();
	}
	void push(char c)
	{
		m_value.push_back(c);
	}
	void pop()
	{
		m_value.pop_back();
	}
	const std::vector<Ty>& full()
	{
		return m_value;
	}
};

template<>
class FullDepositStack<char>
{
private:
	std::string m_value;

public:
	char& top()
	{
		return m_value.back();
	}
	void push(char c)
	{
		m_value.push_back(c);
	}
	void pop()
	{
		m_value.pop_back();
	}
	const std::string& full()
	{
		return m_value;
	}
};

struct Node;
struct Node
{
	~Node()
	{
		for (auto [c, next] : m_children)
		{
			delete next;
		}
	}

	std::map<char, Node*> m_children;
};

int main()
{
	Node tree;

	std::string line;
	std::ifstream file("Tree.txt");

	// Input
	std::vector<Node*> treeStack;
	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			size_t depth = line.find_first_not_of(" ");
			char val = line[depth];
			if (val != '.')
			{
				Node* node = new Node;
				treeStack[depth]->m_children.insert({ val, node });
			}
		}
		file.close();
	}
	// Print
	FullDepositStack<char> path;
	for ()
	{
		stack.push();
	}
	// Output

	// Cleanup
	tree.

	return 0;
}
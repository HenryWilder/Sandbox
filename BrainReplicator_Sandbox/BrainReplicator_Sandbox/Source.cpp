#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stack>

#include "intake.h"
#include "memory.h"
#include "planning.h"
#include "output.h"
#include "scoring.h"

int main()
{
	MemoryNode top;
	top.response = "";
	std::ifstream file("knowledge.txt");
	while (file.is_open())
	{
		std::string line;
		size_t depth = 0;
		std::stack<MemoryNode*> treeStack;
		while (std::getline(file, line))
		{
			std::cout << line << std::endl;
			size_t localDepth = line.find_first_not_of(" ");
			std::stringstream _line;
			char path;
			std::string response;
			_line >> path >> response;
			MemoryNode* node = new MemoryNode;
			treeStack.top()->next[path] = node;
			node->response = response.c_str();
			if (localDepth > (depth + 1))
			{
				treeStack.push(node);
			}
			else if (localDepth == (depth + 1))
			{
				treeStack.top() = node;
			}
			else if (localDepth < (depth + 1))
			{
				treeStack.pop();
			}
			depth = localDepth;
		}
		file.close();
	}

	return 0;
}
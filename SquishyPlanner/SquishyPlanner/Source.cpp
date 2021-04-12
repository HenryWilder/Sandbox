#include <stdio.h>			// printf scanf etc
#include <iostream>			// c io
#include <fstream>			// file io
#include <cstdarg>			// Variadic functions
#include <string>			// Safer than char* in some cases
#include <sstream>			// String stream
#include <unordered_map>	//
#include <unordered_set>	//
#include <list>				//
#include <vector>			//
#include <any>				//

int main()
{
	printf("Name: ");
	std::string name;
	std::cin >> name;
	printf("Nice to meet you, %s!", name.c_str());



	return 0;
}
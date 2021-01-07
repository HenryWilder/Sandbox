#pragma once
#include <fstream>
#include "Interpreter.h"

typedef std::array<std::array<std::array<std::array<long long, MAXULONGLONG>, MAXULONGLONG>, MAXULONGLONG>, MAXULONGLONG> FWORD; // I TYPEDEF'D THE FWORD!!!

std::string FormatText(const char* text, ...);

struct Change
{
	std::string from, to; 
};
std::ifstream& operator>>(std::ifstream&, Change);

void UpdateFile(const char* filename, int fVersion);

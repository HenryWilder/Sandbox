#pragma once
#include <fstream>
#include "Interpreter.h"

const char* FormatText(const char* text, ...);

struct Change
{
	std::string from, to;
};
std::ifstream& operator>>(std::ifstream&, Change);

void UpdateFile(const char* filename, int fVersion);

#pragma once
#include "Interpreter.h"

void UpdateFile(const char* filename, int fVersion)
{
	std::ifstream versionFormatFile = std::ifstream(FormatText("%i.vhvf", fVersion));
	printf(FormatText("%i.vhvf", fVersion));


	versionFormatFile.close();
}

#pragma once
#include <vector>

class Transistor;
extern std::vector<Transistor*> s_allTransistors;

void Save(const std::vector<Transistor*>&);

void Load(std::vector<Transistor*>&);

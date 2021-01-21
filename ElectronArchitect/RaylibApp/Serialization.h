#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>

class Transistor;

void Save(const std::vector<Transistor*>&);

void Load(std::vector<Transistor*>&);

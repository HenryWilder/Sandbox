#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <fstream>

struct Wire;
struct Transistor;

void Save(std::vector<Transistor*>*);

void Load(std::vector<Transistor*>*, std::vector<Wire*>*);

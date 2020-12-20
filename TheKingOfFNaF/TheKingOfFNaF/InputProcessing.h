#pragma once
#include "Input.h"

enum class NumberScale
{
	Small = 0,
	Norml = 1,
	Large = 2,
};

// Input should be top-left corner of the number followed by the size
char ReadNumber(int x, int y, NumberScale size);

// Run this about once every frame
void ReadGameClock();

void CheckVentsReset();

void GenerateSamplePoints(POINT* arr, POINT start, long size);

int TestSamples(Button button, CNorm compare, double threshold);

int MaxInArray(int* arr, size_t size); // Returns the index of the highest value

void UpdateState();

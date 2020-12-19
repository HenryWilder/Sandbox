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
void ReadGameClock(HDC context);

void CheckVentsReset(HDC context);

void UpdateState(HDC context);

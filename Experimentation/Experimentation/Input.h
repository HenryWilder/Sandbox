#pragma once
#include <Windows.h>
#include <iostream>

extern SHORT keyInputs[5];

void InitKeys();

void KeyPress(SHORT key);

void CheckInputs();
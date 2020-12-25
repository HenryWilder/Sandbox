#pragma once
#include "Globals.h"
#include "Constants.h"

///////////////////////////////////////////////
// This is where we take input from the game //
// e.g.                                      //
// - Test pixel color at { 253, 1004 }       //
///////////////////////////////////////////////

void UpdateScreencap();

unsigned long PixelIndex(long x, long y);

Color GetPixelColor(long x, long y);
Color GetPixelColor(POINT pos);

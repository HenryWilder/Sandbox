#pragma once
#include "Shared.h"
#include "Tracking.h"

unsigned long PixelIndex(long x, long y);

Color GetPixelColor(long x, long y);

void Consume();

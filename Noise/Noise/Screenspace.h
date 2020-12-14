#pragma once
#include "CustomMath.h"

// All the constant values used in rendering the screen
namespace screen
{
    constexpr Whole width = 255; // Width of display buffer
    constexpr Whole height = 255; // Height of display buffer
    constexpr Whole area = width * height;
    const double diagonal = sqrt(width + height);
}
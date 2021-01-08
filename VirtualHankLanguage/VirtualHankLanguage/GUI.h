#pragma once
#include <string>
#include <cstdarg>
#include <stdio.h>

char DensityRamp(const double);
char DensityRamp(const float);

enum class Color8Bit
{
	/*************************
	* Key
	* ===
	* Name            FG  BG
	* Black           30  40
	* Red             31  41
	* Green           32  42
	* Yellow          33  43
	* Blue            34  44
	* Magenta         35  45
	* Cyan            36  46
	* White           37  47
	* Bright Black    90  100
	* Bright Red      91  101
	* Bright Green    92  102
	* Bright Yellow   93  103 // TODO use background color
	* Bright Blue     94  104
	* Bright Magenta  95  105
	* Bright Cyan     96  106
	* Bright White    97  107
	**************************/
	
	Black = 30,
	Red,
	Green,
	Yellow,
	Blue,
	Magenta,
	Cyan,
	White,

	Bright_Offset = 60,
	BG_Offset = 10,
};
std::string ColorEsc(const Color8Bit, const bool, const bool); // Produces an escape code for the desired color which can be inserted into a string

void InsertEscColor(std::string&, const Color8Bit, const size_t);

void DrawOverlay();

void SayDebug(const std::string&, const Color8Bit = Color8Bit::Yellow);
void SayComm(const std::string&, const Color8Bit = Color8Bit::Yellow);
void SayPretty(const std::string&, const Color8Bit = Color8Bit::Yellow);

#pragma once
#include <string>
#include <sstream>
#include <cstdarg>
#include <stdio.h>
#include <fstream>

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
Color8Bit operator+(Color8Bit l, Color8Bit r);
std::string ColorEsc(Color8Bit, bool, bool); // Produces an escape code for the desired color which can be inserted into a string

void InsertEscColor(std::string&, Color8Bit, size_t);

void SetPrintColorRGB(unsigned char r, unsigned char g, unsigned char b);
void SetPrintColor8Bit(Color8Bit color);
void ResetPrintColor();
void SetPrintPos(int x, int y);

extern const std::string g_overlay;

// Symbol used for positioning ancoring in overlay
enum class PosSymbol : char
{
	Comm = '>',
	Debug = '~',
	Pretty = '$',
};
struct LineData
{
	size_t pos, width;
	int x();
	int y();
};
LineData FindPositionInOverlay(PosSymbol symbol, int index);
void PrepareLine(LineData line);

void DrawOverlay();

enum class DebugColor
{
	Msg,
	Warning,
	Critical,
};
struct DebugLine
{
	std::string m_str;
	DebugColor m_color = DebugColor::Msg;
};
extern DebugLine g_debugLines[];
extern std::string g_commLines[];

void SayDebug(const std::string& str, DebugColor color = DebugColor::Msg);

void SayComm(const std::string&);

void SayPretty(const std::string, Color8Bit = Color8Bit::Yellow);
#include <Windows.h>
#include <iostream>
#pragma once

struct CNorm
{
	double r, g, b;
};
double CDot(CNorm, CNorm);

struct Color
{
	unsigned char r, g, b;

	unsigned char Gray() const;
	unsigned char RedDev() const;
	unsigned char GreenDev() const;
	unsigned char BlueDev() const;
	CNorm Normal() const;
};
double CDot(Color, Color);

struct Pixel
{
	Pixel() : color{ 0,0,0 }, x{ 0 }, y{ 0 } {};
	Pixel(Color _color, int _x, int _y) : color{ _color }, x{ _x }, y{ _y } {};

	Color color;
	int x;
	int y;
};

class ClockTime
{
private:
	// One hour is 45 seconds. A night is 4 minutes 30 seconds, or 270 seconds -- 2700 deciseconds. This can be expressed in 12 bits as 0b101010001100.
	unsigned short m_deciseconds;
	int m_pingsSinceChange;

public:
	ClockTime() : m_deciseconds{ 0u }, m_pingsSinceChange{ 0 } {};
	ClockTime(unsigned short const& deciseconds) : m_deciseconds{ deciseconds }, m_pingsSinceChange{ 0 }{};

	unsigned short const& GetDeciseconds() const; // Read-only
	unsigned short GetSeconds() const; // It takes 1 bit more than a char to describe the number of seconds in a night.
	unsigned char GetMinutes() const; // Not sure what we'd need this for, but just in case.
	unsigned char GetHour() const; // What hour of the night we are at

	unsigned short GetWholeHourDeciseconds() const; // Converts hours to deciseconds, for finding how many deciseconds we are through the current hour.
	unsigned short GetDecisecondsSinceHour() const; // Finds how many deciseconds into the current hour we are.

	void UpdateTime(unsigned short const&);
	int const& GetPingsSinceChange();
};

// What gamestate we are in (what we can see on the screen)
enum class State : unsigned char
{
	Office = 0,
	Camera = 1,
	Vent = 2,
	Duct = 3,
};

enum class Camera : unsigned char
{
	PleaseUpdate = 0, // If this is 0 we are working with un-updated data
	WestHall = 1,
	EastHall = 2,
	Closet = 3,
	Kitchen = 4,
	PirateCove = 5,
	ShowtimeStage = 6,
	PrizeCounter = 7,
	PartsAndServices = 8,
};

enum class Vent : unsigned char
{
	Inactive = 0, // Snares reset after being tripped
	WestSnare = 1,
	NorthSnare = 2,
	EastSnare = 3,
};

enum class Duct : bool
{
	West = false,
	East = true,
};

struct GameState
{
	State state; // What state we are in (office, checking cameras, ducts, vents)

	union StateData // The metadata about the state (what part of the office, which camera)
	{
		struct OfficeData
		{
			signed char officeYaw; // How far left/right we are looking [-1,1]
		};
		OfficeData od;

		struct CamData
		{
			Camera camera; // Which camera we are looking at
		};
		CamData cd;

		struct VentData
		{
			Vent ventSnare; // Which vent snare is active
		};
		VentData vd;

		struct DuctData
		{
			Duct closedDuct; // Which duct is currently closed
			POINT audioLure;
		};
		DuctData dd;
	};
	StateData stateData; // Information about the current state that can tell us how to interpret information

	// This is the type which actually stores the data we have about the gamestate
	struct GameData
	{
		ClockTime time;
		bool ventilationNeedsReset;

		bool doorsClosed[4]; // In order from left to right
		bool flashlight;
	};
	GameData gameData;

	void DisplayData();
	void Init();
};

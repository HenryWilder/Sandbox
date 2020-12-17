#pragma once

struct Color
{
	unsigned char r, g, b;

	unsigned char Gray()
	{
		return (unsigned char)(((int)r + (int)g + (int)b) / 3);
	}
};

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

public:
	ClockTime() : m_deciseconds{ 0u } {};
	ClockTime(unsigned short const& deciseconds) : m_deciseconds{ deciseconds } {};

	unsigned short const& GetDeciseconds(); // Read-only
	unsigned short GetSeconds(); // It takes 1 bit more than a char to describe the number of seconds in a night.
	unsigned char GetMinutes(); // Not sure what we'd need this for, but just in case.
	unsigned char GetHour(); // What hour of the night we are at

	unsigned short GetWholeHourDeciseconds(); // Converts hours to deciseconds, for finding how many deciseconds we are through the current hour.
	unsigned short GetDecisecondsSinceHour(); // Finds how many deciseconds into the current hour we are.

	void IncrementTime(unsigned short const& amount = 1);
};

struct GameState
{
	/*Tells information about how the data should be read (What data is known vs what is uncertain/unknowable at the current state)
	* -Examples-
	* ==========
	* Information which can be semi-permanently known:
	* -When Funtime Foxy's next show will be, given we have checked since it last updated.
	* -Whether Afton has attacked yet or not
	* 
	* Information which cannot be known at certain gamestates:
	* -How close Freddy or Lefty is to the office (would need to check the cameras)
	* -How wound the music box is (can still be inferred, however)
	*/
	enum State : unsigned char
	{
		Office = 0,
		Camera = 1,
		Vent = 2,
		Duct = 3,
	};
	State m_state; // What state we are in (office, checking cameras, ducts, vents)

	union StateData // The metadata about the state (what part of the office, which camera)
	{
		signed char officeYaw; // How far left/right we are looking [-1,1]

		enum Camera : unsigned char
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
		Camera camera; // Which camera we are looking at

		enum Vent : unsigned char
		{
			Inactive = 0, // Snares reset after being tripped
			WestSnare = 1,
			NorthSnare = 2,
			EastSnare = 3,
		};
		Vent ventSnare; // Which vent snare is active

		enum Duct : unsigned char
		{
			NA = 0, // Both are open (only true at the beginning of the game)
			West = 1,
			East = 2,
		};
		Duct ductClosed; // Which duct is currently closed
	};
	StateData m_metaState; // Information about the current state that can tell us how to interpret information

	// Flags referring to what data we can test in the current gameState
	// This is dependent on both the state and state metadata (in the case of the cameras/office)
	enum GameDataFlag : unsigned short
	{
		// Basically always
		InGameTime = 0,
		PowerLevel = 0,
		PowerUsage = 0,
		NoiseLevel = 0,
		OfficeTemp = 0,
		TokenCount = 0,
		// Requires internal memory
		DFanActive = 0,
		MuManAnger = 0,
		// Only in office
		MaskOnFace = 0,
		WhoInPCove = 0,
		NMarioOpac = 0, // Nightmarione
		NMBBStandn = 0,
		GoldFreddy = 0,
		WetFlrSign = 0,
		GostFreddy = 0,
		// Only on camera
		ScndSystem = 0,
		FFShowtime = 0,
		// Only in vents
		// Only in ducts
	};
	unsigned short m_testableDataFlags;

	/*
	// This is the type which actually stores the data we have about the gamestate
	class Data
	{

	};
	Data m_data;
	*/
};
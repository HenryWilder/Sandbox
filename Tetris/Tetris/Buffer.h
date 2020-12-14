#pragma once
#include "Math.h"
#include <iostream>

using namespace ext;



/*
   _____   _______   _____    _    _    _____   _______   _    _   _____    ______ 
  / ____| |__   __| |  __ \  | |  | |  / ____| |__   __| | |  | | |  __ \  |  ____|
 | (___      | |    | |__) | | |  | | | |         | |    | |  | | | |__) | | |__   
  \___ \     | |    |  _  /  | |  | | | |         | |    | |  | | |  _  /  |  __|  
  ____) |    | |    | | \ \  | |__| | | |____     | |    | |__| | | | \ \  | |____ 
 |_____/     |_|    |_|  \_\  \____/   \_____|    |_|     \____/  |_|  \_\ |______|
 
 */

// A color with RGB value
struct Color
{
	// Creates an RGB color
	Color(Byte rIn, Byte gIn, Byte bIn);
	// Creates a grayscale color
	Color(Byte value);
	// Creates a color value of black
	Color();

	Byte r, g, b;
};
// Float RGB color (0-1)
struct FColor
{
	// Converts a Color to a Float color
	FColor(Color color);
	// Creates an RGB color
	FColor(float rIn, float gIn, float bIn);
	// Creates a grayscale color
	FColor(float value);
	// Creates a color value of black
	FColor();

	float r, g, b;

	Color ToColor();
};

struct CColor
{
	CColor(Color color);
	CColor(FColor color);
	CColor();

	char r[3];
	char g[3];
	char b[3];

	Color ToColor();
	char Comp(Byte i); // Returns the componenets of the color as if the entire CColor were a single array
};

struct Pixel
{
	Pixel(Color col);
	Pixel(Byte rIn, Byte gIn, Byte bIn);
	Pixel();

	Pixel(const Pixel&) = default;

	char ESC[7];
	char pcolor[11];
	char endFormat;
	char character[2];

	// Assign the colors of the pixel as a single ColorRGB struct
	void Set(Color col);
	// Assign the colors of the pixel as a series of red, green, and blue values
	void Set(Byte redd, Byte gren, Byte bluu);
	// Assign the colors of the pixel as a grayscale value
	void Set(Byte val);

	// Resets the pixel to its initial value ("\x1b[48;2;0;0;0m  ")
	void Init();

	// Identifies the pixel to a return symbol
	void EndRow();
	void EndRow(bool isReturn);

	// Identifies the pixel as the end of the buffer, stopping the cout when it is reached.
	void CutoffPix();
};

/*
  _____    ______   _   _   _____    ______   _____
 |  __ \  |  ____| | \ | | |  __ \  |  ____| |  __ \
 | |__) | | |__    |  \| | | |  | | | |__    | |__) |
 |  _  /  |  __|   | . ` | | |  | | |  __|   |  _  /
 | | \ \  | |____  | |\  | | |__| | | |____  | | \ \
 |_|  \_\ |______| |_| \_| |_____/  |______| |_|  \_\
 */

struct FrameBuffer
{
	// Input
	static const Whole g_frameWidth = 9; // The width of the visible section of frame buffer, for calculational purposes
	static const Whole g_frameHeight = 9; // The height of the frame buffer
	// Calculated
	static const Whole g_frameArea = g_frameWidth * g_frameHeight; // The area of the frame buffer for mathematical purposes
	static const Whole g_frameExtWidth = g_frameWidth + 1; // The width of the frame with the newline pixel incorporated, for array indexing purposes
	static const Whole g_frameTotalSize = g_frameExtWidth * g_frameHeight + 1; // The last valid index in the pixel array

	static Rect g_framespace; // Framespace visibile area
	static Rect g_framespaceExt; // Framespace usible area

	FrameBuffer();

	Pixel m_frameBuffer[g_frameTotalSize + 1]; // The actual buffer itself ( + 1 not for cutoff pixel, but for size of buffer)

private:
	// Get a particular pixel
	Pixel GetPixel(int index);
	Pixel GetPixel(int x, int y);
	Pixel GetPixel(Coord position);

	// Set a pixel using another pixel
	void SetPixel(int index, Pixel pixel);
	void SetPixel(int x, int y, Pixel pixel);
	void SetPixel(Coord position, Pixel pixel);

	// Set a pixel at an index to match a pixel at another index
	void ExchangePixel(int startIndex, int endIndex);
	void ExchangePixel(int startX, int startY, int endX, int endY);
	void ExchangePixel(Coord start, Coord end);

public:
	// Return the index at the specified coordinates
	int Index(int x, int y);
	int Index(Coord position);

	// Returns a bool of whether the specified coordinates are valid
	// Note: There is no overload for index, as x-position cannot be found from an already indexed point.
	// There would be no way to tell whether the point is invalid or wrapped around the screen during indexing.
	bool Valid(int x, int y);
	bool Valid(Coord position);

	// Set the color of a particular pixel
private:
	void SetPixelColor(int index, Color color = Color(0, 0, 0));
public:
	void SetPixelColor(int x, int y, Color color = Color(0, 0, 0));
	void SetPixelColor(Coord position, Color color = Color(0, 0, 0));

	// Set the text of a pixel (char[2])
	void SetPixelStr(int x, int y, char text[2]);
	void SetPixelChar(int index, bool offset, char character);

private:
	// Cut and paste an entire row down 1
	void CutRow(int row);
public:
	// Drop the entire board down a row starting at the input row
	void DropRow(int rowToDrop);

	// Draw the entire buffer
	void Draw();
};

extern FrameBuffer g_frame;

/*
   _____    ____    _        _        _____    _____   _____    ____    _   _
  / ____|  / __ \  | |      | |      |_   _|  / ____| |_   _|  / __ \  | \ | |
 | |      | |  | | | |      | |        | |   | (___     | |   | |  | | |  \| |
 | |      | |  | | | |      | |        | |    \___ \    | |   | |  | | | . ` |
 | |____  | |__| | | |____  | |____   _| |_   ____) |  _| |_  | |__| | | |\  |
  \_____|  \____/  |______| |______| |_____| |_____/  |_____|  \____/  |_| \_|

*/

class CollisionBuffer
{
	// Input
	static Coord g_playspaceOffset; // The Offset of the collision buffer
	static const Whole g_playspaceWidth = 6; // The width of the visible section of collision buffer
	static const Whole g_playspaceHeight = 9; // The height of the collision buffer
	// Calculated
	static const Whole g_playspaceArea = g_playspaceWidth * g_playspaceHeight; // The area of the collision buffer
	static Rect g_playspace; // Offset range for the playspace


	bool m_collisionBuffer[g_playspaceArea + 1]; // The actual buffer itself

public:

	// Return the index at the specified coordinates
	int Index(int x, int y);
	int Index(Coord position);

	// Returns a bool of whether the specified coordinates are valid
	bool Valid(int x, int y);
	bool Valid(Coord position);

	// Get a particular pixel
	bool Get(int index);
	bool Get(int x, int y);
	bool Get(Coord position);

	// Set the value of a particular pixel
	void Set(Coord pos, bool value);
	void Set(int x, int y, bool value);
	void Set(int index, bool value);

	// Imprint a collision at the specified point
	void FillPos(const Coord& pos);

	// Remove any collision that might be at the specified point
	void ClearPos(const Coord& pos);

	// Cut and paste an entire row down 1
	void CutRow(int row);

	// Clears the buffer
	void Reset();

private:
	
};

void Solid(int x, int y, Color color); // Draws a static, colliding pixel to both the collision & frame buffers

extern CollisionBuffer g_collision;
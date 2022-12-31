#pragma once
#include <array>
#include <raylib.h>
#include <raymath.h>

struct GridPoint
{
	// The actual location of the gridpoint in the world
	Vector3 truePosition = Vector3Zero();
};
constexpr uint8_t g_gridWidth = 20;
constexpr uint8_t g_gridHeight = 10;
constexpr uint8_t g_gridDepth = 20;
extern GridPoint g_grid[g_gridWidth][g_gridHeight][g_gridDepth];

void InitGrid();

bool IsGridIntegerPositionWithinBounds(int x, int y, int z);

// Return is meaningless but does not error when out of bounds.
// Always test if the point is in bounds first, using IsGridIntegerPositionWithinBounds(int, int, int).
Vector3 TruePositionAtGridIntegerPosition(int x, int y, int z);

bool IsGridPositionWithinBounds(Vector3 gridPosition);

// Return is meaningless but does not error when out of bounds.
// Always test if the point is in bounds first, using IsGridPositionWithinBounds(Vector3).
Vector3 TruePositionAtGridPosition(Vector3 gridPosition);

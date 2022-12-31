#include <exception>
#include "vector_field.h"

GridPoint g_grid[g_gridWidth][g_gridHeight][g_gridDepth];

// Returns nullptr if out of bounds
// The point (0,0,0) is at a corner, not the center
GridPoint* GridPointAtIntegerGridPosition(uint8_t x, uint8_t y, uint8_t z)
{
	if (x >= g_gridWidth || y >= g_gridHeight || z >= g_gridDepth)
		return nullptr;
	return &g_grid[x][y][z];
}

void InitGrid()
{
	for (uint8_t x = 0; x < g_gridWidth; ++x)
	{
		for (uint8_t y = 0; y < g_gridHeight; ++y)
		{
			for (uint8_t z = 0; z < g_gridDepth; ++z)
			{
				GridPoint& point = *GridPointAtIntegerGridPosition(x,y,z);

				point.truePosition.x = (float)x;
				point.truePosition.y = (float)y;
				point.truePosition.z = (float)z;
			}
		}
	}
}

bool IsGridIntegerPositionWithinBounds(int x, int y, int z)
{
	return
		x >= 0 && x < g_gridWidth &&
		y >= 0 && y < g_gridHeight &&
		z >= 0 && z < g_gridDepth;
}

Vector3 TruePositionAtGridIntegerPosition(int x, int y, int z)
{
#if _DEBUG
	if (!IsGridIntegerPositionWithinBounds(x, y, z))
		throw new std::exception("Using " __FUNCTION__ " on an out of bounds position can result in undefined behavior.\nIt is safe to continue.");
#endif

	GridPoint* gridPoint = GridPointAtIntegerGridPosition(x, y, z);
	if (gridPoint == nullptr)
		return Vector3Zero();
	return gridPoint->truePosition;
}

bool IsGridPositionWithinBounds(Vector3 gridPosition)
{
	int xLo = (int)(gridPosition.x - 0.5f); // Low x
	int xHi = (int)(gridPosition.x + 0.5f); // High x
	int yLo = (int)(gridPosition.y - 0.5f); // Low y
	int yHi = (int)(gridPosition.y + 0.5f); // High y
	int zLo = (int)(gridPosition.z - 0.5f); // Low z
	int zHi = (int)(gridPosition.z + 0.5f); // High z

	return
		IsGridIntegerPositionWithinBounds(xLo, yLo, zLo) &&
		IsGridIntegerPositionWithinBounds(xHi, yHi, zHi);
}

Vector3 TruePositionAtGridPosition(Vector3 gridPosition)
{
#if _DEBUG
	if (!IsGridPositionWithinBounds(gridPosition))
		throw new std::exception("Do not use " __FUNCTION__ " on an out of bounds position. Continuing will result in an error.");
#endif

	int xLo = (int)(gridPosition.x - 0.5f); // Low x
	int xHi = (int)(gridPosition.x + 0.5f); // High x

	int yLo = (int)(gridPosition.y - 0.5f); // Low y
	int yHi = (int)(gridPosition.y + 0.5f); // High y

	int zLo = (int)(gridPosition.z - 0.5f); // Low z
	int zHi = (int)(gridPosition.z + 0.5f); // High z

	float xAlpha = gridPosition.x - (float)xLo;
	float yAlpha = gridPosition.y - (float)yLo;
	float zAlpha = gridPosition.z - (float)zLo;

	/*****************************
	*
	*	Point index key
	* 
	*	0 = 000 = (0,0,0)
	*	1 = 001 = (0,0,1)
	*	2 = 010 = (0,1,0)
	*	3 = 011 = (0,1,1)
	*	4 = 100 = (1,0,0)
	*	5 = 101 = (1,0,1)
	*	6 = 110 = (1,1,0)
	*	7 = 111 = (1,1,1)
	*
	*       (1,1,1)
	*          \
	*	3-------7
	*   |\      |\
	*   | 2-------6 - (1,1,0)
	*   1-|-----5 |
	*    \|      \|
	*     0-------4
	*      \       \
	*    (0,0,0) (1,0,0)
	*
	*****************************/
	Vector3 truePoints[8] =
	{
		g_grid[xLo][yLo][zLo].truePosition,
		g_grid[xLo][yLo][zHi].truePosition,
		g_grid[xLo][yHi][zLo].truePosition,
		g_grid[xLo][yHi][zHi].truePosition,
		g_grid[xHi][yLo][zLo].truePosition,
		g_grid[xHi][yLo][zHi].truePosition,
		g_grid[xHi][yHi][zLo].truePosition,
		g_grid[xHi][yHi][zHi].truePosition,
	};

	/***********************************************************************************************************
	* 
	*	Interpolating an arbitrary hexahedron
	* 
	*	1. Find the polygon created by interpolating each point across the x-axis connection.
	*	   These points will be named according to the 0-x points.
	* 
	*		3--*----7          3
	*		|\      |\         |\         3------2
	*		| 2--*----6   ->   | 2   ~~   |      |
	*		1-|*----5 |        1 |   ~~   |      |
	*		 \|      \|         \|        1------0
	*		  0--*----4          0
	* 
	*	2. Slice the resulting polygon into a line along the z-axis by interpolating the y-axis connections.
	*	   These points will be named according to the 0-y points.
	* 
	*		3------2
	*		*      *   ->   1------0
	*		|      |
	*		1------0
	* 
	*	3. Interpolate the points on this line along its z-axis connection.
	*	   This point will be named 0, as it is the only point.
	* 
	*		1-*----0   ->   0
	* 
	*	This point is your location in spacetime.
	* 
	***********************************************************************************************************/

	Vector3 polygon[4] =
	{
		Vector3Lerp(truePoints[0], truePoints[4], xAlpha),
		Vector3Lerp(truePoints[1], truePoints[5], xAlpha),
		Vector3Lerp(truePoints[2], truePoints[6], xAlpha),
		Vector3Lerp(truePoints[3], truePoints[7], xAlpha),
	};
	Vector3 line[2] =
	{
		Vector3Lerp(polygon[0], polygon[2], yAlpha),
		Vector3Lerp(polygon[1], polygon[3], yAlpha),
	};
	Vector3 point = Vector3Lerp(line[0], line[1], zAlpha);

	return point;
}

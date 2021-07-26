#include <raylib.h>
#include "Templates.h"
#include <tuple>
#include <vector>

struct Int2
{
	int x, y;
};

struct Space
{
	bool m_visited = false;
	int m_gcost = 0; // Distance from start
	int m_hcost = 0; // Distance from end
	int m_fcost = 0; // gcost + hcost
};

constexpr int worldWidth = 8;
constexpr int worldHeight = 8;
constexpr int worldScale = 64;

void DrawSpace(int x, int y, Color color)
{
	x *= worldScale;
	y *= worldScale;
	DrawRectangle(x, y, worldScale, worldScale, color);
	DrawRectangleLines(x, y, worldScale, worldScale, {30,30,30,255});
}

int dist(long x1, long y1, long x2, long y2)
{
	long x = labs(x2 - x1);
	long y = labs(y2 - y1);
	long dist = ((x * x) + (y * y));
	return lround(sqrt(dist) * 10.0);
}

int main()
{
	int windowWidth = worldWidth * worldScale;
	int windowHeight = worldHeight * worldScale;
	InitWindow(windowWidth, windowHeight, "Pathfinder");

	bool world[worldWidth * worldHeight]
	{
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,1,1,1,0,0,
		0,0,0,0,0,1,0,0,
		0,0,0,0,0,1,0,0,
		0,0,0,0,0,1,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
	};
	Space spaces[worldWidth * worldHeight];
	
	Int2 a{ 2, 4 };
	Int2 b{ 6, 1 };
	Int2 pt = a;

	std::vector<Int2> path;

	auto Index = [](int x, int y) { return y * worldWidth + x; };
	auto Index2 = [](Int2 pt) { return pt.y * worldWidth + pt.x; };
	auto Value = [&](int x, int y)
	{
		int index = Index(x,y);
		spaces[index].m_fcost =
			(spaces[index].m_gcost = dist(x, y, a.x, a.y)) +
			(spaces[index].m_hcost = dist(x, y, b.x, b.y));
		spaces[index].m_visited = true;
		return spaces[index].m_fcost;
	};

	path.push_back(a);
	while (!WindowShouldClose())
	{
		if (!(path.back().x == b.x && path.back().y == b.y))
		{
			std::pair<int, Int2> best = { INT_MAX, {} }; // We want the lowest value possible
			// 3x3 area around the point
			for (int y = -1; y <= 1; ++y)
			{
				for (int x = -1; x <= 1; ++x)
				{
					if (x==0 && y==0) continue;
					if (!world[Index(path.back().x + x, path.back().y + y)])
					{
						Int2 pt = { path.back().x + x, path.back().y + y };
						int val = Value(pt.x, pt.y);
						if (val < best.first)
						{
							best.first = val;
							best.second = pt;
						}
					}
				}
			}
			path.push_back(best.second);
		}

		BeginDrawing();
		{
			ClearBackground(RAYWHITE);

			int i = 0;
			for (int y = 0; y < worldHeight; ++y)
			{
				for (int x = 0; x < worldWidth; ++x)
				{
					bool isWall = world[i];
					DrawSpace(x, y, (isWall ? BLACK : WHITE));
					if (spaces[i].m_visited)
					{
						DrawSpace(x, y, GREEN);
						DrawText(TextFormat("%#2i  %#2i", spaces[i].m_gcost, spaces[i].m_hcost), x * worldScale + 2, y * worldScale + 2, worldScale / 4, BLACK);
						DrawText(TextFormat(" %#2i ", spaces[i].m_fcost), x * worldScale, (y * worldScale) + (worldScale / 3), worldScale / 2, BLACK);
					}
					++i;
				}
			}
			if ((path.back().x == b.x && path.back().y == b.y))
			{
				for (Int2& pt : path)
				{
					DrawSpace(pt.x, pt.y, YELLOW);
				}
			}

			DrawSpace(path.back().x, path.back().y, RED);

			i = 0;
			for (int y = 0; y < worldHeight; ++y)
			{
				for (int x = 0; x < worldWidth; ++x)
				{
					if (spaces[i].m_visited)
					{
						DrawText(TextFormat("%#2i  %#2i", spaces[i].m_gcost, spaces[i].m_hcost), x * worldScale + 2, y * worldScale + 2, worldScale / 4, BLACK);
						DrawText(TextFormat(" %#2i ", spaces[i].m_fcost), x * worldScale, (y * worldScale) + (worldScale / 3), worldScale / 2, BLACK);
					}
					++i;
				}
			}

			DrawSpace(a.x, a.y, BLUE);
			DrawSpace(b.x, b.y, BLUE);
		}
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
#pragma once
#include <raylib.h>
#include <vector>

class Notecard;

class Thread
{
public:
	Thread(Color color, Notecard* start, Notecard* end) : color(color), start(start), end(end) {}

	constexpr static float thickness = 4.0f;

	Color color;
	Notecard* start;
	Notecard* end;

	Vector2 StartPosition() const;
	Vector2 EndPosition() const;

	void Draw() const;
	bool IsHovered(Vector2) const;
};

extern std::vector<Thread*> g_threads;

void CreateThread(Color color, Notecard* start, Notecard* end);

void DestroyThread(Thread* thread);

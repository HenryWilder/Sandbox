#pragma once
#include "Rendering.h"
#include <vector>

struct Vec2
{
	double x, y;
	Vec2 operator+(const Vec2& p2) const;
	operator POINT() const { return { (long)(x + .5), (long)(y + .5)}; }
};

struct Color
{
	unsigned char r, g, b;
	operator COLORREF() const { return RGB(r, g, b); }
};

class Particle
{
	Vec2 pos;
	Vec2 vel;
	Color color;
	int maxLife;
	int life;
public:
	Particle();
	Particle(Vec2 startPos, Vec2 vel);
	~Particle();
	void IncrementFrame();
};

class Emitter
{
	Vec2 pos;
	Particle particleTemplate;
	std::vector<Particle*> existing;
	int maxParticles;

	void Frame();
public:
	Emitter();
	~Emitter();
	void Run();
};

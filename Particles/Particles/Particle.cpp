#include "Particle.h"

Vec2 Vec2::operator+(const Vec2& p2) const
{
	return { x + p2.x, y + p2.y };
}

Particle::Particle()
{
	pos = {};
	vel = {};
	color = {};
	maxLife = 0;
	life = 0;
}

Particle::~Particle()
{
}

void Particle::IncrementFrame()
{
	Vec2 newPos = pos + vel;
	DrawPixel(pos, color);
	DrawPixel(newPos, color);
	pos = newPos;
	++life;
}

void Emitter::Frame()
{
	for (Particle* particle : existing)
	{
		particle->IncrementFrame();
	}

}

Emitter::Emitter()
{
	pos = {};
	particleTemplate = Particle();
	existing;
	maxParticles;
}

Emitter::~Emitter()
{
}

void Emitter::Run()
{
	while (true)
	{
		Frame();
	}
}

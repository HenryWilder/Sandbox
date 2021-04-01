#pragma once
#include <raylib.h>
#include <raymath.h>

struct PhysMaterial
{
	float rebound;
	float friction;
	float density;
};

struct Transform
{
	Transform(Vector2 position, float rotation) : pos(position), rot(rotation) {}

	Vector2 pos;
	float rot;
};

Transform Update(Transform transform, Vector2 velocity, Vector2 gravity, float mass, PhysMaterial material, Rectangle collision)
{

}

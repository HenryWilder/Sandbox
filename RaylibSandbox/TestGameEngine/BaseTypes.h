#pragma once

struct Vector2;
struct Texture;
typedef Texture Texture2D;
Texture2D LoadTexture(const char*); // Because of this, raylib.h MUST be included before BaseTypes.h in all places where BaseTypes.h is included!

// Wrapper for all classes - handles swapping between different types and memory management
class Object
{
	Object() {}
	~Object() {}
};

// A physical object in the game world - has position
class Actor
{
public:
	Actor() : b_hidden(false), pos() {};
	~Actor() {}

	virtual void Draw() const = 0;

protected:
	bool b_hidden;
	Vector2 pos;
};

// An actor with movement and interactivity
class Entity : public Actor
{
	Entity() {}
	~Entity() {}
};

// An entity with movement
class Character : public Entity
{

};
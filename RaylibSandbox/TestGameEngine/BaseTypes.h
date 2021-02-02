#pragma once

struct Vector2;
struct Texture;
typedef Texture Texture2D;
Texture2D LoadTexture(const char*); // Because of this, raylib.h MUST be included before BaseTypes.h in all places where BaseTypes.h is included!

class Actor
{
public:
	Actor() : b_hidden(false), texture(&LoadTexture("missing.png")), pos() {};
	Actor(const Texture2D& _texture) : b_hidden(false), texture(&_texture), pos() {};
	Actor(const Texture2D& _texture, Vector2 _pos) : b_hidden(false), texture(&_texture), pos(_pos) {};

	virtual void Draw() const = 0;

protected:
	bool b_hidden;
	const Texture2D* texture;
	Vector2 pos;
};
class StaticActor : public Actor
{
public:
	void Draw() const;
};
class Entity : public Actor
{
public:
	void Draw() const;

protected:
	float rot;
};
#pragma once
#include <raylib.h>
#include <vector>

class Actor;

// Holds information about sensory targets
class Sense_Base
{
public:
	Sense_Base()
	{
	}

	~Sense_Base()
	{
	}

private:
	Actor* instigator;
	Vector2 source;
	float age;
	float strength;
};

// Visual, blocked by visibility collision, direct, high-precision but easily broken
class Sight : public Sense_Base
{
public:
	Sight()
	{
	}

	~Sight()
	{
	}

private:

};

// Auditory, quickly attenuates with distance, travels through cover, low-precision but omni-directional
class Hearing : public Sense_Base
{
public:
	Hearing()
	{
	}

	~Hearing()
	{
	}

private:

};

// The uneasy feeling that someone is around even when you cannot see nor hear them
class ESP : public Sense_Base
{
public:
	ESP()
	{
	}

	~ESP()
	{
	}

private:

};

// Object permanence and guessing based on map knowledge. A target may not be seen or heard, but the last time they were their route was guessed.
class Prediction : public Sense_Base
{
public:
	Prediction()
	{
	}

	~Prediction()
	{
	}

private:

};

// This agent may not even be aware of the target personally, but a teammate made them aware of the target
class Communication : public Sense_Base
{
public:
	Communication()
	{
	}

	~Communication()
	{
	}

private:

};

std::pinned_vector<Sense_Base*> g_allSenses;

std::vector<Sense_Base*> UpdateSenses(float maxAge, float radius, Vector2 position, )
{
	std::vector<Sense_Base*> output;

}

// Knowledgebase for ai agents
// POLYMORPH BASE CLASS - Make specializations of this
class Blackboard
{

};

// An AI decision maker
class Agent
{
	Blackboard* m_blackboard;
};
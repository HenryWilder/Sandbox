#pragma once
#include <raylib.h>
#include <vector>
#include <map>
#include "interp.h"

template<typename T>
__interface ValueDelegate
{
public:
	virtual T GetValue() const = 0;
};

template<typename T>
class Range_Constant : public ValueDelegate<T>
{
public:
	T value;

	T GetValue() const override
	{
		return value;
	}
};

template<typename T>
class RangeDelegate : public ValueDelegate<T>
{
	InterpDelegate interp;
	T Interpolate(T a, T b, double t)
	{
		interp(a, b, t);
	}
	virtual T GetValue() const = 0;
};

template<typename T>
class Range_Random : public RangeDelegate<T>
{
public:
	T min;
	T max;

	T GetValue() const override
	{
		return interp(min, max, (double)rand() / (double)RAND_MAX);
	}
};

template<typename T>
class Range_Dynamic : public RangeDelegate<T>
{
public:
	std::map<float, T> timeline;

	T GetValue() const override
	{
		for ()
	}
};

class Particle_Base
{
	float lifetime;
};

class Particle
{
	float age;
};

struct Emission
{
	Particle_Base particle;
};

class Emitter
{
	std::vector<Emission> emissionInfo;
};
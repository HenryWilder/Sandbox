#pragma once
#include <vector>

struct ParticleProperties
{
	Vector2 position;
	Vector2 velocity, velocityVariation;
	Color colorBegin, colorEnd;
	float sizeBegin, sizeEnd, sizeVariation;
	float lifeTime = 1.0f;
};

class Emitter
{
public:
	Emitter();

	void OnUpdate(float dt);
	void OnRender();

	void Emit(const ParticleProperties& properties);

#if _DEBUG
	enum DebugFlags : short
	{
		Prop_Position		= 0b0000000001,
		Prop_Velocity		= 0b0000000010,
		Prop_ColorBeg		= 0b0000000100,
		Prop_ColorEnd		= 0b0000001000,
		Prop_Rotation		= 0b0000010000,
		Prop_SizeBeg		= 0b0000100000,
		Prop_SizeEnd		= 0b0001000000,
		Prop_Lifetime		= 0b0010000000,
		Prop_LifeRem		= 0b0100000000,
		Prop_Active			= 0b1000000000,

		Prop_Max			= 0b10000000000,
	};
	void Debug(int maxParticles, int x, int y, int fontSize, Color color, short flags = 1);
#endif

private:
	struct Particle
	{
		Vector2 position;
		Vector2 velocity;
		Color colorBegin, colorEnd;
		float rotation = 0.0f;
		float sizeBegin, sizeEnd;

		float lifeTime = 1.0f;
		float lifeRemaining = 0.0f;

		bool active = false;
	};
	std::vector<Particle> m_ParticlePool;
	uint32_t m_PoolIndex = 999;

	Shader m_ParticleShader;
	Color m_ParticlesTint;
};
#include <raylib.h>
#include "random.h"
#include "vecmath.h"
#include "interp.h"
#include "particles.h"

Emitter::Emitter()
{
	m_ParticlePool.resize(1000);
	m_ParticleShader = GetShaderDefault();
	m_ParticlesTint = WHITE;
}

void Emitter::OnUpdate(float dt)
{
	for (Particle& particle : m_ParticlePool)
	{
		if (!particle.active)
			continue;

		if (particle.lifeRemaining <= 0.0f)
		{
			particle.active = false;
			continue;
		}

		particle.lifeRemaining -= dt;
		particle.position += particle.velocity * dt;
		particle.rotation += 0.01f * dt;
	}
}

void Emitter::OnRender()
{
	BeginShaderMode(m_ParticleShader);

	for (const Particle& particle : m_ParticlePool)
	{
		if (!particle.active)
			continue;

		float lifePercent = particle.lifeRemaining / particle.lifeTime;
		float scale = /*Lerp(particle.sizeEnd, particle.sizeBegin, lifePercent)*/particle.sizeBegin;
		Color color = ColorCombine(particle.colorEnd, particle.colorEnd, lifePercent);
		DrawTextureEx(GetTextureDefault(), particle.position - scale / 2, particle.rotation, scale, color);
	}

	EndShaderMode();
}

void Emitter::Emit(const ParticleProperties& properties)
{
	Particle& particle = m_ParticlePool[m_PoolIndex];
	particle.active = true;
	particle.position = properties.position;
	particle.rotation = Random::Float() * 2.0f * PI;

	// Velocity
	particle.velocity = properties.velocity;
	particle.velocity.x = properties.velocityVariation.x * (Random::Float() - 0.5f);
	particle.velocity.y = properties.velocityVariation.y * (Random::Float() - 0.5f);

	// Color
	particle.colorBegin = properties.colorBegin;
	particle.colorEnd = properties.colorEnd;

	particle.lifeTime = properties.lifeTime;
	particle.lifeRemaining = properties.lifeTime;
	particle.sizeBegin = properties.sizeBegin + properties.sizeVariation * (Random::Float() - 0.5f);
	particle.sizeEnd = properties.sizeEnd;

	m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
}

#if _DEBUG
void Emitter::Debug(int maxParticles, int x, int y, int fontSize, Color color, short flags)
{
	enum Format
	{
		FLT,
		VEC2,
		COL,
		BOOl,
	};
	static const char* fmt[] =
	{
		"{ %1.3f, %1.3f }", // Vector2
		"{ %#03ui, %#03ui, %#03ui, %#03ui }", // Color
		"%1.3f", // Float
		"%c", // Bool
	};
	static const char* var[] =
	{
		"position", // Vector2
		"velocity", // Vector2
		"colorBeg", // Color
		"colorEnd", // Color
		"rotation", // Float
		"sizeBeg",  // Float
		"sizeEnd",  // Float
		"lifeTime", // Float
		"lifeRem",  // Float
		"active",   // Bool
	};
	static auto Property = [](short p) {
		short f = (short)(p >= 2) + (short)(p >= 4) + (short)(p == 9);
		return TextFormat("%8s = %s\n", var[p], fmt[f]);
	};

	size_t count = 0;
	for (int i = 0; i < 10; ++i)
	{
		count += (flags & (1 << i));
	}

	if (!count)
		return;

	std::string format;
	format.reserve(34ull * count + 1ull);
	for (int i = 0; i < 10; ++i)
	{
		if (flags & (1 << i))
			format.append(Property(i));
	}
	
	int p = 0;

	for (const Particle& particle : m_ParticlePool)
	{
		if (p >= maxParticles)
			break;

		if (!(flags & Prop_Active))
		{
			if (!particle.active)
				continue;
		}

		const char* information = TextFormat(format.c_str(), particle.position.x, particle.position.y, particle.velocity.x, particle.velocity.y);
		DrawText(information, x, y + (fontSize)*p, fontSize, color);

		++p;
	}
}
#endif
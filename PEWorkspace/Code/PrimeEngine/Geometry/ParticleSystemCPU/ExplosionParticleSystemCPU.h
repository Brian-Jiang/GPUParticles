#ifndef __PYENGINE_2_0_EXPLOSION_PARTICLE_CPU_H__
#define __PYENGINE_2_0_EXPLOSION_PARTICLE_CPU_H__

#include "ParticleSystemCPU.h"

namespace PE {

struct ExplosionParticle
{
	Vector3 m_pos;
	Vector3 m_vel;
	Vector2 m_size;
	float m_age;
	PrimitiveTypes::UInt32 m_type;

	ExplosionParticle()
	{
		m_pos = Vector3();
		m_vel = Vector3();
		m_size = Vector2();
		m_age = 0.0f;
		m_type = 1;
	}
};

struct ExplosionParticleSystemCPU : public ParticleSystemCPU
{
	ExplosionParticleSystemCPU(PE::GameContext &context, PE::MemoryArena arena):ParticleSystemCPU(context, arena)
	{}

	void create(Vector3 spawnPos, Vector2 size = Vector2(240.0f, 240.0f), float ageOfEmitter = 0.0f);
};
}; // namespace PE
#endif

#ifndef __PYENGINE_2_0_SMOKE_PARTICLE_CPU_H__
#define __PYENGINE_2_0_SMOKE_PARTICLE_CPU_H__

#include "ParticleSystemCPU.h"

namespace PE {

struct SmokeParticle
{
	Vector3 m_pos;
	Vector3 m_vel;
	Vector2 m_size;
	float m_age;
	PrimitiveTypes::UInt32 m_type;

	SmokeParticle()
	{
		m_pos = Vector3();
		m_vel = Vector3();
		m_size = Vector2();
		m_age = 0.0f;
		m_type = 1;
	}
};

struct SmokeParticleSystemCPU : public ParticleSystemCPU
{
	SmokeParticleSystemCPU(PE::GameContext &context, PE::MemoryArena arena):ParticleSystemCPU(context, arena)
	{}
	void create(Vector3 spawnPos, Vector2 size = Vector2(240.0f, 240.0f), float ageOfEmitter = 0.0f);
};
}; // namespace PE
#endif

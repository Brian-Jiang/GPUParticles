#ifndef __PYENGINE_2_0_FIRE_PARTICLE_CPU_H__
#define __PYENGINE_2_0_FIRE_PARTICLE_CPU_H__

#include "ParticleSystemCPU.h"

namespace PE {

struct FireParticle
{
	Vector3 m_pos;
	Vector2 m_size;
	float m_age;
	PrimitiveTypes::UInt32 m_type;

	FireParticle()
	{
		m_pos = Vector3();
		m_size = Vector2();
		m_age = 0.0f;
		m_type = 1;
	}
};

struct FireParticleSystemCPU : public ParticleSystemCPU
{
	FireParticleSystemCPU(PE::GameContext &context, PE::MemoryArena arena):ParticleSystemCPU(context, arena)
	{}
	virtual void create();
};
}; // namespace PE
#endif

#ifndef __PYENGINE_2_0_BLADE_FIRE_PARTICLE_CPU_H__
#define __PYENGINE_2_0_BLADE_FIRE_PARTICLE_CPU_H__

#include "ParticleSystemCPU.h"

namespace PE {

struct BladeFireParticle
{
	Vector3 m_curPos;
	Vector3 m_prevPos0;
	Vector3 m_prevPos1;
	Vector2 m_size;
	float m_age;
	PrimitiveTypes::UInt32 m_type;
	float m_randomOffset;

	BladeFireParticle()
	{
		m_curPos = Vector3();
		m_prevPos0 = Vector3();
		m_prevPos1 = Vector3();
		m_size = Vector2();
		m_age = 0.0f;
		m_type = 1;
		m_randomOffset = 0.0f;
	}
};

struct BladeFireParticleSystemCPU : public ParticleSystemCPU
{
	BladeFireParticleSystemCPU(PE::GameContext &context, PE::MemoryArena arena):ParticleSystemCPU(context, arena)
	{
	}
	virtual void create(Vector3 spawnPos, Vector2 size);
};

}; // namespace PE
#endif

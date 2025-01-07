#ifndef __PYENGINE_2_0_RAIN_PARTICLE_CPU_H__
#define __PYENGINE_2_0_RAIN_PARTICLE_CPU_H__

#include "ParticleSystemCPU.h"

namespace PE {

struct RainParticle
{
	Vector3 m_curPos;
	Vector2 m_size;
	float m_age;
	PrimitiveTypes::UInt32 m_type;
//	float m_randomOffset;

	RainParticle()
	{
		m_curPos = Vector3();
		m_size = Vector2();
		m_age = 0.0f;
		m_type = 1;
//		m_randomOffset = 0.0f;
	}
};

struct RainParticleSystemCPU : public ParticleSystemCPU
{
	RainParticleSystemCPU(PE::GameContext &context, PE::MemoryArena arena):ParticleSystemCPU(context, arena)
	{}
	virtual void create();
};
}; // namespace PE
#endif

#ifndef __PYENGINE_2_0_PARTICLE_BUFFER_CPU__
#define __PYENGINE_2_0_PARTICLE_BUFFER_CPU__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../../Utils/Array/Array.h"
#include "PrimeEngine/Math/Vector3.h"
// Sibling/Children includes

struct ParticleCPU
{
	Vector3 m_pos;
	Vector3 m_velocity;
	Vector2 m_size;
	float m_age;
	PrimitiveTypes::UInt32 m_type;

	ParticleCPU()
	{
		m_pos = Vector3();
		m_velocity = Vector3();
		m_size = Vector2();
		m_age = 0.0f;
		m_type = 1;
	}
};

template <typename Particle_t>
struct ParticleBufferCPU : PE::PEAllocatableAndDefragmentable
{
	ParticleBufferCPU(PE::GameContext &context, PE::MemoryArena arena) : m_values(context, arena, 0)
	{ }

	// Reads the specified buffer from file
	void createBuffer()
	{
	}

	// start of the vertex data in memory
	void *getStartAddress(){return &m_values[0];}

	PrimitiveTypes::UInt32 getByteSize(){return m_values.m_size * sizeof(Particle_t);}

	Array<Particle_t> m_values;
};

#endif

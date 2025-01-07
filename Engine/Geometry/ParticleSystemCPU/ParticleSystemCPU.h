#ifndef __PYENGINE_2_0_PARTICLE_SYSTEM_CPU__
#define __PYENGINE_2_0_PARTICLE_SYSTEM_CPU__

#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/Utils/StringOps.h"
#include "../../Utils/Array/Array.h"
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"
#include "../MaterialCPU/MaterialSetCPU.h"

#include "../PositionBufferCPU/PositionBufferCPU.h"
#include "../TexCoordBufferCPU/TexCoordBufferCPU.h"
#include "../IndexBufferCPU/IndexBufferCPU.h"
#include "../NormalBufferCPU/NormalBufferCPU.h"
#include "ParticleBufferCPU.h"

// Sibling/Children includes

#define PE_MAX_PARTICLE_COUNT_IN_DRAW_CALL 1048576

namespace PE {

	struct ParticleEmitter {
        Vector4 m_position;
        float emissionRate;

	};

	struct Particle {
		Vector4 m_position;
        Vector4 m_velocity;
        Vector4 m_acceleration;
		float m_age;
        float m_lifeTime;
	};

// This class is a simple POD struct that holds all the CPU information about the mesh before it is given to DX to be put in GPU memory
struct ParticleSystemCPU : PE::PEAllocatableAndDefragmentable
{
	ParticleSystemCPU(PE::GameContext &context, PE::MemoryArena arena)
	{
		m_arena = arena; m_pContext = &context;
	}

	virtual void create()
	{

		PrimitiveTypes::UInt32 n = 1;
		m_hParticleBufferCPU = Handle("PARTICLE_BUFFER_CPU", sizeof(ParticleBufferCPU<ParticleCPU>));
		ParticleBufferCPU<ParticleCPU> *ppbcpu = new(m_hParticleBufferCPU) ParticleBufferCPU<ParticleCPU>(*m_pContext, m_arena);
		ppbcpu->m_values.reset(n);

		/*
		m_hTexCoordBufferCPU = Handle(TEXCOORD_BUFFER_CPU, sizeof(TexCoordBufferCPU));
		TexCoordBufferCPU *ptcbcpu = new(m_hTexCoordBufferCPU) TexCoordBufferCPU();
		ptcbcpu->m_values.reset(nsprites * 2);

		m_hOrientationTexCoordBufferCPU = Handle(TEXCOORD_BUFFER_CPU, sizeof(TexCoordBufferCPU));
		TexCoordBufferCPU *porienttcbcpu = new(m_hOrientationTexCoordBufferCPU) TexCoordBufferCPU();
		porienttcbcpu->m_values.reset(nsprites * 2);
		*/
		for (PrimitiveTypes::UInt32 ip = 0; ip < n; ip++)
		{
			ppbcpu->m_values.add(ParticleCPU());
			ppbcpu->m_values[ppbcpu->m_values.m_size-1].m_pos = Vector3(0, 500.0f, 0);
			ppbcpu->m_values[ppbcpu->m_values.m_size-1].m_size.m_x = 100.0f;
			ppbcpu->m_values[ppbcpu->m_values.m_size-1].m_size.m_y = 50.0f;
			ppbcpu->m_values[ppbcpu->m_values.m_size-1].m_type = 0;
		}
		
		m_hMaterialSetCPU = Handle("MATERIAL_SET_CPU", sizeof(MaterialSetCPU));
		MaterialSetCPU *pmscpu = new(m_hMaterialSetCPU) MaterialSetCPU(*m_pContext, m_arena);
		pmscpu->createSetWithOneTexturedMaterial("grass.dds", "Default");
	}
	
	Handle m_hParticleBufferCPU;
	
	Handle m_hMaterialSetCPU;
	PE::MemoryArena m_arena; PE::GameContext *m_pContext;
};
}; // namespace PE {
#endif

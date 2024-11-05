#define NOMINMAX

#include "RainParticleSystemCPU.h"

namespace PE {

void RainParticleSystemCPU::create()
{
	m_hParticleBufferCPU = Handle("PARTICLE_BUFFER_CPU", sizeof(ParticleBufferCPU<RainParticle>));
	ParticleBufferCPU<RainParticle> *ppbcpu = new(m_hParticleBufferCPU) ParticleBufferCPU<RainParticle>(*m_pContext, m_arena);
	PrimitiveTypes::UInt32 n = 1;
	ppbcpu->m_values.reset(n);

	for (PrimitiveTypes::UInt32 ip = 0; ip < n; ip++)
	{
		ppbcpu->m_values.add(RainParticle());
		ppbcpu->m_values[ppbcpu->m_values.m_size-1].m_curPos = Vector3(0, 0.0f, 0.0f);
		ppbcpu->m_values[ppbcpu->m_values.m_size-1].m_size.m_x = 200.0f;
		ppbcpu->m_values[ppbcpu->m_values.m_size-1].m_size.m_y = 200.0f;
		ppbcpu->m_values[ppbcpu->m_values.m_size-1].m_type = 0;
		ppbcpu->m_values[ppbcpu->m_values.m_size-1].m_age = 100.0f;
	}
	
	m_hMaterialSetCPU = Handle("MATERIAL_SET_CPU", sizeof(MaterialSetCPU));
	MaterialSetCPU *pmscpu = new(m_hMaterialSetCPU) MaterialSetCPU(*m_pContext, m_arena);

	pmscpu->createSetWithOneDefaultMaterial();
}
}; // namespace PE

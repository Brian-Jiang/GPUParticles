#define NOMINMAX

#include "SmokeParticleSystemCPU.h"

namespace PE {
void SmokeParticleSystemCPU::create(Vector3 spawnPos, Vector2 size, float ageOfEmitter)
{
	m_hParticleBufferCPU = Handle("PARTICLE_BUFFER_CPU", sizeof(ParticleBufferCPU<SmokeParticle>));
	ParticleBufferCPU<SmokeParticle> *ppbcpu = new(m_hParticleBufferCPU) ParticleBufferCPU<SmokeParticle>(*m_pContext, m_arena);
	PrimitiveTypes::UInt32 n = 1; 
	ppbcpu->m_values.reset(n);

	for (PrimitiveTypes::UInt32 ip = 0; ip < n; ip++)
	{
		ppbcpu->m_values.add(SmokeParticle());
		ppbcpu->m_values[ppbcpu->m_values.m_size-1].m_pos = spawnPos;
		ppbcpu->m_values[ppbcpu->m_values.m_size-1].m_size.m_x = size.m_x;
		ppbcpu->m_values[ppbcpu->m_values.m_size-1].m_size.m_y = size.m_y;
		ppbcpu->m_values[ppbcpu->m_values.m_size-1].m_type = 0; //type PT_EMITTER
		ppbcpu->m_values[ppbcpu->m_values.m_size-1].m_age = ageOfEmitter;
	}
	
	m_hMaterialSetCPU = Handle("MATERIAL_SET_CPU", sizeof(MaterialSetCPU));
	MaterialSetCPU *pmscpu = new(m_hMaterialSetCPU) MaterialSetCPU(*m_pContext, m_arena);

	const char name0[255] = "eexplo001.dds";
	const char name1[255] = "eexplo002.dds";
	const char name2[255] = "eexplo003.dds";
	const char name3[255] = "eexplo004.dds";
	const char name4[255] = "eexplo005.dds";
	const char name5[255] = "eexplo006.dds";
	const char name6[255] = "eexplo007.dds";
	const char name7[255] = "eexplo008.dds";
	const char name8[255] = "eexplo009.dds";
	const char name9[255] = "eexplo010.dds";

	const char name10[255] = "eexplo011.dds";
	const char name11[255] = "eexplo012.dds";
	const char name12[255] = "eexplo013.dds";
	const char name13[255] = "eexplo014.dds";
	const char name14[255] = "eexplo015.dds";
	const char name15[255] = "eexplo016.dds";
	const char name16[255] = "eexplo017.dds";
	const char name17[255] = "eexplo018.dds";
	const char name18[255] = "eexplo019.dds";
	const char name19[255] = "eexplo020.dds";

	const char name20[255] = "eexplo021.dds";
	const char name21[255] = "eexplo022.dds";
	const char name22[255] = "eexplo023.dds";
	const char name23[255] = "eexplo024.dds";
	const char name24[255] = "eexplo025.dds";
	

	const char *names[64] = {
		name0, name1, name2, name3, name4, 
		name5, name6, name7, name8, name9, 
		name10, name11, name12, name13, name14, 
		name15, name16, name17, name18, name19,
		name20, name21, name22, name23, name24
	};
	

	pmscpu->createSetWithOneColorTextureArrayMaterial(names, 25, "Default");
}
}; // namespace PE

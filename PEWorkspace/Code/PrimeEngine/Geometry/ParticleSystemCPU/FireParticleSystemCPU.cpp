#define NOMINMAX

#include "FireParticleSystemCPU.h"

namespace PE {

void FireParticleSystemCPU::create()
{
	m_hParticleBufferCPU = Handle("PARTICLE_BUFFER_CPU", sizeof(ParticleBufferCPU<FireParticle>));
	ParticleBufferCPU<FireParticle> *ppbcpu = new(m_hParticleBufferCPU) ParticleBufferCPU<FireParticle>(*m_pContext, m_arena);
	PrimitiveTypes::UInt32 n = 100;
	ppbcpu->m_values.reset(n);

	Vector3 pos(1100.0f, 0.0f, 1000.0f);
	Vector3 dpos(0, 0, -3000.0f / n);
	PrimitiveTypes::Float32 sizeFactor = 0.1f;
	PrimitiveTypes::Float32 dSizeFactor = (1.0f - sizeFactor) / n;

	for (PrimitiveTypes::UInt32 ip = 0; ip < n; ip++)
	{
		ppbcpu->m_values.add(FireParticle());
		ppbcpu->m_values[ppbcpu->m_values.m_size-1].m_size.m_x = 720.0f * sizeFactor;
		ppbcpu->m_values[ppbcpu->m_values.m_size-1].m_size.m_y = 486.0f * sizeFactor;
		ppbcpu->m_values[ppbcpu->m_values.m_size-1].m_pos = pos + Vector3(float(rand() % 100), float(ppbcpu->m_values[ppbcpu->m_values.m_size-1].m_size.m_y / 2.0f), float(rand() %100));
		ppbcpu->m_values[ppbcpu->m_values.m_size-1].m_age = float(rand() % 40);
		ppbcpu->m_values[ppbcpu->m_values.m_size-1].m_type = 1;
		pos = pos + dpos;
		sizeFactor += dSizeFactor;
	}
	
	m_hMaterialSetCPU = Handle("MATERIAL_SET_CPU", sizeof(MaterialSetCPU));
	MaterialSetCPU *pmscpu = new(m_hMaterialSetCPU) MaterialSetCPU(*m_pContext, m_arena);

	const char name0[255] = "full_frame_fire.Full0001.dds";
	const char name1[255] = "full_frame_fire.Full0002.dds";
	const char name2[255] = "full_frame_fire.Full0003.dds";
	const char name3[255] = "full_frame_fire.Full0004.dds";
	const char name4[255] = "full_frame_fire.Full0005.dds";
	const char name5[255] = "full_frame_fire.Full0006.dds";
	const char name6[255] = "full_frame_fire.Full0007.dds";
	const char name7[255] = "full_frame_fire.Full0008.dds";
	const char name8[255] = "full_frame_fire.Full0009.dds";
	const char name9[255] = "full_frame_fire.Full0010.dds";

	const char name10[255] = "full_frame_fire.Full0011.dds";
	const char name11[255] = "full_frame_fire.Full0012.dds";
	const char name12[255] = "full_frame_fire.Full0013.dds";
	const char name13[255] = "full_frame_fire.Full0014.dds";
	const char name14[255] = "full_frame_fire.Full0015.dds";
	const char name15[255] = "full_frame_fire.Full0016.dds";
	const char name16[255] = "full_frame_fire.Full0017.dds";
	const char name17[255] = "full_frame_fire.Full0018.dds";
	const char name18[255] = "full_frame_fire.Full0019.dds";
	const char name19[255] = "full_frame_fire.Full0020.dds";

	const char name20[255] = "full_frame_fire.Full0021.dds";
	const char name21[255] = "full_frame_fire.Full0022.dds";
	const char name22[255] = "full_frame_fire.Full0023.dds";
	const char name23[255] = "full_frame_fire.Full0024.dds";
	const char name24[255] = "full_frame_fire.Full0025.dds";
	const char name25[255] = "full_frame_fire.Full0026.dds";
	const char name26[255] = "full_frame_fire.Full0027.dds";
	const char name27[255] = "full_frame_fire.Full0028.dds";
	const char name28[255] = "full_frame_fire.Full0029.dds";
	const char name29[255] = "full_frame_fire.Full0030.dds";

	const char name30[255] = "full_frame_fire.Full0031.dds";
	const char name31[255] = "full_frame_fire.Full0032.dds";
	const char name32[255] = "full_frame_fire.Full0033.dds";
	const char name33[255] = "full_frame_fire.Full0034.dds";
	const char name34[255] = "full_frame_fire.Full0035.dds";
	const char name35[255] = "full_frame_fire.Full0036.dds";
	const char name36[255] = "full_frame_fire.Full0037.dds";
	const char name37[255] = "full_frame_fire.Full0038.dds";
	const char name38[255] = "full_frame_fire.Full0039.dds";
	const char name39[255] = "full_frame_fire.Full0040.dds";

	const char name40[255] = "full_frame_fire.Full0041.dds";
	const char name41[255] = "full_frame_fire.Full0042.dds";
	const char name42[255] = "full_frame_fire.Full0043.dds";
	const char name43[255] = "full_frame_fire.Full0044.dds";
	const char name44[255] = "full_frame_fire.Full0045.dds";
	const char name45[255] = "full_frame_fire.Full0046.dds";
	const char name46[255] = "full_frame_fire.Full0047.dds";
	const char name47[255] = "full_frame_fire.Full0048.dds";
	const char name48[255] = "full_frame_fire.Full0049.dds";
	const char name49[255] = "full_frame_fire.Full0050.dds";

	const char *names[64] = {
		name0, name1, name2, name3, name4, name5, name6, name7, name8, name9,
		name10, name11, name12, name13, name14, name15, name16, name17, name18, name19,
		name20, name21, name22, name23, name24, name25, name26, name27, name28, name29,
		name30, name31, name32, name33, name34, name35, name36, name37, name38, name39,
		name40, name41, name42, name43, name44, name45, name46, name47, name48, name49
	};

	pmscpu->createSetWithOneColorTextureArrayMaterial(names, 50, "Default");
}
}; // namespace PE {

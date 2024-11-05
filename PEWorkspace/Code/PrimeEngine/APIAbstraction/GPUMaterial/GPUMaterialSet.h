#ifndef __PYENGINE_2_0_GPUMATERIALSET__
#define __PYENGINE_2_0_GPUMATERIALSET__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes


// Inter-Engine includes
#include "PrimeEngine/Render/IRenderer.h"


#include "../../Geometry/MaterialCPU/MaterialSetCPU.h"

// Sibling/Children includes
#include "GPUMaterial.h"

namespace PE {

struct GPUMaterialSet : public PE::PEAllocatableAndDefragmentable
{
	GPUMaterialSet(PE::GameContext &context, PE::MemoryArena arena)
	: m_materials(context, arena)
	{
		m_arena = arena; m_pContext = &context;
	}

	void createFromMaterialSetCPU(MaterialSetCPU &matsetcpu)
	{
		PrimitiveTypes::UInt32 numMats = matsetcpu.m_materials.m_size;

		m_materials.reset(numMats);
		for (PrimitiveTypes::UInt32 imat = 0; imat < numMats; imat++)
		{
			MaterialCPU &matcpu = matsetcpu.m_materials[imat];
			GPUMaterial gpumat(*m_pContext, m_arena);
			gpumat.createFromMaterialCPU(matcpu);
			m_materials.add(gpumat);
		}
	}

	Array<GPUMaterial> m_materials;
	PE::MemoryArena m_arena; PE::GameContext *m_pContext;
};
}; // namespace PE
#endif

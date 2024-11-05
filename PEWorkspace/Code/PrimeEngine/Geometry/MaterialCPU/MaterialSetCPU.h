#ifndef __PYENGINE_2_0_MATERIAL_SET_CPU__
#define __PYENGINE_2_0_MATERIAL_SET_CPU__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../../Utils/Array/Array.h"

// Sibling/Children includes
#include "MaterialCPU.h"

namespace PE {

struct MaterialSetCPU : PE::PEAllocatableAndDefragmentable
{
	
	MaterialSetCPU(PE::GameContext &context, PE::MemoryArena arena) : m_materials(context, arena)
	{
		m_arena = arena; m_pContext = &context;
	}

	// Reads the specified buffer from file
	void ReadMaterialSet(const char *filename, const char *package = NULL);

	void createSetWithOneDefaultMaterial();

	void createSetWithOneTexturedMaterial(const char *textureFilename, const char *package, ESamplerState customSamplerState = SamplerState_Count);

	void createSetWithOneColorTextureArrayMaterial(const char * filenames[], PrimitiveTypes::UInt32 nFilenames, const char *package);

	void createSetWithOneTexturedMaterialWithGlow(const char *colorTextureFilename, const char *glowTextureFilename, const char *package);

	struct MaterialSetStats
	{
		MaterialSetStats();

		PrimitiveTypes::Bool hasAllColorMaps;
		PrimitiveTypes::Bool hasAllColorCubeMaps;
		PrimitiveTypes::Bool hasAllNormalMaps;
		PrimitiveTypes::Bool hasAllSpecularMaps;
		PrimitiveTypes::Bool hasAllGlowMaps;
	
		PrimitiveTypes::Bool setHasOneColorMap;
		PrimitiveTypes::Bool setHasOneColorCubeMap;
		PrimitiveTypes::Bool setHasOneNormalMap;
		PrimitiveTypes::Bool setHasOneSpecularMap;
		PrimitiveTypes::Bool setHasOneGlowMap;
	};

	void prepareStats();

	void removeMapsOfFamily(TextureFamily::TextureFamily_ f);

	Array<MaterialCPU> m_materials;

	PE::MemoryArena m_arena; PE::GameContext *m_pContext;

	MaterialSetStats m_stats;
};
}; // namespace PE
#endif

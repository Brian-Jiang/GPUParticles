#ifndef __PYENGINE_2_0_GPUMATERIAL__
#define __PYENGINE_2_0_GPUMATERIAL__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/Render/IRenderer.h"
#include "../../Geometry/MaterialCPU/MaterialCPU.h"
#include "../Texture/Texture.h"
#include "../Texture/GPUTextureManager.h"

// Sibling/Children includes

namespace PE {

// forward declaration
namespace Components{ struct DrawList; };

struct GPUMaterial : public PE::PEAllocatableAndDefragmentable
{
	GPUMaterial(PE::GameContext &context, PE::MemoryArena arena)
	: m_textures(context, arena)
	{
		m_arena = arena; m_pContext = &context;
	}

	void removeAllTexturesOfFamily(TextureFamily::TextureFamily_ family);

	void createFromMaterialCPU(MaterialCPU &matcpu);

	void createShaderActions(Components::DrawList *pDrawList);
	
	Vector4 m_diffuse;
	Vector3 m_specular;
	Vector3 m_emissive;
	PrimitiveTypes::Float32 m_shininess;

	// store in the same way as on GPU
	Vector4 m_xHasNrm_yHasSpec_zHasGlow_wHasDiff;

	Array<Handle> m_textures;

	PE::MemoryArena m_arena; PE::GameContext *m_pContext;
	char m_dbgName[128];
	
};

}; // namespace PE

#endif

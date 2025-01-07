#ifndef __PYENGINE_2_0_MATERIAL_CPU__
#define __PYENGINE_2_0_MATERIAL_CPU__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/Math/Vector3.h"
#include "PrimeEngine/Math/Vector4.h"
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../../Utils/Array/Array.h"
#include "../../Utils/PEString.h"
#include "PrimeEngine/APIAbstraction/Texture/Texture.h"
// Sibling/Children includes

namespace PE {

struct MaterialCPU : PE::PEAllocatableAndDefragmentable
{
	MaterialCPU(PE::GameContext &context, PE::MemoryArena arena);

	// Reads the specified buffer from file
	void ReadMaterial(const char *filename, const char *package = NULL);

	void LoadMaterialFromLuaScript(const char *filename, const char *package = NULL);

	void createDefaultMaterial();

	void createMaterialWithColorTexture(const char *textureFilename, const char *package = NULL, ESamplerState customSamplerState = SamplerState_Count);

	void createMaterialWithOneColorTextureArrayMaterial(const char * filenames[], PrimitiveTypes::UInt32 nFilenames, const char *package);

	void createMaterialWithColorAndGlowTextures(const char *colorTextureFilename, const char *glowTextureFilename, const char *package);

	// lua interface
	static void SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM);

	static int l_CreateMaterial(lua_State* luaVM);


	Vector4 m_diffuse;
	Vector3 m_specular;
	Vector3 m_emissive;
	PrimitiveTypes::Float32 m_shininess;
	Vector3 m_reflectivity;
	Array<PEString> m_textureFilenames;
	Array<PEString> m_texturePackages;
	Array<TextureFamily::TextureFamily_> m_textureFamilies;
	Array<ESamplerState> m_customSamplerStates;
	
	PE::MemoryArena m_arena; PE::GameContext *m_pContext;
	char m_dbgName[128];
	
	char m_preferredTechName[64]; // preferred technique


};
}; // namespace PE
#endif

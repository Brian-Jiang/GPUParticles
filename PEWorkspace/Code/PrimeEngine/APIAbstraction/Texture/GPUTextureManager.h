#ifndef __PYENGINE_2_0_GPU_TEXTURE_MANAGER__
#define __PYENGINE_2_0_GPU_TEXTURE_MANAGER__

#define NOMINMAX

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes

#include "PrimeEngine/Utils/StrToHandleMap.h"

#if APIABSTRACTION_D3D9
#include "../DirectX9/D3D9_GPUBuffers/D3D9_VertexBufferGPU.h"
#elif APIABSTRACTION_D3D11
#include "../DirectX11/D3D11_GPUBuffers/D3D11_VertexBufferGPU.h"
#endif
// Sibling/Children includes
#include "Texture.h"

namespace PE {

struct GPUTextureManager : PE::PEAllocatableAndDefragmentable
{
	GPUTextureManager(PE::GameContext &context, PE::MemoryArena arena) : m_map(context, arena, 1024)
	{
		m_arena = arena; m_pContext = &context;
	}

	static void Construct(PE::GameContext &context, PE::MemoryArena arena)
	{
		s_myHandle = Handle("GPU_TEXTURE_MANAGER", sizeof(GPUTextureManager));
		/* GPUTextureManager *pTM = */ new(s_myHandle) GPUTextureManager(context, arena);
		buildRandomTexture(context, arena);
	}

	static GPUTextureManager *Instance()
	{
		return s_myHandle.getObject<GPUTextureManager>();
	}

	Handle getRandomTexture()
	{
		return s_randomTexture;
	}

	Handle createColorTextureGPU(const PrimitiveTypes::String textureFilename, const char *package, ESamplerState samplerState = SamplerState_Count);

	Handle createColorCubeTextureGPU(const PrimitiveTypes::String textureFilename, const char *package);

	Handle createColorTextureArrayGPU(const PrimitiveTypes::String textureFilenames[], PrimitiveTypes::UInt32 nTextures, const char *package);

	Handle createBumpTextureGPU(const PrimitiveTypes::String textureFilename, const char *package);

	Handle createSpecularTextureGPU(const PrimitiveTypes::String textureFilename, const char *package);

	Handle createGlowTextureGPU(const PrimitiveTypes::String textureFilename, const char *package);
	
	static void buildRandomTexture(PE::GameContext &context, PE::MemoryArena arena)
	{
		s_randomTexture = Handle("TEXTURE_GPU", sizeof(TextureGPU));
		TextureGPU *pTex = new(s_randomTexture) TextureGPU(context, arena);
		pTex->createRandom2DTexture();
	}

	static Handle s_myHandle;
	static Handle s_randomTexture;

	StrToHandleMap m_map;
	PE::MemoryArena m_arena; PE::GameContext *m_pContext;
};

};// namespace PE
#endif

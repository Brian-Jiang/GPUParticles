#include "GPUTextureManager.h"

namespace PE {

Handle GPUTextureManager::s_myHandle;
Handle GPUTextureManager::s_randomTexture;

Handle GPUTextureManager::createColorTextureGPU(const PrimitiveTypes::String textureFilename, const char *package, ESamplerState samplerState/* = SamplerState_Count*/)
{
	char path[256];
	StringOps::concat(textureFilename, package, path, 256);

	Handle res = m_map.findHandle(path);
	if (res.isValid())
		return res; // already have it
	
	res = Handle("TEXTURE_GPU", sizeof(TextureGPU));
	TextureGPU *pTex = new(res) TextureGPU(*m_pContext, m_arena);

	pTex->createColorTextureGPU(textureFilename, package, samplerState);

	m_map.add(path, res);
	return res;
}

Handle GPUTextureManager::createBumpTextureGPU(const PrimitiveTypes::String textureFilename, const char *package)
{
	char path[256];
	StringOps::concat(textureFilename, package, path, 256);

	Handle res = m_map.findHandle(path);
	if (res.isValid())
		return res; // already have it
	
	res = Handle("TEXTURE_GPU", sizeof(TextureGPU));
	TextureGPU *pTex = new(res) TextureGPU(*m_pContext, m_arena);

	pTex->createBumpTextureGPU(textureFilename, package);
	
	m_map.add(path, res);
	return res;
}

Handle GPUTextureManager::createSpecularTextureGPU(const PrimitiveTypes::String textureFilename, const char *package)
{
	char path[256];
	StringOps::concat(textureFilename, package, path, 256);

	Handle res = m_map.findHandle(path);
	if (res.isValid())
		return res; // already have it
	
	res = Handle("TEXTURE_GPU", sizeof(TextureGPU));
	TextureGPU *pTex = new(res) TextureGPU(*m_pContext, m_arena);

	pTex->createSpecularTextureGPU(textureFilename, package);

	m_map.add(path, res);
	return res;
}

Handle GPUTextureManager::createGlowTextureGPU(const PrimitiveTypes::String textureFilename, const char *package)
{
	char path[256];
	StringOps::concat(textureFilename, package, path, 256);

	Handle res = m_map.findHandle(path);
	if (res.isValid())
		return res; // already have it

	res = Handle("TEXTURE_GPU", sizeof(TextureGPU));
	TextureGPU *pTex = new(res) TextureGPU(*m_pContext, m_arena);

	pTex->createGlowTextureGPU(textureFilename, package);
	
	m_map.add(path, res);
	return res;
}


Handle GPUTextureManager::createColorCubeTextureGPU(const PrimitiveTypes::String textureFilename, const char *package)
{
	char path[256];
	StringOps::concat(textureFilename, package, path, 256);

	Handle res = m_map.findHandle(path);
	if (res.isValid())
	{
		// already have it
		return res;
	}

	res = Handle("TEXTURE_GPU", sizeof(TextureGPU));
	TextureGPU *pTex = new(res) TextureGPU(*m_pContext, m_arena);

	pTex->createColorCubeTextureGPU(textureFilename, package);

	pTex->m_family = TextureFamily::COLOR_CUBE;

	m_map.add(path, res);

	return res;
}

Handle GPUTextureManager::createColorTextureArrayGPU(const PrimitiveTypes::String textureFilenames[], PrimitiveTypes::UInt32 nTextures, const char *package)
{
	char path[256];
	StringOps::concat(textureFilenames[0], package, path, 256);

	Handle res = m_map.findHandle(path);
	if (res.isValid())
	{
		// already have it
		return res;
	}

	res = Handle("TEXTURE_GPU", sizeof(TextureGPU));
	TextureGPU *pTex = new(res) TextureGPU(*m_pContext, m_arena);
	
	pTex->createColorTextureArrayGPU(textureFilenames, nTextures, package);
	
	pTex->m_family = TextureFamily::COLOR_MAP_ARRAY;
	m_map.add(path, res);

	return res;
}

}; // namespace PE

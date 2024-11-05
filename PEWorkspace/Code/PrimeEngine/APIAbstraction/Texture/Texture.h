#ifndef __PYENGINE_2_0_TEXTURE__
#define __PYENGINE_2_0_TEXTURE__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes

#include "PrimeEngine/Render/IRenderer.h"

// Sibling/Children includes
#include "SamplerState.h"
#include "Texture_DDS_Loader_Common.h"

namespace PE {

namespace TextureFamily
{
	enum TextureFamily_
	{
		COLOR_MAP,
		NORMAL_MAP,
		SPECULAR_MAP,
		GLOW_MAP,
		COLOR_MAP_ARRAY,
		COLOR_CUBE,
	};
};

struct TextureGPU : PE::PEAllocatableAndDefragmentable
{
#if APIABSTRACTION_OGL
	GLuint gfxLoadDDSTexture(char *filename, bool genMips = true);
	GLuint gfxLoadDDSTextureCubeMap(char *filename);
#elif APIABSTRACTION_D3D9
	IDirect3DTexture9 *gfxLoadDDSTexture(char *filename, bool genMips = true);
#elif APIABSTRACTION_D3D11
	ID3D11Texture2D *gfxLoadDDSTexture(char *filename, bool genMips = true);
#elif PE_PLAT_IS_PSVITA
	SceGxmTexture gfxLoadDDSTexture(char *filename, bool genMips = true);
#endif

	TextureGPU(PE::GameContext &context, PE::MemoryArena arena)
	{
		m_arena = arena; m_pContext = &context;
		m_samplerState = SamplerState_INVALID;
		StringOps::writeToString("no name", m_name, 256);
	}
	
	void createTextureNoFamily(const char* textureFilename, const char *package = NULL);
	void createColorTextureGPU(const char * textureFilename, const char *package = NULL, ESamplerState samplerState = SamplerState_Count);
	void createBumpTextureGPU(const PrimitiveTypes::String textureFilename, const char *package = NULL);
	void createSpecularTextureGPU(const PrimitiveTypes::String textureFilename, const char *package = NULL);
	void createGlowTextureGPU(const PrimitiveTypes::String textureFilename, const char *package = NULL);

	void createDrawableIntoColorTexture(PrimitiveTypes::UInt32 w, PrimitiveTypes::UInt32 h, ESamplerState sampler);
	void createDrawableIntoColorTextureWithDepth(PrimitiveTypes::UInt32 w, PrimitiveTypes::UInt32 h, ESamplerState sampler, bool use32BitRedForDepth = false);
	void createDrawableIntoDepthTexture(PrimitiveTypes::UInt32 w, PrimitiveTypes::UInt32 h, ESamplerState sampler);

#if 0
	void createReadableColorTexture(PrimitiveTypes::UInt32 w, PrimitiveTypes::UInt32 h);
#endif
	
	void createRandom2DTexture();

	void createColorCubeTextureGPU(const char *textureFilename, const char *package = NULL);

	// not implemented for DX9
	void createColorTextureArrayGPU(const PrimitiveTypes::String textureFilenames[], PrimitiveTypes::UInt32 nTextures, const char *package = NULL);

	ESamplerState m_samplerState;
	TextureFamily::TextureFamily_ m_family;

#if APIABSTRACTION_D3D9
	D3DVIEWPORT9 m_viewport;
	union {
	LPDIRECT3DTEXTURE9 m_pTexture;
	LPDIRECT3DCUBETEXTURE9 m_pCubeTexture;
	};
	LPDIRECT3DSURFACE9 m_pSurface;

	#if D3D9_USE_RENDER_TO_SURFACE
		LPD3DXRENDERTOSURFACE m_pRenderToSurface;
	#else
		LPDIRECT3DSURFACE9 m_pDSSurface;
	#endif

#elif APIABSTRACTION_D3D11
	ID3D11ShaderResourceView *m_pShaderResourceView; // a standard texture view
	ID3D11RenderTargetView *m_pRenderTargetView; // a view for D3D to draw into this texture

	ID3D11DepthStencilView *m_DepthStencilView; // a view for D3D to draw into this texture as depth buffer (is different from normal texture)
	ID3D11ShaderResourceView *m_pDepthShaderResourceView; // a view of depth as texture
	
	D3D11_VIEWPORT m_viewport;
	ID3D11Texture2D *m_pTexture;
#elif APIABSTRACTION_OGL
	GLuint m_texture;
	GLuint m_frameBufferObject;

	struct GLViewport
	{
		GLint x, y, w, h;
		GLfloat minDepth, maxDepth;
	};

	GLViewport m_viewport;
#elif PE_PLAT_IS_PSVITA
	SceGxmTexture m_texture;
	
	struct Viewport
	{
		int x, y, w, h;
		float minDepth, maxDepth;
	};

	Viewport m_viewport;
#endif

	PE::MemoryArena m_arena; PE::GameContext *m_pContext;
	char m_name[256];
};

}; // namespace PE

#endif


// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
// Sibling includes
#include "Texture.h"
// Inter-Engine includes
#include "../../Utils/ErrorHandling.h"
#include "PrimeEngine/Math/CameraOps.h"
#include "PrimeEngine/Utils/StringOps.h"
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"
#include "PrimeEngine/Render/D3D11Renderer.h"
namespace PE {
#if 1
void TextureGPU::createDrawableIntoColorTexture(PrimitiveTypes::UInt32 w, PrimitiveTypes::UInt32 h, ESamplerState sampler)
{
	
	m_samplerState = sampler;

	StringOps::writeToString("DrawableIntoColorTexture", m_name, 256);

#	if APIABSTRACTION_D3D9
	D3D9Renderer *pD3D9Renderer = static_cast<D3D9Renderer *>(m_pContext->getGPUScreen());
	LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;
	
	m_viewport.X = 0;
	m_viewport.Y = 0;
	m_viewport.Width = w;
	m_viewport.Height = h;
	m_viewport.MinZ = 0.0f;
	m_viewport.MaxZ = 1.0f;

	IDirect3DTexture9 *pColorMap = 0;

	HRESULT hr = pDevice->CreateTexture( 
		w, 
		h, 
		1, 
		D3DUSAGE_RENDERTARGET, 
		D3DFMT_A8R8G8B8, 
		D3DPOOL_DEFAULT, 
		&this->m_pTexture,
		NULL);
	
	D3DSURFACE_DESC desc;
	m_pTexture->GetSurfaceLevel( 0, &m_pSurface );
	m_pSurface->GetDesc( &desc );

	#if D3D9_USE_RENDER_TO_SURFACE
		hr = D3DXCreateRenderToSurface(pDevice, 
			desc.Width, 
			desc.Height, 
			desc.Format, 
			false,
			D3DFMT_UNKNOWN, 
			&m_pRenderToSurface );
	#endif
		assert(SUCCEEDED(hr));

#elif APIABSTRACTION_OGL
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
    IRenderer::checkForErrors("glTexImage2D");
	glTexImage2D(
		GL_TEXTURE_2D,				// Target
		0,							// Mip-level
		GL_RGBA,						// InternalFormat
		w,							// width size
		h,							// height size
		0,							// border
		GL_RGBA,						// input pixel format
		GL_UNSIGNED_BYTE,			// input pixel type
		NULL);						// input pixels
    IRenderer::checkForErrors("glTexImage2D");
    
	SamplerState &ss = SamplerStateManager::getInstance()->getSamplerState(m_samplerState);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, ss.val_GL_TEXTURE_MIN_FILTER);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, ss.val_GL_TEXTURE_MAG_FILTER);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, ss.val_GL_TEXTURE_WRAP_S);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, ss.val_GL_TEXTURE_WRAP_T);
#if !APIABSTRACTION_IOS
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_LEVEL,0); //only one mip
#endif
	//depth related
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE_ARB,GL_COMPARE_R_TO_TEXTURE_ARB);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
	glBindTexture(GL_TEXTURE_2D, 0);

	#if APIABSTRACTION_PS3
		glGenFramebuffersOES(1, &m_frameBufferObject);
	#else
		glGenFramebuffers(1, &m_frameBufferObject);
	#endif
	#if APIABSTRACTION_PS3
		glBindFramebufferOES(GL_FRAMEBUFFER_OES, m_frameBufferObject); //set framebuffer for reading and writing. could also use GL_READ_FRAMEBUFFER to set a buffer for reading vs writing. 
	#else
		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObject); //set framebuffer for reading and writing. could also use GL_READ_FRAMEBUFFER to set a buffer for reading vs writing. 
	#endif
	//glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, w); / no need for this, since it take size of the texture passed

	#if APIABSTRACTION_PS3
		glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_texture, 0);
	#else
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
	#endif
	m_viewport.x = 0;
	m_viewport.y = 0;
	m_viewport.w = w;
	m_viewport.h = h;
	m_viewport.minDepth = 0;
	m_viewport.maxDepth = 1.0f;

	#if !APIABSTRACTION_IOS
		#if APIABSTRACTION_PS3
			assert(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) == GL_FRAMEBUFFER_COMPLETE_OES);
		#else
			assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		#endif
	#endif

	#if APIABSTRACTION_PS3
		glBindFramebufferOES(GL_FRAMEBUFFER_OES, 0); // back to default
	#else
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
	#endif
#elif APIABSTRACTION_D3D11
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = w;
	m_viewport.Height = h;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

	//ID3D11Texture2D *pColorMap = 0;

	D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width = w;
		texDesc.Height = h;
		texDesc.MipLevels = 0;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET |
		                    D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		HRESULT hr = pDevice->CreateTexture2D(&texDesc, 0, &m_pTexture);
		assert(SUCCEEDED(hr));

		// Null description means to create a view to all mipmap levels
		// using the format the texture was created with
		hr = pDevice->CreateRenderTargetView(m_pTexture, 0, &m_pRenderTargetView);
		assert(SUCCEEDED(hr));
		hr = pDevice->CreateShaderResourceView(m_pTexture, 0, &m_pShaderResourceView);
		assert(SUCCEEDED(hr));

#endif
}
#endif
}; // namespace PE

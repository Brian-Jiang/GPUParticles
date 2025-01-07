
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
namespace PE {
#if 1
void TextureGPU::createDrawableIntoColorTextureWithDepth(PrimitiveTypes::UInt32 w, PrimitiveTypes::UInt32 h, ESamplerState sampler, bool use32BitRedForDepth /* = false*/)
{
	StringOps::writeToString("createDrawableIntoColorTextureWithDepth", m_name, 256);
	m_samplerState = sampler;

#	if APIABSTRACTION_D3D9
	D3D9Renderer *pD3D9Renderer = static_cast<D3D9Renderer *>(m_pContext->getGPUScreen());
	LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;
	
	m_viewport.X = 0;
	m_viewport.Y = 0;
	m_viewport.Width = w;
	m_viewport.Height = h;
	m_viewport.MinZ = 0.0f;
	m_viewport.MaxZ = 1.0f;

	HRESULT hr = pDevice->CreateTexture(
		w, 
		h, 
		1, 
		D3DUSAGE_RENDERTARGET, 
		use32BitRedForDepth ? D3DFMT_R32F : D3DFMT_A8R8G8B8,
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
		TRUE, 
		D3DFMT_D16, 
		&m_pRenderToSurface );
#else
	//create depth stencil surface to use with this texture
	hr = pDevice->CreateDepthStencilSurface(
		w,
		h,
		D3DFMT_D24S8,
		D3DMULTISAMPLE_NONE,
		0,     // multi sample quality
		TRUE,  // Set this flag to TRUE to enable z-buffer discarding, and FALSE otherwise.
		//If this flag is set, the contents of the depth stencil buffer will be invalid
		// after calling either IDirect3DDevice9::Present or IDirect3DDevice9::SetDepthStencilSurface with a different depth surface.
		&m_pDSSurface,
		NULL);
#endif

	assert(SUCCEEDED(hr));

#elif APIABSTRACTION_OGL

	SamplerState &ss = SamplerStateManager::getInstance()->getSamplerState(m_samplerState);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
    IRenderer::checkForErrors("glTexImage2D");
    
    #if PE_PLAT_IS_IOS
        PEWARN("We are creating depth texture as 32 bit, because I could not get 16 bit depth working. If you figure it out, change it to 16 bit for better perf (hopefully!)");
    #endif
    
    glTexImage2D(
		GL_TEXTURE_2D,				// Target
		0,							// Mip-level
        #if PE_PLAT_IS_IOS
            GL_DEPTH_COMPONENT, 	// InternalFormat
        #else
            GL_DEPTH_COMPONENT16,
        #endif
        w,							// width size
		h,							// height size
		0,							// border
		GL_DEPTH_COMPONENT,						// input pixel format
        #if PE_PLAT_IS_IOS
            GL_UNSIGNED_INT,			// input pixel type
        #else
            GL_UNSIGNED_SHORT,
        #endif
		NULL);						// input pixels

    IRenderer::checkForErrors("glTexImage2D");
    
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, ss.val_GL_TEXTURE_MIN_FILTER);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, ss.val_GL_TEXTURE_MAG_FILTER);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, ss.val_GL_TEXTURE_WRAP_S);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, ss.val_GL_TEXTURE_WRAP_T);
    
#if !APIABSTRACTION_IOS
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_LEVEL,0); //only one mip

	// depth related comparison functions
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
#endif
    
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
		glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_TEXTURE_2D, texture, 0);
	#else
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
	#endif

	m_viewport.x = 0;
	m_viewport.y = 0;
	m_viewport.w = w;
	m_viewport.h = h;
	m_viewport.minDepth = 0;
	m_viewport.maxDepth = 1.0f;

	// had to comment out this assert becasue apparently with newest hardware it is actually complete too
	//assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE); // make sure API requires color texture too

	// need to cerate color texture too, otherwise framebuffer object is incomplete
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
    IRenderer::checkForErrors("glTexImage2D");
    #if PE_PLAT_IS_IOS
        if (use32BitRedForDepth)
            PEWARN("We are creating depth+color and storing dpeth in color. for ios we store it as rgba, since we cant store it as 32bit red..");
    #endif
	glTexImage2D(
		GL_TEXTURE_2D,				// Target
		0,							// Mip-level
			#if APIABSTRACTION_IOS
				use32BitRedForDepth ? GL_RGBA : GL_RGBA,				// InternalFormat
			#else
				use32BitRedForDepth ? GL_R32F : GL_RGBA,						// InternalFormat
			#endif
		
		w,							// width size
		h,							// height size
		0,							// border
		GL_RGBA,					// input pixel format
		GL_UNSIGNED_BYTE,			// input pixel type
		NULL);						// input pixels
    IRenderer::checkForErrors("glTexImage2D");
    
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
		glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_texture, 0);
	#else
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
	#endif

	#if !APIABSTRACTION_IOS
		#if APIABSTRACTION_PS3
			assert(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) == GL_FRAMEBUFFER_COMPLETE_OES);
		#else
			assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		#endif
	#endif
    
    IRenderer::checkRenderBufferComplete();
    
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
	texDesc.MipLevels = 1;
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

	// Now create depth part fo the texture
	ID3D11Texture2D *pDepthMap = 0;

	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL |
		D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	hr = pDevice->CreateTexture2D(&texDesc, 0, &pDepthMap);
	assert(SUCCEEDED(hr));

	// setting up view for rendering into depth buffer/and reading (stenciling) from it (z-buffer algorithm red/write)
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Flags = 0;//D3D11_DSV_READ_ONLY_DEPTH;

	hr = pDevice->CreateDepthStencilView(pDepthMap, &dsvDesc, &m_DepthStencilView);
	assert(SUCCEEDED(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	hr = pDevice->CreateShaderResourceView(pDepthMap, &srvDesc, &m_pDepthShaderResourceView);
	assert(SUCCEEDED(hr));

	pDepthMap->Release();
#endif
}
#endif

}; // namespace PE

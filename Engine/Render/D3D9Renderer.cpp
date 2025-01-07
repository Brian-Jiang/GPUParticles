
#define NOMINMAX

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#if APIABSTRACTION_D3D9

// Outer-Engine includes

// Inter-Engine includes
#include "D3D9Renderer.h"
#include "PrimeEngine/Utils/ErrorHandling.h"

#include "PrimeEngine/APIAbstraction/Texture/Texture.h"

// Sibling/Children includes


namespace PE {

void D3D9Renderer::setRenderTargetsAndViewportWithDepth(TextureGPU *pDestColorTex, TextureGPU *pDestDepthTex, bool clearRenderTarget, bool clearDepth)
{
	// in DX9 hardware the depth texture is part of the same surface
	// so we need to make sure we are using the same texture
	assert(pDestColorTex == pDestDepthTex);

	IDirect3DSurface9 *pRenderTarget9;
	
	if (pDestColorTex != 0)
	{
		// texture is used
		// Set the render target to be our offscreen texture
		
		#if D3D9_USE_RENDER_TO_SURFACE
		#else
			HRESULT hr;
			hr = m_pD3D9Device->SetRenderTarget( 0, pDestColorTex->m_pSurface );
			assert(SUCCEEDED(hr));
			hr = m_pD3D9Device->SetDepthStencilSurface( pDestColorTex->m_pDSSurface );
			assert(SUCCEEDED(hr));
		
		#endif

		m_pD3D9Device->SetViewport(&pDestColorTex->m_viewport);
	}
	else
	{
		// use the default device
		pRenderTarget9 = m_pRenderTarget9;
		HRESULT hr = m_pD3D9Device->SetRenderTarget(0, m_pRenderTarget9);
		assert(SUCCEEDED(hr));
		hr = m_pD3D9Device->SetDepthStencilSurface(m_pDepthStencil9);
		assert(SUCCEEDED(hr));
	
		D3DVIEWPORT9 vp;

		vp.X = 0;
		vp.Y = 0;
		vp.Width = m_width;
		vp.Height = m_height;
		vp.MinZ = 0.0f;
		vp.MaxZ = 1.0f;

		hr = m_pD3D9Device->SetViewport(&vp);
		assert(SUCCEEDED(hr));

	}
	
	//render debugging: chnage background color
	#if 0
		static float red = 0.0f;
		red += 0.001f;
		if (red > 1.0f) red = 0.0f;
		m_clearColor = D3DCOLOR_COLORVALUE(red, 0, 0, 0);
		clearRenderTarget = true;
	#endif
		
	if (clearRenderTarget)
	{
		HRESULT hr = m_pD3D9Device->Clear(0, NULL, D3DCLEAR_TARGET /*| D3DCLEAR_ZBUFFER*/, m_clearColor, 1.0f, 0);
		assert(SUCCEEDED(hr));
	}

	if (clearDepth)
	{
		HRESULT hr = m_pD3D9Device->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, m_clearColor, 1.0f, 0);
		assert(SUCCEEDED(hr));
	}

#if D3D9_USE_RENDER_TO_SURFACE
	HRESULT hr = pDestColorTex->m_pRenderToSurface->BeginScene(pDestColorTex->m_pSurface, &pDestColorTex->m_viewport);
	assert(SUCCEEDED(hr));
#else
	HRESULT hr = m_pD3D9Device->BeginScene();
	assert(SUCCEEDED(hr));
#endif
}

void D3D9Renderer::endRenderTarget(TextureGPU *pTex)
{

#if D3D9_USE_RENDER_TO_SURFACE
	if (pTex)
		pTex->m_pRenderToSurface->EndScene(0);
#else
	m_pD3D9Device->EndScene();
#endif

}

void D3D9Renderer::endFrame()
{

}

void D3D9Renderer::setRenderTargetsAndViewportWithNoDepth(TextureGPU *pDestColorTex, bool clear)
{
	IDirect3DSurface9 *pRenderTarget9;
	
	if (pDestColorTex != 0)
	{
		// texture is used

	#if D3D9_USE_RENDER_TO_SURFACE
		// here we rely on render to surface not having depth buffer
		pDestColorTex->m_pRenderToSurface->BeginScene(pDestColorTex->m_pSurface, &pDestColorTex->m_viewport);
	#else
		HRESULT hr;
		hr = m_pD3D9Device->SetRenderTarget( 0, pDestColorTex->m_pSurface );
		assert(SUCCEEDED(hr));
		hr = m_pD3D9Device->SetDepthStencilSurface( NULL );
		assert(SUCCEEDED(hr));
		m_pD3D9Device->SetViewport(&pDestColorTex->m_viewport);

		m_pD3D9Device->BeginScene();
	#endif


	}
	else
	{
		// use the default device
		pRenderTarget9 = m_pRenderTarget9;

		m_pD3D9Device->SetRenderTarget(0, m_pRenderTarget9);
		m_pD3D9Device->SetDepthStencilSurface(0);

		m_pD3D9Device->BeginScene();
		D3DVIEWPORT9 vp;

		vp.X = 0;
		vp.Y = 0;
		vp.Width = m_width;
		vp.Height = m_height;
		vp.MinZ = 0.0f;
		vp.MaxZ = 1.0f;

		m_pD3D9Device->SetViewport(&vp);
	}

	if (clear)
		m_pD3D9Device->Clear(0, NULL, D3DCLEAR_TARGET /*| D3DCLEAR_ZBUFFER*/, m_clearColor, 1.0f, 0);
}


void D3D9Renderer::setDepthStencilOnlyRenderTargetAndViewport(TextureGPU *pDestDepthTex, bool clear)
{
	// in DX9 the render-to-depth texture is just a color texture
	// it also has the zbuffer attached
	if (pDestDepthTex != 0)
	{
		// texture is used
		
		#if D3D9_USE_RENDER_TO_SURFACE
			pDestDepthTex->m_pRenderToSurface->BeginScene(pDestDepthTex->m_pSurface, &pDestDepthTex->m_viewport);
		#else
			HRESULT hr;
			hr = m_pD3D9Device->SetRenderTarget( 0, pDestDepthTex->m_pSurface );
			assert(SUCCEEDED(hr));
			hr = m_pD3D9Device->SetDepthStencilSurface( pDestDepthTex->m_pDSSurface );
			assert(SUCCEEDED(hr));
			m_pD3D9Device->SetViewport(&pDestDepthTex->m_viewport);

			m_pD3D9Device->BeginScene();
		#endif
	}
	else
	{
		// use the default device
		assert(!"Cant render to default device as depth only render");
	}

	if (clear)
		m_pD3D9Device->Clear(0, NULL, D3DCLEAR_TARGET /*| D3DCLEAR_ZBUFFER*/, 0, 1.0f, 0);

	if (clear)
		m_pD3D9Device->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

}
D3D9Renderer::D3D9Renderer(PE::GameContext &context, unsigned int width, unsigned int height)
: IRenderer(context, width, height) // create window
{
	WinApplication *pWinApp = static_cast<WinApplication*>(context.getApplication());
	m_clearColor = D3DCOLOR_COLORVALUE(0, 0, 0, 0);

	m_width = width;
	m_height = height;

	// InitD3D

	m_pD3D9 = Direct3DCreate9( D3D_SDK_VERSION );

	if( m_pD3D9 == NULL )
	{
		OutputDebugStringA("PE: ERROR: Direct3DCreate9 Failed\n");
		assert(!"PE: ERROR: Direct3DCreate9 Failed");
		return;
	}
	HRESULT hr;

	DWORD dwBehaviorFlags = 0;

	D3DDISPLAYMODE d3ddm;
	if( FAILED( m_pD3D9->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) )
	{
		OutputDebugStringA("PE: ERROR: GetAdapterDisplayMode Failed\n");
		assert(!"PE: ERROR: GetAdapterDisplayMode Failed");
		return;
	}

	if( FAILED( hr = m_pD3D9->CheckDeviceFormat(
		D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, // these settings describe what adapter to be used with (current)
		D3DUSAGE_DEPTHSTENCIL, // checked resource usage
		D3DRTYPE_SURFACE, // checked resource type
		D3DFMT_D24S8 // checked resource format
		) ) )
	{
		if( hr == D3DERR_NOTAVAILABLE )
		{
			OutputDebugStringA("PE: ERROR: 16 Bit Z-Buffer is not available for current adapter/format\n");
			assert(!"PE: ERROR: 16 Bit Z-Buffer is not available for current adapter/format");
			return;
		}
		else
		{
			assert(!"Unexpected Error");
		}
	}

	D3DCAPS9 d3dCaps;

	if( FAILED( m_pD3D9->GetDeviceCaps( D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_HAL, &d3dCaps ) ) )
	{
		// TO DO: Respond to failure of GetDeviceCaps
		return;
	}

	if( d3dCaps.VertexProcessingCaps != 0 )
	{
		dwBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		assert(!"Hardware Vertex Processing Not Supported");
	}

	OutputDebugStringA( "Hardware D3D9Caps (before CreateDevice): \n");
	OutputDebugStringA( "D3DPMISCCAPS_SEPARATEALPHABLEND : ");
	OutputDebugStringA( (d3dCaps.PrimitiveMiscCaps | D3DPMISCCAPS_SEPARATEALPHABLEND) ? "1\n" : "0\n");
	


	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(d3dpp));

	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;

	d3dpp.BackBufferFormat       = d3ddm.Format;

	d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;

	d3dpp.Windowed               = TRUE;

	d3dpp.BackBufferCount = 1;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_ONE; // D3DPRESENT_INTERVAL_IMMEDIATE;
#if PE_DETAILED_GPU_PROFILING
	d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE; // this will allow present to not wait for vsync and measure true perf
#endif
	OutputDebugStringA("PE: PROGRESS: Creating D3D Device.. ");
	hr = m_pD3D9->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
		pWinApp->m_windowHandle,
		dwBehaviorFlags, &d3dpp, &m_pD3D9Device );

	OutputDebugStringA("Done. \n");
	if( FAILED( hr ) )
	{
		OutputDebugStringA("PE: ERROR: CreateDevice Failed\n");
		assert(!"PE: ERROR: CreateDevice Failed");
		return;
	}

	//m_pD3D9Device->GetSwapChain(0, &m_pSwapChain9);
	hr = m_pD3D9Device->GetRenderTarget(0, &m_pRenderTarget9);
	assert(SUCCEEDED(hr));

	//IDirect3DSurface9 *pBackBuffer;
	//m_pSwapChain9->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	//assert(m_pRenderTarget9 == pBackBuffer); // Sanity check
	//pBackBuffer->Release();

	hr = m_pD3D9Device->GetDepthStencilSurface(&m_pDepthStencil9);
	assert(SUCCEEDED(hr));
}

void D3D9Renderer::swap(bool vsync /* = 0*/)
{
	HRESULT hr = m_pD3D9Device->Present(NULL, NULL, NULL, NULL);

	if (!SUCCEEDED(hr))
	{
		if (hr == D3DERR_DEVICELOST)
		{
			PEERROR("D3DERR_DEVICELOST\n");
		}
		else if(hr == D3DERR_DRIVERINTERNALERROR)
		{
			PEERROR("D3DERR_DRIVERINTERNALERROR\n");
		}
		else if(hr == D3DERR_INVALIDCALL)
		{
			PEERROR("D3DERR_INVALIDCALL\n");
		}
		else if(hr == D3DERR_OUTOFVIDEOMEMORY)
		{
			PEERROR("D3DERR_OUTOFVIDEOMEMORY\n");
		}
		else if(hr == E_OUTOFMEMORY)
		{
			PEERROR("E_OUTOFMEMORY\n");
		}
		else 
		{
			PEERROR("Unknown error\n");
		}
	}
}

void IRenderer::Construct(PE::GameContext &context, unsigned int width, unsigned int height)
{
	PE::Handle h("D3D9GPUSCREEN", sizeof(D3D9Renderer));
	D3D9Renderer *pScreen = new(h) D3D9Renderer(context, width, height);
	context.m_pGPUScreen = pScreen;
}



bool IRenderer::checkForErrors(const char *situation)
{
	return true;
}

void IRenderer::checkRenderBufferComplete()
{
}


void IRenderer::AcquireRenderContextOwnership(int &threadOwnershipMask)
{
	bool needAssert = (threadOwnershipMask & Threading::RenderContext) > 0;

	if (needAssert)
	{
		assert(!needAssert);
	}

	m_renderLock.lock();

	threadOwnershipMask = threadOwnershipMask | Threading::RenderContext;
}
void IRenderer::ReleaseRenderContextOwnership(int &threadOwnershipMask)
{
	assert((threadOwnershipMask & Threading::RenderContext));

	m_renderLock.unlock();

	threadOwnershipMask = threadOwnershipMask & ~Threading::RenderContext;
}

}; // namespace PE

#endif // APIAbstraction

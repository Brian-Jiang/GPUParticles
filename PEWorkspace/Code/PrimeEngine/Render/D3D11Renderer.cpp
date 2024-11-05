
#define NOMINMAX

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#if APIABSTRACTION_D3D11

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/APIAbstraction/Texture/Texture.h"

// Sibling/Children includes
#include "D3D11Renderer.h"

namespace PE {

D3D11Renderer::D3D11Renderer(PE::GameContext &context, unsigned int width, unsigned int height)
	:IRenderer(context, width, height)
{
	WinApplication *pWinApp = static_cast<WinApplication*>(context.getApplication());
	m_width = width;
	m_height = height;

	// get main function args
	char buf[255];
	char name[] = "PYENGINE_3DVISION_ON";
	DWORD res = 0;
	res = GetEnvironmentVariableA(
		name,
		buf, 255);

	m_nv3DvisionOn = res > 0;

	// InitD3D
	HRESULT hr;
	
	IDXGIFactory *pFactory = 0;
	hr = CreateDXGIFactory( __uuidof(IDXGIFactory), (void**)&pFactory );
	assert( SUCCEEDED(hr));

	IDXGIAdapter *pAdapter = 0;
	hr = pFactory->EnumAdapters(0, &pAdapter);
	assert(hr != DXGI_ERROR_NOT_FOUND);

	IDXGIOutput *pOutput = 0;
	hr = pAdapter->EnumOutputs(0, &pOutput);
	assert(hr != DXGI_ERROR_NOT_FOUND);

	UINT numModes = 512;
	DXGI_MODE_DESC descs[512];
	hr = pOutput->GetDisplayModeList(
		DXGI_FORMAT_R8G8B8A8_UNORM, //  [in]       DXGI_FORMAT EnumFormat,
		0, //  [in]       UINT Flags,
		&numModes, //   [in, out]  UINT *pNumModes,
		descs //  [out]      DXGI_MODE_DESC *pDesc
	);

	DXGI_MODE_DESC best = descs[0];
	PrimitiveTypes::Float32 maxRate = 0;
	for (PrimitiveTypes::UInt32 i = 0; i < numModes; i++)
	{
		if (descs[i].Width == width && descs[i].Height == height)
		{
			PrimitiveTypes::Float32 rate = (PrimitiveTypes::Float32)(descs[i].RefreshRate.Numerator) / (PrimitiveTypes::Float32)(descs[i].RefreshRate.Denominator);
			if (rate > maxRate)
			{
				best = descs[i];
			}
		}
	}

	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.
	DXGI_SWAP_CHAIN_DESC sd;
	// Back buffer descritpion
	DXGI_MODE_DESC bufferDesc;
	bufferDesc.Width  = width;
	bufferDesc.Height = height;


	//RefreshRate	{Numerator=119974 Denominator=1000 }	DXGI_RATIONAL

	bufferDesc.RefreshRate.Numerator = 119974;
	bufferDesc.RefreshRate.Denominator = 1000;

	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
	//bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	//bufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
	sd.BufferDesc = bufferDesc;

	// Multi-sampling properties
	DXGI_SAMPLE_DESC sampleDesc;
	// No multisampling.
	sampleDesc.Count   = 1;
	sampleDesc.Quality = 0;
	sd.SampleDesc = sampleDesc;

	// This buufer is going to be used for output only (we render into it)
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;// | DXGI_USAGE_SHADER_INPUT;

	// how many buffers to use in swap chain; 1 == double buffering, 2 == triple buffering
	sd.BufferCount = 2;

	sd.OutputWindow = pWinApp->m_windowHandle;
	// go full screen if doing 3d vision
	sd.Windowed = !m_nv3DvisionOn; //true;

	// how to swap buffers; with DXGI_SWAP_EFFECT_DISCARD we let driver to select the ost efficient way
	sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;

	// if DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH is specified
	// when going to full screen, a ode will be selected to best fit the description above
	// if it is not selected, then the current mode will be used (i.e. leave same resolution for monitor -> bigger for application)
	sd.Flags        = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// End DXGI_SWAP_CHAIN_DESC sd;

	bufferDesc.ScanlineOrdering = best.ScanlineOrdering;
	bufferDesc.Scaling = best.Scaling;
		// Create the device.---------------------------------------------------

	UINT createDeviceFlags = 0;//D3D11_CREATE_DEVICE_DEBUG;
#		if defined(DEBUG) || defined(_DEBUG)  
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG; // this will add D3D error messages to output
#		endif

	D3D_FEATURE_LEVEL level;
	D3D_FEATURE_LEVEL levelsWanted[] = 
	{ 
		//D3D_FEATURE_LEVEL_11_1, 
		D3D_FEATURE_LEVEL_11_0, 
		D3D_FEATURE_LEVEL_10_1, 
		D3D_FEATURE_LEVEL_10_0
	};
	UINT numLevelsWanted = sizeof( levelsWanted ) / sizeof( levelsWanted[0] );

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = sizeof( driverTypes ) / sizeof( driverTypes[0] );

	for (PrimitiveTypes::UInt32 iDrv = 0; iDrv < numDriverTypes; iDrv++)
	{

		HRESULT hr = D3D11CreateDeviceAndSwapChain(
			0,                 //default adapter
			driverTypes[iDrv], //D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_REFERENCE, D3D_DRIVER_TYPE_SOFTWARE, or D3D_DRIVER_TYPE_WARP.
			0,                 // no software device
			createDeviceFlags, 
			levelsWanted, // which levels we desire in order of best first, D3D_FEATURE_LEVEL_11_0
			numLevelsWanted, // need 1
			D3D11_SDK_VERSION,
			&sd,
			&m_pSwapChain,
			&m_pD3DDevice,
			&level, // TODO: pFeatureLevel
			&m_pD3DContext
		);

		if (SUCCEEDED(hr))
		{
			OutputDebugStringA("PyEgnine2.0: Created D3D Device:");
			if (level == D3D_FEATURE_LEVEL_11_0)
			{
				OutputDebugStringA("D3D_FEATURE_LEVEL_11_0\n");
			}
			else if (level == D3D_FEATURE_LEVEL_10_1)
			{
				OutputDebugStringA("D3D_FEATURE_LEVEL_10_1\n");
			}
			else if (level == D3D_FEATURE_LEVEL_10_0)
			{
				OutputDebugStringA("D3D_FEATURE_LEVEL_10_0\n");
			}
			break;
		}
		else
		{
			if (driverTypes[iDrv] == D3D_DRIVER_TYPE_HARDWARE)
			{
				OutputDebugStringA("PyEgnine2.0: WARNING: Could not created D3D device with HW driver\n");
			}
		}
	}

	// At this point the backbuffer is created, but nothing can be drawn in it
	// we need to link it to the output merger stage i.e. create a render target view

	ID3D11Texture2D *pBackBufferResource;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)(&pBackBufferResource));
	// get pointer to texture (back buffer) handle
	hr = m_pD3DDevice->CreateRenderTargetView(pBackBufferResource, 0, &m_pRenderTargetView);
	pBackBufferResource->Release(); // releasing handle


	//	hr = m_pD3DDevice->CreateShaderResourceView(pBackBufferResource, 0, &m_pSRV);
	//	assert(SUCCEEDED(hr));


	D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width = width;
		depthStencilDesc.Height = height;
		
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		// Multi-sampling properties
		DXGI_SAMPLE_DESC sampleDesc2;
			// No multisampling.
			sampleDesc2.Count   = 1;
			sampleDesc2.Quality = 0;
		depthStencilDesc.SampleDesc = sampleDesc2;

		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

	m_pD3DDevice->CreateTexture2D(
		&depthStencilDesc, 0, &m_pDepthStencilBuffer);

	m_pD3DDevice->CreateDepthStencilView(
		m_pDepthStencilBuffer, 0, &m_pDepthStencilView);

	setRenderTargetsAndViewportWithDepth();
}


void D3D11Renderer::setRenderTargetsAndViewportWithDepth(TextureGPU *pDestColorTex, TextureGPU *pDestDepthTex, bool clearRenderTarget, bool clearDepth)
{
	ID3D11RenderTargetView *renderTargets[1] = {0};
	if (pDestColorTex != 0)
	{
		renderTargets[0] = pDestColorTex->m_pRenderTargetView;
	}
	else
	{
		renderTargets[0] = m_pRenderTargetView;
	}

	m_pD3DContext->OMSetRenderTargets(1, renderTargets, pDestDepthTex ? pDestDepthTex->m_DepthStencilView : m_pDepthStencilView);
	static float x = 0;
#if 0
	x = fmodf(x + 0.01, 1.0f);
#endif
	if (pDestColorTex != 0)
	{
		m_pD3DContext->RSSetViewports(1, &pDestColorTex->m_viewport);
		
		float color[] = {0.1f+x, 0.1f, 0.1f, 0.0f};

		if (clearRenderTarget)
			m_pD3DContext->ClearRenderTargetView(pDestColorTex->m_pRenderTargetView, color);
		if (clearDepth)
			m_pD3DContext->ClearDepthStencilView(pDestDepthTex->m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
		else
	{
		D3D11_VIEWPORT vp;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			vp.Width = (float)(m_width);
			vp.Height = (float)(m_height);
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;

		m_pD3DContext->RSSetViewports(1, &vp);
		float color[] = {0.0f+x, 0.0f, 0.0f, 1.0f};
		if (clearRenderTarget)
		{
			m_pD3DContext->ClearRenderTargetView(m_pRenderTargetView, color);
		}
		if (clearDepth)
			m_pD3DContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

}

void D3D11Renderer::setRenderTargetsAndViewportWithNoDepth(TextureGPU *pDestColorTex, bool clear)
{
	ID3D11RenderTargetView *renderTargets[1] = {0};
	if (pDestColorTex != 0)
	{
		renderTargets[0] = pDestColorTex->m_pRenderTargetView;
	}
	else
	{
		renderTargets[0] = m_pRenderTargetView;
	}

	m_pD3DContext->OMSetRenderTargets(1, renderTargets, 0);

	if (pDestColorTex != 0)
	{
		m_pD3DContext->RSSetViewports(1, &pDestColorTex->m_viewport);
		
		float color[] = {0.0f, 0.0f, 0.0f, 0.0f};
		if (clear)
		{
			m_pD3DContext->ClearRenderTargetView(pDestColorTex->m_pRenderTargetView, color);
		}
	}
	else
	{
		D3D11_VIEWPORT vp;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			vp.Width = (float)(m_width);
			vp.Height = (float)(m_height);
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;

		m_pD3DContext->RSSetViewports(1, &vp);
		float color[] = {1.0f * (rand() % 100) / 100.0f, 0.0f, 0.0f, 1.0f};
		if (clear)
		{
			m_pD3DContext->ClearRenderTargetView(m_pRenderTargetView, color);
		}
	}
}


void D3D11Renderer::setDepthStencilOnlyRenderTargetAndViewport(TextureGPU *pDestDepthTex, bool clear)
{
	m_pD3DContext->OMSetRenderTargets(0, NULL, pDestDepthTex->m_DepthStencilView);
	m_pD3DContext->RSSetViewports(1, &pDestDepthTex->m_viewport);
	
	if (clear)
	{
		m_pD3DContext->ClearDepthStencilView(pDestDepthTex->m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}

void D3D11Renderer::endRenderTarget(TextureGPU *pTex)
{
	m_pD3DContext->OMSetRenderTargets(0, NULL, NULL);
}


void IRenderer::Construct(PE::GameContext &context, unsigned int width, unsigned int height)
{
	PE::Handle h("D3D11GPUSCREEN", sizeof(D3D11Renderer));
	D3D11Renderer *pScreen = new(h) D3D11Renderer(context, width, height);
	context.m_pGPUScreen = pScreen;
}

void D3D11Renderer::endFrame()
{

}

bool IRenderer::checkForErrors(const char *situation)
{
	return true;
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

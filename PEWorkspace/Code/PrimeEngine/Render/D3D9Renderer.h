#ifndef __PYENGINE_2_0_D3D9Renderer_H___
#define __PYENGINE_2_0_D3D9Renderer_H___

#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#if APIABSTRACTION_D3D9

#include "PrimeEngine/Application/WinApplication.h"

#ifdef _WIN32
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <windows.h>

#endif

// Outer-Engine includes
#include <D3DCommon.h>
#include <D3DCompiler.h>

#include <d3d9.h>

// Inter-Engine includes
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/Events/EventQueueManager.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "PrimeEngine/Render/IRenderer.h"
// Sibling/Children includes

// This class is implementation of IRenderer for D3D10

namespace PE {

// forward declaration of texture class
struct TextureGPU;

class D3D9Renderer : public IRenderer
{
public:
	
	virtual void setRenderTargetsAndViewportWithDepth(TextureGPU *pDestColorTex = 0, TextureGPU *pDestDepthTex = 0, bool clearRenderTargte = false, bool clearDepth = false);

	virtual void setDepthStencilOnlyRenderTargetAndViewport(TextureGPU *pDestDepthTex, bool clear = false);

	virtual void setRenderTargetsAndViewportWithNoDepth(TextureGPU *pDestColorTex = 0, bool clear = false);

	D3D9Renderer(PE::GameContext &context, unsigned int width, unsigned int height);	

	virtual void swap(bool vsync = 0);
	
	virtual PrimitiveTypes::UInt32 getWidth(){return m_width;}
	virtual PrimitiveTypes::UInt32 getHeight(){return m_height;}
	virtual void setClearColor(Vector4 c){m_clearColor = D3DCOLOR_COLORVALUE(c.m_x, c.m_y, c.m_z, c.m_w);}
	virtual void setVSync(bool useVsync){m_wantVSync = useVsync;}

	virtual void clear()
	{
		m_pD3D9Device->Clear(0, NULL, D3DCLEAR_TARGET /*| D3DCLEAR_ZBUFFER*/, m_clearColor, 1.0f, 0);
		clearDepthAndStencil();
	}
	
	virtual void endRenderTarget(TextureGPU *pTex);
	virtual void endFrame();

	void clearDepthAndStencil()
	{
		m_pD3D9Device->Clear(0, NULL, /*D3DCLEAR_TARGET*/D3DCLEAR_ZBUFFER, D3DCOLOR_COLORVALUE(0, 0, 0, 0), 1.0f, 0);
	}

public:
	
	//IDirect3DSwapChain9 *m_pSwapChain9;

	IDirect3DSurface9 *m_pDepthStencil9;
	IDirect3DSurface9 *m_pRenderTarget9;	
public:
	
	LPDIRECT3D9 m_pD3D9;
	LPDIRECT3DDEVICE9 m_pD3D9Device;

	PrimitiveTypes::UInt32 m_width, m_height;
	D3DCOLOR m_clearColor;
	bool m_wantVSync;
};
}; // namespace PE
#endif // APIABstraction
#endif // File guard

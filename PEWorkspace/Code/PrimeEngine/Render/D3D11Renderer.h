#ifndef __PYENGINE_2_0_D3D11Renderer_H___
#define __PYENGINE_2_0_D3D11Renderer_H___

#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#if APIABSTRACTION_D3D11

#include "PrimeEngine/Application/WinApplication.h"

//#define D3D10_IGNORE_SDK_LAYERS

#include <D3DCommon.h>
#include <D3DCompiler.h>
#include <d3d11.h>

// Inter-Engine includes
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/Events/EventQueueManager.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "PrimeEngine/Render/IRenderer.h"
// Sibling/Children includes

// This class is implementation of IRenderer for D3D11

// forward declaration of texture classs

namespace PE {

struct TextureGPU;

class D3D11Renderer : public IRenderer
{
public:

	virtual void setRenderTargetsAndViewportWithDepth(TextureGPU *pDestColorTex = 0, TextureGPU *pDestDepthTex = 0, bool clearRenderTargte = false, bool clearDepth = false);

	virtual void setDepthStencilOnlyRenderTargetAndViewport(TextureGPU *pDestDepthTex, bool clear = false);

	virtual void setRenderTargetsAndViewportWithNoDepth(TextureGPU *pDestColorTex = 0, bool clear = false);

	D3D11Renderer(PE::GameContext &context, unsigned int width, unsigned int height);

	virtual void swap(bool vsync = 0) {m_pSwapChain->Present(vsync ? 1 : 0, 0);}

	virtual PrimitiveTypes::UInt32 getWidth(){return m_width;}
	virtual PrimitiveTypes::UInt32 getHeight(){return m_height;}
	virtual void setClearColor(Vector4 color){m_clearColor = color;}
	virtual void setVSync(bool useVsync){m_wantVSync = useVsync;}
	virtual void clear()
	{
		m_pD3DContext->ClearRenderTargetView(m_pRenderTargetView, m_clearColor.m_values);
		clearDepthAndStencil();
	}

	virtual void endRenderTarget(TextureGPU *pTex);
	virtual void endFrame();

	void clearDepthAndStencil()
	{
		m_pD3DContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

public:
	IDXGISwapChain *m_pSwapChain;
	ID3D11Texture2D *m_pDepthStencilBuffer;
	ID3D11DepthStencilView *m_pDepthStencilView;
	ID3D11RenderTargetView *m_pRenderTargetView;
	
public:
	ID3D11ShaderResourceView *m_pSRV;
	ID3D11Device *m_pD3DDevice;
	ID3D11DeviceContext *m_pD3DContext;
	PrimitiveTypes::UInt32 m_width, m_height;
	PrimitiveTypes::Bool m_nv3DvisionOn;
	Vector4 m_clearColor;
	bool m_wantVSync;
friend class IRenderer;
};

}; // namepsace PE

#endif // APIABstraction
#endif // File guard

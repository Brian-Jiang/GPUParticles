#define NOMINMAX
#include "PERasterizerState.h"


#include "PrimeEngine/FileSystem/FileWriter.h"
#include "../../Lua/LuaEnvironment.h"
#include "../../Lua/EventGlue/EventDataCreators.h"
#include "PrimeEngine/Utils/ErrorHandling.h"

#include "Effect.h"
#include "PrimeEngine/Game/Common/GameContext.h"


namespace PE {


PERasterizerState::PERasterizerState(PE::GameContext &context, PE::MemoryArena arena)
{
	m_cullMode = PERasterizerCullMode_None;
	#if APIABSTRACTION_D3D11
		m_pD3D11RasterizerStateObject = NULL;
	#endif	
	m_arena = arena; m_pContext = &context;
}

static void PECullFillModeToAPICullFillMode(PERasterizerState::E_PERasterizerCullMode peVal,
	PERasterizerState::E_PERasterizerFillMode peFillVal,
	PERasterizerState::APICullModeType &outCullMode,
	PERasterizerState::APIFillModeType &outFillMode)
{
	switch(peVal)
	{
	case PERasterizerState::PERasterizerCullMode_None: outCullMode = API_CHOOSE_DX11_DX9_OGL_PSVITA(D3D11_CULL_NONE, D3DCULL_NONE, GL_FALSE, 0);break;
	case PERasterizerState::PERasterizerCullMode_Front: outCullMode = API_CHOOSE_DX11_DX9_OGL_PSVITA(D3D11_CULL_FRONT, D3DCULL_CW, GL_FRONT, 0);break;
	case PERasterizerState::PERasterizerCullMode_Back: outCullMode = API_CHOOSE_DX11_DX9_OGL_PSVITA(D3D11_CULL_BACK, D3DCULL_CCW, GL_BACK, 0);break;
	default: assert(!"This CullMode is not supported yet"); outCullMode = API_CHOOSE_DX11_DX9_OGL_PSVITA(D3D11_CULL_NONE, FALSE, GL_FALSE, 0);break;
	};

	if (peFillVal == PERasterizerState::PERasterizerFillMode_Solid)
	{
		outFillMode =  API_CHOOSE_DX11_DX9_OGL_IOS_PSVITA(D3D11_FILL_SOLID, /*probably wont compile.. change this*/D3DFILL_SOLID, GL_FILL, 0, 0);
	}
	else if (peFillVal == PERasterizerState::PERasterizerFillMode_SolidLine)
	{
		outFillMode =  API_CHOOSE_DX11_DX9_OGL_IOS_PSVITA(D3D11_FILL_SOLID, /*probably wont compile.. change this*/D3DFILL_SOLID, GL_FILL, 0, 0);
	}
	else
	{
		//todo: add wireframe option
		PEASSERT(false, "not supported");
	}
}

void PERasterizerState::setAPIValues()
{
	PECullFillModeToAPICullFillMode(m_cullMode, m_fillMode, m_apiCullMode, m_apiFillMode);

#if APIABSTRACTION_D3D11
	D3D11_RASTERIZER_DESC rasterizerState;
	memset(&rasterizerState, 0, sizeof(rasterizerState));
	rasterizerState.FillMode = m_apiFillMode; // D3D11_FILL_WIREFRAME;
	rasterizerState.CullMode = m_apiCullMode;
	rasterizerState.FrontCounterClockwise = false;
	rasterizerState.DepthClipEnable = true; // allow only 0 < z < w
	/*
	rasterizerState.DepthBias = false;
	rasterizerState.DepthBiasClamp = 0;
	rasterizerState.SlopeScaledDepthBias = 0;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.ScissorEnable = true;
	rasterizerState.MultisampleEnable = false;
	rasterizerState.AntialiasedLineEnable = false;
	*/
	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;
	HRESULT hr = pDevice->CreateRasterizerState( &rasterizerState, &m_pD3D11RasterizerStateObject);
	PEASSERT(SUCCEEDED(hr), "Error creating raterizer state");
#endif
}

void PERasterizerState::bindToPipeline(Components::Effect *pCurEffect)
{
	#if APIABSTRACTION_D3D9
	{	
		D3D9Renderer *pD3D9Renderer = static_cast<D3D9Renderer *>(m_pContext->getGPUScreen());
		LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;
		
		pDevice->SetRenderState( D3DRS_CULLMODE, m_apiCullMode );
		pDevice->SetRenderState( D3DRS_FILLMODE, m_apiFillMode );
	}

#	elif APIABSTRACTION_D3D11
	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

	pDeviceContext->RSSetState(m_pD3D11RasterizerStateObject);
	
#elif APIABSTRACTION_OGL

	// todo: set fill mode
	if (m_apiCullMode == GL_FALSE)
		glDisable(GL_CULL_FACE);
	else
	{
        
		glEnable(GL_CULL_FACE);
		glCullFace(m_apiCullMode);
	}
#if !PE_PLAT_IS_IOS
    // ios has only one polygon mode = fill
	glPolygonMode(GL_FRONT_AND_BACK, m_apiFillMode);
#endif
#elif PE_PLAT_IS_PSVITA
#endif
}

void PERasterizerStateManager::ConstructAndInitialize(PE::GameContext &context, PE::MemoryArena arena)
{
	context.m_pRaterizerStateManager = new (arena) PERasterizerStateManager();
	
	context.m_pRaterizerStateManager->Initialize(context, arena);
}

void PERasterizerStateManager::Initialize(PE::GameContext &context, PE::MemoryArena arena)
{
	// set gl winding order

	#if PE_API_IS_GL
		glFrontFace(GL_CW);
	#endif

	m_rasterizerStates = new (arena) PERasterizerState[PERasterizerState_Count];

	PERasterizerState rs(context, arena);
	rs.m_cullMode = PERasterizerState::PERasterizerCullMode_Back;
	rs.m_fillMode = PERasterizerState::PERasterizerFillMode_Solid;
	rs.setAPIValues();
	m_rasterizerStates[PERasterizerState_SolidTriBackCull] = rs;

	rs.m_cullMode = PERasterizerState::PERasterizerCullMode_None;
	rs.setAPIValues();
	m_rasterizerStates[PERasterizerState_SolidTriNoCull] = rs;

	rs.m_cullMode = PERasterizerState::PERasterizerCullMode_None;
	rs.m_fillMode = PERasterizerState::PERasterizerFillMode_SolidLine;
	rs.setAPIValues();
	m_rasterizerStates[PERasterizerState_Line] = rs;
}

PERasterizerState *PERasterizerStateManager::getRasterizerState(E_PERasterizerState state)
{
	return &m_rasterizerStates[state];
}



}; // namespace PE

#define NOMINMAX
#include "PEAlphaBlendState.h"


#include "PrimeEngine/FileSystem/FileWriter.h"
#include "../../Lua/LuaEnvironment.h"
#include "../../Lua/EventGlue/EventDataCreators.h"
#include "PrimeEngine/Utils/ErrorHandling.h"

#include "Effect.h"
#include "PrimeEngine/Game/Common/GameContext.h"


namespace PE {

PEAlphaBlendState::APIBlendOpType PEAlphaBlendState::PEBlendOpToAPIBlendOp(PEBlendOp peVal)
{
	switch(peVal)
	{
		case PEBlendOp_Add: return API_CHOOSE_DX11_DX9_OGL_PSVITA(D3D11_BLEND_OP_ADD, D3DBLENDOP_ADD, GL_FUNC_ADD, 0);
		default: assert(!"This BlendOp is not supported yet"); return API_CHOOSE_DX11_DX9_OGL(D3D11_BLEND_OP_ADD, 0, 0);
	};
}

PEAlphaBlendState::APIBlendType PEAlphaBlendState::PEBlendFactorToAPIBlendFactor(PEBlendFactor peVal)
{
	switch(peVal)
	{
#if APIABSTRACTION_D3D9
	case PEBlendFactor_0: return D3DBLEND_ZERO;
	case PEBlendFactor_1: return D3DBLEND_ONE;
	case PEBlendFactor_SrcA: return D3DBLEND_SRCALPHA;
	case PEBlendFactor_InvSrcA: return D3DBLEND_INVSRCALPHA;
	case PEBlendFactor_DestA: return D3DBLEND_DESTALPHA;
#elif APIABSTRACTION_D3D11
	case PEBlendFactor_0: return D3D11_BLEND_ZERO;
	case PEBlendFactor_1: return D3D11_BLEND_ONE;
	case PEBlendFactor_SrcA: return D3D11_BLEND_SRC_ALPHA;
	case PEBlendFactor_InvSrcA: return D3D11_BLEND_INV_SRC_ALPHA;
	case PEBlendFactor_DestA: return D3D11_BLEND_DEST_ALPHA;
#elif APIABSTRACTION_OGL
	case PEBlendFactor_0: return GL_ZERO;
	case PEBlendFactor_1: return GL_ONE;
	case PEBlendFactor_SrcA: return GL_SRC_ALPHA;
	case PEBlendFactor_InvSrcA: return GL_ONE_MINUS_SRC_ALPHA;
	case PEBlendFactor_DestA: return GL_DST_ALPHA;
#elif PE_PLAT_IS_PSVITA
#endif
	default: assert(!"This PEBlendFactor is not supported yet"); return API_CHOOSE_DX11_DX9_OGL(D3D11_BLEND_ZERO, 0, 0);
	};
}

PEAlphaBlendState::PEAlphaBlendState()
{
    memset(this, 0, sizeof(*this));
}

PEAlphaBlendState::PEAlphaBlendState(PE::GameContext &context, PE::MemoryArena arena)
{
	m_blendEnabled = false;

	m_rgbBlendOp = PEBlendOp_Add;
	m_srcRGBBlendFactor = PEBlendFactor_SrcA;
	m_dstRGBBlendFactor = PEBlendFactor_InvSrcA;

	m_useSeparateSettingForAlphaChannel = false;
	m_alphaBlendOp = PEBlendOp_Add;
	m_srcAlphaBlendFactor = PEBlendFactor_0;
	m_dstAlphaBlendFactor = PEBlendFactor_1;

	#if APIABSTRACTION_D3D11
		m_pD3D11BlendStateObject = NULL;
	#endif	
	
	m_arena = arena; m_pContext = &context;
}

void PEAlphaBlendState::setAPIValues()
{
	if (!m_blendEnabled)
	{
		return;
	}

	m_apiRGBBlendOp = PEBlendOpToAPIBlendOp(m_rgbBlendOp);
	m_apiSrcRGBBlendFactor = PEBlendFactorToAPIBlendFactor(m_srcRGBBlendFactor);
	m_apiDstRGBBlendFactor = PEBlendFactorToAPIBlendFactor(m_dstRGBBlendFactor);

	if (m_useSeparateSettingForAlphaChannel)
	{
		m_apiAlphaBlendOp = PEBlendOpToAPIBlendOp(m_alphaBlendOp);
		m_apiSrcAlphaBlendFactor = PEBlendFactorToAPIBlendFactor(m_srcAlphaBlendFactor);
		m_apiDstAlphaBlendFactor = PEBlendFactorToAPIBlendFactor(m_dstAlphaBlendFactor);
	}
	else
	{
		// set to be the same value
		m_apiAlphaBlendOp = m_apiRGBBlendOp;
		m_apiSrcAlphaBlendFactor = m_apiSrcRGBBlendFactor;
		m_apiDstAlphaBlendFactor = m_apiDstRGBBlendFactor;
	}

#if APIABSTRACTION_D3D11
	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

	D3D11_BLEND_DESC bdsc;

	// NoBlend //////////////////////////////////////////////////////////////////
	memset(&bdsc, 0, sizeof(bdsc));
	bdsc.IndependentBlendEnable = false; // use only [0] render target
	bdsc.RenderTarget[0].BlendEnable = m_blendEnabled;
	bdsc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// RGB
	bdsc.RenderTarget[0].SrcBlend = m_apiSrcRGBBlendFactor;
	bdsc.RenderTarget[0].DestBlend = m_apiDstRGBBlendFactor;
	bdsc.RenderTarget[0].BlendOp = m_apiRGBBlendOp;

	// A
	bdsc.RenderTarget[0].SrcBlendAlpha = m_apiSrcAlphaBlendFactor;
	bdsc.RenderTarget[0].DestBlendAlpha = m_apiDstAlphaBlendFactor;
	bdsc.RenderTarget[0].BlendOpAlpha = m_apiAlphaBlendOp;

	HRESULT hr = pDevice->CreateBlendState(&bdsc, &m_pD3D11BlendStateObject);

	PEASSERT(SUCCEEDED(hr), "Error creating blend state");
#elif PE_PLAT_IS_PSVITA
#endif
}

void PEAlphaBlendState::bindToPipeline(Components::Effect *pCurEffect)
{
	#if APIABSTRACTION_D3D9
	{	
		D3D9Renderer *pD3D9Renderer = static_cast<D3D9Renderer *>(m_pContext->getGPUScreen());
		LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;
		
		// apply blending settings ////////////////////////////////////////////////////////////////////////
		if (m_blendEnabled)
		{
			pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

			pDevice->SetRenderState(D3DRS_BLENDOP, m_apiRGBBlendOp);
			
			pDevice->SetRenderState(D3DRS_SRCBLEND, m_apiSrcRGBBlendFactor);
			pDevice->SetRenderState(D3DRS_DESTBLEND, m_apiDstRGBBlendFactor);

			if (m_useSeparateSettingForAlphaChannel)
			{
				pDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
				pDevice->SetRenderState(D3DRS_BLENDOPALPHA, m_apiAlphaBlendOp);
				pDevice->SetRenderState(D3DRS_SRCBLENDALPHA, m_apiSrcAlphaBlendFactor);
				pDevice->SetRenderState(D3DRS_DESTBLENDALPHA, m_apiDstAlphaBlendFactor);
			}
			else
			{
				pDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);
			}
		}
		else
		{
			pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}

       // Alpha blending reference:
       // Ad = Ad * 0 + As * 1
      /* 
       IRenderer::Instance()->getDevice()->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
       IRenderer::Instance()->getDevice()->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);
       IRenderer::Instance()->getDevice()->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
       IRenderer::Instance()->getDevice()->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO);
       */
	   
	   // threshold discard alpha test settings:
	   //IRenderer::Instance()->getDevice()->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000001);
       //IRenderer::Instance()->getDevice()->SetRenderState(D3DRS_ALPHAFUNC,   D3DCMP_ALWAYS);
       //IRenderer::Instance()->getDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	}

#	elif APIABSTRACTION_D3D11
	
	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

	pDeviceContext->OMSetBlendState(m_pD3D11BlendStateObject, NULL, 0xFFFFFFFF);
	
#elif APIABSTRACTION_OGL
	if (m_blendEnabled)
	{
		glEnable(GL_BLEND);

		if (m_useSeparateSettingForAlphaChannel)
		{
			glBlendFuncSeparate(m_apiSrcRGBBlendFactor, m_apiDstRGBBlendFactor, m_apiSrcAlphaBlendFactor, m_apiDstAlphaBlendFactor);
			glBlendEquationSeparate(m_apiRGBBlendOp, m_apiAlphaBlendOp);
		}
		else
		{
			glBlendFunc(m_apiSrcRGBBlendFactor, m_apiDstRGBBlendFactor);
			glBlendEquation(m_apiRGBBlendOp);
		}
	}
	else
	{
		glDisable(GL_BLEND);
	}

	//discarded with fixed pipeline: deprecated with gl 3
	//glAlphaFunc(GL_GREATER, 0.5);
	//glEnable(GL_ALPHA_TEST);
	//just do it in shader:
	//if(texel.a < 0.5)
	//    discard;
#endif
}

    
    
    void PEAlphaBlendStateManager::ConstructAndInitialize(PE::GameContext &context, PE::MemoryArena arena)
    {
        context.m_pAlphaBlendStateManager = new (arena) PEAlphaBlendStateManager();
        
        context.getAlphaBlendStateManager()->Initialize(context, arena);
    }
    
    void PEAlphaBlendStateManager::Initialize(PE::GameContext &context, PE::MemoryArena arena)
    {
        m_alphaBlendStates = new (arena) PEAlphaBlendState[PEAlphaBlendState_Count];
        
        {
            // NoBlend
            // RtRGB = TexRGB
            // RtA = TexA
            PEAlphaBlendState blendState(context, arena);
            blendState.m_blendEnabled = false;
            blendState.setAPIValues();
            m_alphaBlendStates[PEAlphaBlendState_NoBlend] = blendState;
        }
        
        {
            // DefaultRGBLerp_A_DestUnchanged
            // RtRGB = TexRGB * TexA + RtRGB * (1-TexA)
            // RtA = RtA
            PEAlphaBlendState blendState(context, arena);
            blendState.m_blendEnabled = true;
            blendState.m_alphaBlendOp = PEAlphaBlendState::PEBlendOp_Add;
            blendState.m_srcRGBBlendFactor = PEAlphaBlendState::PEBlendFactor_SrcA;
            blendState.m_dstRGBBlendFactor = PEAlphaBlendState::PEBlendFactor_InvSrcA;
            
            blendState.m_useSeparateSettingForAlphaChannel = true;
            blendState.m_alphaBlendOp = PEAlphaBlendState::PEBlendOp_Add;
            blendState.m_srcAlphaBlendFactor = PEAlphaBlendState::PEBlendFactor_0;
            blendState.m_dstAlphaBlendFactor = PEAlphaBlendState::PEBlendFactor_1;
            blendState.setAPIValues();
            
            m_alphaBlendStates[PEAlphaBlendState_DefaultRGBLerp_A_DestUnchanged] = blendState;
        }
        
    }
    
    PEAlphaBlendState *PEAlphaBlendStateManager::getAlphaBlendState(E_PEAlphaBlendState state)
    {
        return &m_alphaBlendStates[state];
    }
    
    
}; // namespace PE

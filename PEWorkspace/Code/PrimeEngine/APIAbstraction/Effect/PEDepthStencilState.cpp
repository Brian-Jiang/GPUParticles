#define NOMINMAX
#include "PEDepthStencilState.h"


#include "PrimeEngine/FileSystem/FileWriter.h"
#include "../../Lua/LuaEnvironment.h"
#include "../../Lua/EventGlue/EventDataCreators.h"
#include "PrimeEngine/Utils/ErrorHandling.h"

#include "Effect.h"
#include "PrimeEngine/Game/Common/GameContext.h"


namespace PE {
    PEDepthStencilState::PEDepthStencilState()
    {
        memset(this, 0, sizeof(*this));
    }
    
    PEDepthStencilState::PEDepthStencilState(PE::GameContext &context, PE::MemoryArena arena)
    {
        m_depthTestEnabled = false;
#if APIABSTRACTION_D3D11
		m_pD3D11DepthStencilStateObject = NULL;
#endif
        m_arena = arena; m_pContext = &context;
    }
    
    
    void PEDepthStencilState::setAPIValues()
    {
        #if APIABSTRACTION_D3D9
        #elif APIABSTRACTION_D3D11
        
        D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
        ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
        ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;
        
        D3D11_DEPTH_STENCIL_DESC dsDesc;
        
        // Depth test parameters
        dsDesc.DepthEnable = m_depthTestEnabled;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        
        // Stencil test parameters
        dsDesc.StencilEnable = false;
        //dsDesc.StencilReadMask = 0xFF;
        //dsDesc.StencilWriteMask = 0xFF;
        
        // Stencil operations if pixel is front-facing
        //dsDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
        //dsDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
        //dsDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
        //dsDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
        
        // Stencil operations if pixel is back-facing
        //dsDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
        //dsDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
        //dsDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
        //dsDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
        
        // Create depth stencil state
        pDevice->CreateDepthStencilState(&dsDesc, &m_pD3D11DepthStencilStateObject);
        
        #elif PE_API_IS_GL
        
        
        #endif
    }
    
    void PEDepthStencilState::bindToPipeline(Components::Effect *pCurEffect)
    {
        #if APIABSTRACTION_D3D9
        {
			D3D9Renderer *pD3D9Renderer = static_cast<D3D9Renderer *>(m_pContext->getGPUScreen());
			LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;
		
            if (m_depthTestEnabled)
                pDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
            else
                pDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS);
        }
        
        #elif APIABSTRACTION_D3D11
        D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
        ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
        ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;
        
        pDeviceContext->OMSetDepthStencilState(m_pD3D11DepthStencilStateObject, 0);
        
        #elif APIABSTRACTION_OGL
        if (m_depthTestEnabled)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }
        #endif
    }
    
    void PEDepthStencilStateManager::ConstructAndInitialize(PE::GameContext &context, PE::MemoryArena arena)
    {
        context.m_pDepthStencilStateManager = new (arena) PEDepthStencilStateManager();
        
        context.getDepthStencilStateManager()->Initialize(context, arena);
    }
    
    void PEDepthStencilStateManager::Initialize(PE::GameContext &context, PE::MemoryArena arena)
    {
        
        m_depthStencilStates = new (arena) PEDepthStencilState[PEDepthStencilState_Count];
        
        {
            PEDepthStencilState dss(context, arena);
            dss.m_depthTestEnabled = true;
            dss.setAPIValues();
            m_depthStencilStates[PEDepthStencilState_ZBuffer] = dss;
        }
        
        {
            PEDepthStencilState dss(context, arena);
            dss.m_depthTestEnabled = false;
            dss.setAPIValues();
            m_depthStencilStates[PEDepthStencilState_NoZBuffer] = dss;
        }
        
        
    }
    
    PEDepthStencilState *PEDepthStencilStateManager::getDepthStencilState(E_PEDepthStencilState state)
    {
        return &m_depthStencilStates[state];
    }
    
    
    
}; // namespace PE

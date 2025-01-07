#ifndef __pe_depthstencilstate_h__
#define __pe_depthstencilstate_h__
// This class works with index buffers in GPU memory

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes

#include "PrimeEngine/Render/IRenderer.h"


#include "PrimeEngine/Render/D3D11Renderer.h"
#include "PrimeEngine/Render/D3D9Renderer.h"
#include "PrimeEngine/Render/GLRenderer.h"

// Sibling/Children includes

namespace PE {
    namespace Components
    {
        struct Effect;
    };
    
    enum E_PEDepthStencilState
    {
        PEDepthStencilState_ZBuffer,
        PEDepthStencilState_NoZBuffer,
        
        PEDepthStencilState_Count
    };
    
    struct PEDepthStencilState : PEAllocatable
    {
        
        typedef API_CHOOSE_TYPE_DX11_DX9_OGL(D3D11_CULL_MODE, int, int) APICullModeType;
        
        PEDepthStencilState();
        
        PEDepthStencilState(PE::GameContext &context, PE::MemoryArena arena);
        
        void bindToPipeline(Components::Effect *pCurEffect);
        
        void setAPIValues();
        
        bool m_depthTestEnabled;
        
        APICullModeType m_apiCullMode;
#if APIABSTRACTION_D3D11
        ID3D11DepthStencilState * m_pD3D11DepthStencilStateObject;
#endif
        PE::MemoryArena m_arena; PE::GameContext *m_pContext;
    };
    
    struct PEDepthStencilState;
    struct PEDepthStencilStateManager : public PEAllocatable
    {
        static void ConstructAndInitialize(PE::GameContext &context, PE::MemoryArena arena);
        void Initialize(PE::GameContext &context, PE::MemoryArena arena);
        
        PEDepthStencilState *getDepthStencilState(E_PEDepthStencilState state);
        
        PEDepthStencilState *m_depthStencilStates;
    };
    
}; // namespace PE

#endif

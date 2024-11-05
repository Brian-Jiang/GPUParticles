#ifndef __pe_pealphabelndstate_h__
#define __pe_pealphabelndstate_h__
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

/// abstract way of storing alpha blending state such as blend op, src and dest alpha values
    struct PEAlphaBlendState : public PEAllocatable
{
	
	typedef API_CHOOSE_TYPE_DX11_DX9_OGL(D3D11_BLEND_OP, int, int) APIBlendOpType;
	typedef API_CHOOSE_TYPE_DX11_DX9_OGL(D3D11_BLEND, int, int) APIBlendType;

    PEAlphaBlendState();

	PEAlphaBlendState(PE::GameContext &context, PE::MemoryArena arena);

	void bindToPipeline(Components::Effect *pCurEffect);
	
	void setAPIValues();

	enum PEBlendOp
	{
		PEBlendOp_Add, // default, example: source * source_alpha + dest * dest_alpha
	};

	enum PEBlendFactor
	{
		PEBlendFactor_0,
		PEBlendFactor_1,
		PEBlendFactor_SrcA,
		PEBlendFactor_InvSrcA,
		PEBlendFactor_DestA,
	};

	PEBlendOp m_alphaBlendOp;
	PEBlendFactor m_srcAlphaBlendFactor;
	PEBlendFactor m_dstAlphaBlendFactor;

	PEBlendOp m_rgbBlendOp;
	PEBlendFactor m_srcRGBBlendFactor;
	PEBlendFactor m_dstRGBBlendFactor;

	bool m_blendEnabled : 1;
	bool m_useSeparateSettingForAlphaChannel : 1;

	static APIBlendOpType PEBlendOpToAPIBlendOp(PEBlendOp peVal);
	static APIBlendType PEBlendFactorToAPIBlendFactor(PEBlendFactor peVal);

	APIBlendOpType m_apiAlphaBlendOp;
	APIBlendType m_apiSrcAlphaBlendFactor;
	APIBlendType m_apiDstAlphaBlendFactor;

	APIBlendOpType m_apiRGBBlendOp;
	APIBlendType m_apiSrcRGBBlendFactor;
	APIBlendType m_apiDstRGBBlendFactor;

#if APIABSTRACTION_D3D11
	ID3D11BlendState * m_pD3D11BlendStateObject;
#elif PE_PLAT_IS_PSVITA
#endif
	PE::MemoryArena m_arena; PE::GameContext *m_pContext;
};

enum E_PEAlphaBlendState
{
    PEAlphaBlendState_NoBlend,
    PEAlphaBlendState_DefaultRGBLerp_A_DestUnchanged,
    PEAlphaBlendState_Count
};
    
struct PEAlphaBlendStateManager : public PEAllocatable
{
    static void ConstructAndInitialize(PE::GameContext &context, PE::MemoryArena arena);
	void Initialize(PE::GameContext &context, PE::MemoryArena arena);
    
	PEAlphaBlendState *getAlphaBlendState(E_PEAlphaBlendState state);
    
	PEAlphaBlendState *m_alphaBlendStates;
        
};
    
}; // namespace PE

#endif

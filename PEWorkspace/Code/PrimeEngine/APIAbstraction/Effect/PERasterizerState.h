#ifndef __pe_perasterizerstate_h__
#define __pe_perasterizerstate_h__
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

enum E_PERasterizerState
{
	PERasterizerState_SolidTriBackCull,
	PERasterizerState_SolidTriNoCull,
	PERasterizerState_Line,

	PERasterizerState_Count
};

struct PERasterizerState : PEAllocatable
{
	
	typedef API_CHOOSE_TYPE_DX11_DX9_OGL(D3D11_CULL_MODE, int, int) APICullModeType;
	typedef API_CHOOSE_TYPE_DX11_DX9_OGL(D3D11_FILL_MODE, int, int) APIFillModeType;

	PERasterizerState(){}

	PERasterizerState(PE::GameContext &context, PE::MemoryArena arena);

	void bindToPipeline(Components::Effect *pCurEffect);

	void setAPIValues();

	enum E_PERasterizerCullMode
	{
		PERasterizerCullMode_None   = 0,
		PERasterizerCullMode_Front  = 1,
		PERasterizerCullMode_Back   = 2
	};

	enum E_PERasterizerFillMode
	{
		PERasterizerFillMode_Solid   = 0,
		PERasterizerFillMode_SolidLine
	};

	E_PERasterizerCullMode m_cullMode;
	E_PERasterizerFillMode m_fillMode;

	APICullModeType m_apiCullMode;
	APIFillModeType m_apiFillMode;
#if APIABSTRACTION_D3D11
	ID3D11RasterizerState * m_pD3D11RasterizerStateObject;
#endif
	PE::MemoryArena m_arena; PE::GameContext *m_pContext;
};

struct PERasterizerState;
struct PERasterizerStateManager : public PEAllocatable
{
	static void ConstructAndInitialize(PE::GameContext &context, PE::MemoryArena arena);
	void Initialize(PE::GameContext &context, PE::MemoryArena arena);

	PERasterizerState *getRasterizerState(E_PERasterizerState state);

	PERasterizerState *m_rasterizerStates;
};

}; // namespace PE

#endif

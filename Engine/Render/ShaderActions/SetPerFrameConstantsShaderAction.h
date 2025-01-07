#ifndef __PYENGINE_2_0_SetPerFrameConstantsShaderAction_H__
#define __PYENGINE_2_0_SetPerFrameConstantsShaderAction_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"

// Sibling/Children includes
#include "ShaderAction.h"

namespace PE {

namespace Components{
struct Effect;
}

struct SetPerFrameConstantsShaderAction : ShaderAction
{
	SetPerFrameConstantsShaderAction(PE::GameContext &context, PE::MemoryArena arena)
	{
		m_arena = arena; m_pContext = &context;
	}

	virtual ~SetPerFrameConstantsShaderAction() {}

	virtual void bindToPipeline(Components::Effect *pCurEffect = NULL) ;
	virtual void unbindFromPipeline(Components::Effect *pCurEffect = NULL) ;
	virtual void releaseData() ;
	PE::MemoryArena m_arena; PE::GameContext *m_pContext;
	
#if PE_PLAT_IS_PSVITA
	static void * s_pBuffer;
#	elif APIABSTRACTION_D3D11
		static ID3D11Buffer * s_pBuffer; // the buffer is first filled with data and then bound to pipeline
		                          // unlike DX9 where we just set registers
#	endif
	// the actual data that is transferred to video memory
	struct Data {
		union{
			struct {
				float	gGameTimes[2];
				UINT32 gGameFrame1;
				UINT32 padding1;
			};

			struct {
				float		gGameTime,  // x
					gFrameTime; // y
                UINT32 gGameFrame2; // z
                UINT32 padding2;
			};
			
			
		};
	} m_data;

	
};

}; // namespace PE
#endif

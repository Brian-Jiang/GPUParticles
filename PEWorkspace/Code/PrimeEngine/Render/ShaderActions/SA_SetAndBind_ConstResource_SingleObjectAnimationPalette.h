#ifndef __PYENGINE_2_0_SetSingleObjectAnimationPaletteShaderAction_H__
#define __PYENGINE_2_0_SetSingleObjectAnimationPaletteShaderAction_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"

#include "PrimeEngine/../../GlobalConfig/GlobalConfig.h"

// Sibling/Children includes
#include "ShaderAction.h"
#include "SA_Bind_Resource.h"
namespace PE {

namespace Components{
struct Effect;
struct Skin;
}
#if PE_API_IS_D3D11
struct SA_SetAndBind_ConstResource_SingleObjectAnimationPalette : SA_Bind_Resource
{
	SA_SetAndBind_ConstResource_SingleObjectAnimationPalette(PE::GameContext &context, PE::MemoryArena arena)  :
		// will use exisitng functionality to bind and unbind the buffer:
		SA_Bind_Resource(context, arena, GpuResourceSlot_SingleObjectPalette_ConstResource, SamplerState_NotNeeded, s_pShaderResourceViewSinglePalette)
	{}

	virtual ~SA_SetAndBind_ConstResource_SingleObjectAnimationPalette() {}
    
	virtual void bindToPipeline(Components::Effect *pCurEffect = NULL) ;
	virtual void releaseData() ;

#	if APIABSTRACTION_D3D11
	static ID3D11Buffer * s_pBufferSinglePalette;
	static ID3D11ShaderResourceView *s_pShaderResourceViewSinglePalette;

	// unlike DX9 where we just set registers
#	endif

	struct Data{
		Matrix4x4 gJoints[PE_MAX_BONE_COUNT_IN_DRAW_CALL];
	} m_data;

#	if APIABSTRACTION_OGL
#endif
};
#endif

}; // namespace PE
#endif

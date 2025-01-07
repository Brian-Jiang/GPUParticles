#ifndef __PYENGINE_2_0_SA_SetAndBind_ConstResource_InstancedObjectsAnimationPalettes_H__
#define __PYENGINE_2_0_SA_SetAndBind_ConstResource_InstancedObjectsAnimationPalettes_H__

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

namespace PE {

namespace Components{
struct Effect;
struct Skin;
}
#if PE_API_IS_D3D11
struct SA_SetAndBind_ConstResource_InstancedObjectsAnimationPalettes : ShaderAction
{
	virtual ~SA_SetAndBind_ConstResource_InstancedObjectsAnimationPalettes() {}
    SA_SetAndBind_ConstResource_InstancedObjectsAnimationPalettes(){}
    
	virtual void bindToPipeline(Components::Effect *pCurEffect = NULL) ;
	virtual void unbindFromPipeline(Components::Effect *pCurEffect = NULL) ;
	virtual void releaseData() ;

#	if APIABSTRACTION_D3D11
	static ID3D11Buffer * s_pBuffer; // the buffer is first filled with data and then bound to pipeline
	static ID3D11ShaderResourceView *s_pShaderResourceView;

	// unlike DX9 where we just set registers
#	endif

	struct Data{
		Matrix4x4 gInstancedAnimationBones[PE_MAX_BONE_COUNT_IN_DRAW_CALL * PE_MAX_SKINED_INSTANCE_COUNT_IN_DRAW_CALL];
	} m_data;

#	if APIABSTRACTION_OGL
#endif
};

#endif
}; // namespace PE

#endif

#ifndef __PYENGINE_2_0_SA_SetAndBind_ConstResource_PerInstanceData_H__
#define __PYENGINE_2_0_SA_SetAndBind_ConstResource_PerInstanceData_H__

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
struct SA_SetAndBind_ConstResource_PerInstanceData : ShaderAction
{
	virtual ~SA_SetAndBind_ConstResource_PerInstanceData() {}
    SA_SetAndBind_ConstResource_PerInstanceData(){}
    
	virtual void bindToPipeline(Components::Effect *pCurEffect = NULL) ;
	virtual void unbindFromPipeline(Components::Effect *pCurEffect = NULL) ;
	virtual void releaseData() ;

#	if APIABSTRACTION_D3D11
	static ID3D11Buffer * s_pBuffer; // the buffer is first filled with data and then bound to pipeline
	static ID3D11ShaderResourceView *s_pShaderResourceView;
#	endif

	struct PerObjectInstanceData
	{
		//Matrix4x4 WVP;
		//Matrix4x4 W;
		Vector3 W[4];
	};

	struct Data{
		PerObjectInstanceData gInstanceData[PE_MAX_INSTANCE_COUNT_IN_DRAW_CALL];
	} m_data;

	int m_numInstances;
};

#endif
}; // namespace PE

#endif

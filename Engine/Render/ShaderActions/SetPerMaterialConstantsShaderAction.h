#ifndef __PYENGINE_2_0_SetPerMaterialConstantsShaderAction_H__
#define __PYENGINE_2_0_SetPerMaterialConstantsShaderAction_H__

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

struct SetPerMaterialConstantsShaderAction : ShaderAction
{
	virtual ~SetPerMaterialConstantsShaderAction() {}

	virtual void bindToPipeline(Components::Effect *pCurEffect = NULL) ;
	virtual void unbindFromPipeline(Components::Effect *pCurEffect = NULL) ;
	virtual void releaseData() ;
#if PE_PLAT_IS_PSVITA
	static void * s_pBuffer;
#	elif APIABSTRACTION_D3D11
	static ID3D11Buffer * s_pBuffer; // the buffer is first filled with data and then bound to pipeline
	// unlike DX9 where we just set registers
#	endif

	struct Data{
		Vector4 m_xHasNrm_yHasSpec_zHasGlow_wHasDiff;
		Vector4 m_diffuse;
		Vector4 gxyzVSpecular_w;
		Vector4 gxyzVEmissive_wVShininess;
	} m_data;

#	if APIABSTRACTION_OGL
// we need this struct for OGL implementation with cg
// because we need to store a list of cgParameters in each effect (vs setting register values in DX)
	struct PerEffectBindIds
	{
#       if APIABSTRACTION_IOS
            GLuint v_xHasNrm_yHasSpec_zHasGlow_wHasDiff, f_xHasNrm_yHasSpec_zHasGlow_wHasDiff;
            GLuint v_gVDiffuse, f_gVDiffuse;
            GLuint v_gxyzVSpecular_w, f_gxyzVSpecular_w;
            GLuint v_gxyzVEmissive_wVShininess, f_gxyzVEmissive_wVShininess;
#       else
            CGparameter v_xHasNrm_yHasSpec_zHasGlow_wHasDiff, f_xHasNrm_yHasSpec_zHasGlow_wHasDiff;
            CGparameter v_gVDiffuse, f_gVDiffuse;
            CGparameter v_gxyzVSpecular_w, f_gxyzVSpecular_w;
            CGparameter v_gxyzVEmissive_wVShininess, f_gxyzVEmissive_wVShininess;
#       endif
		void initialize(Components::Effect *pEffect);
	};
#	endif
};

}; // namespace PE
#endif

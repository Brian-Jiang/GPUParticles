#ifndef __PYENGINE_2_0_SetInstanceControlConstantsShaderAction_SHADER_ACTION_H__
#define __PYENGINE_2_0_SetInstanceControlConstantsShaderAction_SHADER_ACTION_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/Render/IRenderer.h"
// Sibling/Children includes
#include "ShaderAction.h"

namespace PE {

namespace Components{
struct Effect;
}
#if PE_API_IS_D3D11
struct SetInstanceControlConstantsShaderAction : ShaderAction
{
	virtual ~SetInstanceControlConstantsShaderAction() {}

	virtual void bindToPipeline(Components::Effect *pCurEffect = NULL) ;
	virtual void unbindFromPipeline(Components::Effect *pCurEffect = NULL) ;
	virtual void releaseData() ;

#	if APIABSTRACTION_D3D11
	static ID3D11Buffer * s_pBuffer; // the buffer is first filled with data and then bound to pipeline
	// unlike DX9 where we just set registers
#	endif
	// struct to store Light properties
	
	// the actual data that is transferred to video memory
	struct Data {
		int m_instanceIdOffset;
		int m_pad[3];
	} m_data;

#	if APIABSTRACTION_OGL
	// we need this struct for OGL implementation with cg
	// because we need to store a list of cgParameters in each effect (vs setting register values in DX)
	struct PerEffectBindIds
	{
#       if APIABSTRACTION_IOS
		GLuint v_gInstanceIdOffset, f_gInstanceIdOffset;
#       else
		CGparameter v_gInstanceIdOffset, f_gInstanceIdOffset;
#       endif
        void initialize(Components::Effect *pEffect);
        void initializeLightCGParams(unsigned int index, LightCGParams &out_v, LightCGParams &out_f, Components::Effect *pEffect);
	};
#endif
};
#endif

}; // namespace PE
#endif

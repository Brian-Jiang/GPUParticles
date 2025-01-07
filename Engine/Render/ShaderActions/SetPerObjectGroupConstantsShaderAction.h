#ifndef __PYENGINE_2_0_SetPerObjectGroupConstants_SHADER_ACTION_H__
#define __PYENGINE_2_0_SetPerObjectGroupConstants_SHADER_ACTION_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/Render/IRenderer.h"


#include "PrimeEngine/Render/D3D11Renderer.h"
#include "PrimeEngine/Render/D3D9Renderer.h"
#include "PrimeEngine/Render/GLRenderer.h"
#include "PrimeEngine/Math/Matrix4x4.h"
// Sibling/Children includes
#include "ShaderAction.h"

namespace PE {

namespace Components{
struct Effect;
}

struct SetPerObjectGroupConstantsShaderAction : ShaderAction
{
	SetPerObjectGroupConstantsShaderAction(PE::GameContext &context, PE::MemoryArena arena)
	{
		m_arena = arena; m_pContext = &context;
	}

	virtual ~SetPerObjectGroupConstantsShaderAction() {}

	virtual void bindToPipeline(Components::Effect *pCurEffect = NULL) ;
	virtual void unbindFromPipeline(Components::Effect *pCurEffect = NULL) ;
	virtual void releaseData() ;
#if PE_PLAT_IS_PSVITA
	static void * s_pBuffer;
#	elif APIABSTRACTION_D3D11
	static ID3D11Buffer * s_pBuffer; // the buffer is first filled with data and then bound to pipeline
	// unlike DX9 where we just set registers
#	endif
	PE::MemoryArena m_arena; PE::GameContext *m_pContext;
	
	// struct to store Light properties
	struct hlsl_Light
	{
		Vector3	pos;
		float pad0;

		Vector3	dir;
		float pad1;

		Vector4	ambient;
		Vector4	diffuse;
		Vector4	spec;

		Vector3	att;
		float	spotPower;

		float	range;
		float	type; //0 = point, 1 = directional, 2 = spot
		float pad2;
		float pad3;
	};
    
    static const int NUM_LIGHT_SOURCES = 4; // light sources to use
    // note shaders are hardcoded to allocate registers in shader files. if these valeus are changed, shaders have to be changed too
    static const int NUM_LIGHT_SOURCES_DATAS = 8; // ligth sources to store (shaders still assume we have registers allocated for 8 registers)

	// the actual data that is transferred to video memory
	struct Data {
		Matrix4x4 gViewProj;
		Matrix4x4 gPreviousViewProjMatrix;
		Matrix4x4 gViewProjInverseMatrix;
		Matrix4x4 gViewInv;

		Matrix4x4 gLightWVP;
		Vector3 gEyePosW;
		PrimitiveTypes::Float32 gDoMotionBlur;

		hlsl_Light gLights[NUM_LIGHT_SOURCES_DATAS];
	} m_data;

	NameToData m_nameToData[1];

#	if APIABSTRACTION_OGL
	// we need this struct for OGL implementation with cg
	// because we need to store a list of cgParameters in each effect (vs setting register values in DX)
	struct PerEffectBindIds
	{
#       if APIABSTRACTION_IOS
		GLuint v_gViewProj, f_gViewProj;
		GLuint v_gPreviousViewProjMatrix, f_gPreviousViewProjMatrix;
		GLuint v_gViewProjInverseMatrix, f_gViewProjInverseMatrix;
		GLuint v_gViewInv, f_gViewInv;
		
		GLuint v_gLightWVP, f_gLightWVP;
		GLuint v_gEyePosW, f_gEyePosW;
		GLuint v_xyzgEyePosW_wDoMotionBlur, f_xyzgEyePosW_wDoMotionBlur;
        
        struct LightCGParams{
			GLuint pos;
			GLuint dir;
            
			GLuint ambient;
			GLuint diffuse;
			GLuint specular;
            
			GLuint atten_spot;
			GLuint range_type;
		};

#       else
		
		CGparameter v_gViewProj, f_gViewProj;
		CGparameter v_gPreviousViewProjMatrix, f_gPreviousViewProjMatrix;
		CGparameter v_gViewProjInverseMatrix, f_gViewProjInverseMatrix;
		CGparameter v_gViewInv, f_gViewInv;

		CGparameter v_gLightWVP, f_gLightWVP;
		CGparameter v_gEyePosW, f_gEyePosW;
		CGparameter v_xyzgEyePosW_wDoMotionBlur, f_xyzgEyePosW_wDoMotionBlur;

		struct LightCGParams{
			CGparameter pos;
			CGparameter dir;

			CGparameter ambient;
			CGparameter diffuse;
			CGparameter specular;

			CGparameter atten_spot;
			CGparameter range_type;
		};
#       endif
        LightCGParams v_gLight[NUM_LIGHT_SOURCES_DATAS], f_gLight[NUM_LIGHT_SOURCES_DATAS];
		void initialize(Components::Effect *pEffect);
        void initializeLightCGParams(unsigned int index, LightCGParams &out_v, LightCGParams &out_f, Components::Effect *pEffect);
	};
#endif
};


}; // namespace PE
#endif

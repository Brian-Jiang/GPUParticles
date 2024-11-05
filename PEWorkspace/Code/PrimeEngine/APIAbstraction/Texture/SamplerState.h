#ifndef __PYENGINE_2_0_SamplerState__
#define __PYENGINE_2_0_SamplerState__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/Render/D3D11Renderer.h"
#include "PrimeEngine/Render/D3D9Renderer.h"
#include "PrimeEngine/Render/GLRenderer.h"

// Sibling/Children includes


namespace PE {
#if APIABSTRACTION_D3D9
	struct SamplerState
	{
		DWORD 
			val_D3DSAMP_ADDRESSU,
			val_D3DSAMP_ADDRESSV,
			val_D3DSAMP_ADDRESSW,
			val_D3DSAMP_BORDERCOLOR,
			val_D3DSAMP_MAGFILTER,
			val_D3DSAMP_MINFILTER,
			val_D3DSAMP_MIPFILTER,
			val_D3DSAMP_MIPMAPLODBIAS,
			val_D3DSAMP_MAXMIPLEVEL,
			val_D3DSAMP_MAXANISOTROPY,
			val_D3DSAMP_SRGBTEXTURE,
			val_D3DSAMP_ELEMENTINDEX,
			val_D3DSAMP_DMAPOFFSET;

		bool needsMipMaps() { return val_D3DSAMP_MIPFILTER != D3DTEXF_NONE; }
		
	};
#elif APIABSTRACTION_D3D11
	struct SamplerState
	{
		ID3D11SamplerState* m_pd3dSamplerState;
		bool needsMipMaps();
	};
#elif APIABSTRACTION_OGL
	struct SamplerState
	{
		GLenum val_GL_TEXTURE_WRAP_S;
		GLenum val_GL_TEXTURE_WRAP_T;
		// GLenum val_D3DSAMP_ADDRESSW; // havent seen this in OGL
		GLenum val_TEXTURE_BORDER_COLOR;
		GLenum val_TEXTURE_BORDER;

		GLenum val_GL_TEXTURE_MAG_FILTER;
		GLenum val_GL_TEXTURE_MIN_FILTER;
		// GLenum val_D3DSAMP_MIPFILTER; // in gl is specified as aprt of parameter val_GL_TEXTURE_MIN_FILTER
		
		//todo: implement in ogl if exist
		//GLenum val_D3DSAMP_MIPMAPLODBIAS;
		//GLenum val_D3DSAMP_MAXMIPLEVEL;
		//GLenum val_D3DSAMP_MAXANISOTROPY;
		//GLenum val_D3DSAMP_SRGBTEXTURE;
		//GLenum val_D3DSAMP_ELEMENTINDEX;
		//GLenum val_D3DSAMP_DMAPOFFSET;

		bool needsMipMaps() { return val_GL_TEXTURE_MIN_FILTER != GL_LINEAR && val_GL_TEXTURE_MIN_FILTER != GL_NEAREST; }
	};
#elif PE_PLAT_IS_PS4
	
#elif PE_PLAT_IS_PSVITA
	
#endif

	enum ESamplerState
	{
		SamplerState_INVALID = -1,
		SamplerState_NotNeeded, // used fo resources that dont use sampler
		
		SamplerState_NoMips_MinTexelLerp_NoMagTexelLerp_Clamp,
		SamplerState_NoMips_NoMinTexelLerp_NoMagTexelLerp_Clamp,
		SamplerState_NoMips_NoMinTexelLerp_NoMagTexelLerp_Wrap,
		SamplerState_NoMips_NoMinTexelLerp_MagTexelLerp_Clamp,

		SamplerState_MipLerp_MinTexelLerp_MagTexelLerp_Wrap,
		SamplerState_MipLerp_MinTexelLerp_MagTexelLerp_Clamp,


		SamplerStateCustom0,
		// space to add more that are nto potentially static or known at the beginning
		SamplerStateCustom100 = SamplerStateCustom0 + 100,
		SamplerState_Count
	};



	struct SamplerStateManager : public PEAllocatableAndDefragmentable
	{
		static SamplerStateManager *getInstance(){return s_instance;}
		static SamplerStateManager * ConstructAndInitialize(PE::GameContext &context, PE::MemoryArena arena);

		void initializeSamplerStates(PE::GameContext &context);

		SamplerState &getSamplerState(ESamplerState state);

		static SamplerStateManager *s_instance;

		SamplerState m_samplerStates[SamplerState_Count]; // stores ma of sampler states
	};
}; // namespace PE

#endif

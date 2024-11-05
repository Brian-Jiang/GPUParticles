
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"



#include "SamplerState.h"

// Inter-Engine includes
//

// Outer-Engine includes
// Sibling includes

namespace PE {
	SamplerStateManager *SamplerStateManager::s_instance = NULL;

	SamplerStateManager * SamplerStateManager::ConstructAndInitialize(PE::GameContext &context, PE::MemoryArena arena)
	{
		s_instance = new(arena) SamplerStateManager();
	
		s_instance->initializeSamplerStates(context);
		return s_instance;
	}

	SamplerState &SamplerStateManager::getSamplerState(ESamplerState state)
	{
		assert(state < SamplerState_Count && state >= 0);

		return m_samplerStates[state];
	}

	void SamplerStateManager::initializeSamplerStates(PE::GameContext &context)
	{
#	if APIABSTRACTION_D3D9
		SamplerState ss;
		memset(&ss, 0, sizeof(ss));

		ss.val_D3DSAMP_ADDRESSU = D3DTADDRESS_WRAP;
		ss.val_D3DSAMP_ADDRESSV = D3DTADDRESS_WRAP;
		ss.val_D3DSAMP_ADDRESSW = D3DTADDRESS_WRAP;
		//D3DTADDRESS_WRAP          = 1,
		//D3DTADDRESS_MIRROR        = 2,
		//D3DTADDRESS_CLAMP         = 3,
		//D3DTADDRESS_BORDER        = 4,
		//D3DTADDRESS_MIRRORONCE    = 5,

		ss.val_D3DSAMP_BORDERCOLOR = 0x00000000;

		ss.val_D3DSAMP_MAGFILTER = D3DTEXF_LINEAR;
		ss.val_D3DSAMP_MINFILTER = D3DTEXF_LINEAR;
		ss.val_D3DSAMP_MIPFILTER = D3DTEXF_LINEAR;
		//D3DTEXF_NONE              = 0,
		//D3DTEXF_POINT             = 1,
		//D3DTEXF_LINEAR            = 2,
		//D3DTEXF_ANISOTROPIC       = 3,
		//D3DTEXF_PYRAMIDALQUAD     = 6,
		//D3DTEXF_GAUSSIANQUAD      = 7,
		//D3DTEXF_CONVOLUTIONMONO   = 8,

		ss.val_D3DSAMP_MIPMAPLODBIAS = 0;
		ss.val_D3DSAMP_MAXMIPLEVEL = 0;
		ss.val_D3DSAMP_MAXANISOTROPY = 1;
		ss.val_D3DSAMP_SRGBTEXTURE = 0;
		ss.val_D3DSAMP_ELEMENTINDEX = 0;
		ss.val_D3DSAMP_DMAPOFFSET = 0;

		m_samplerStates[SamplerState_MipLerp_MinTexelLerp_MagTexelLerp_Wrap] = ss;

		ss.val_D3DSAMP_ADDRESSU = D3DTADDRESS_CLAMP;
		ss.val_D3DSAMP_ADDRESSV = D3DTADDRESS_CLAMP;
		ss.val_D3DSAMP_ADDRESSW = D3DTADDRESS_CLAMP;

		m_samplerStates[SamplerState_MipLerp_MinTexelLerp_MagTexelLerp_Clamp] = ss;

		ss.val_D3DSAMP_MAGFILTER = D3DTEXF_POINT;
		ss.val_D3DSAMP_MINFILTER = D3DTEXF_LINEAR;
		ss.val_D3DSAMP_MIPFILTER = D3DTEXF_NONE;

		m_samplerStates[SamplerState_NoMips_MinTexelLerp_NoMagTexelLerp_Clamp] = ss;

		ss.val_D3DSAMP_MINFILTER = D3DTEXF_POINT;
		m_samplerStates[SamplerState_NoMips_NoMinTexelLerp_NoMagTexelLerp_Clamp] = ss;

		ss.val_D3DSAMP_ADDRESSU = D3DTADDRESS_WRAP;
		ss.val_D3DSAMP_ADDRESSV = D3DTADDRESS_WRAP;
		ss.val_D3DSAMP_ADDRESSW = D3DTADDRESS_WRAP;

		m_samplerStates[SamplerState_NoMips_NoMinTexelLerp_NoMagTexelLerp_Wrap] = ss;

		ss.val_D3DSAMP_ADDRESSU = D3DTADDRESS_CLAMP;
		ss.val_D3DSAMP_ADDRESSV = D3DTADDRESS_CLAMP;
		ss.val_D3DSAMP_ADDRESSW = D3DTADDRESS_CLAMP;

		ss.val_D3DSAMP_MAGFILTER = D3DTEXF_LINEAR;
		m_samplerStates[SamplerState_NoMips_NoMinTexelLerp_MagTexelLerp_Clamp] = ss;
#endif
#if APIABSTRACTION_D3D11

		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(context.getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

		SamplerState ss;
		memset(&ss, 0, sizeof(ss));

		D3D11_SAMPLER_DESC sdesc;
		memset(&sdesc, 0, sizeof(sdesc));

		sdesc.Filter = D3D11_FILTER_ANISOTROPIC;//D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		sdesc.MinLOD = 0;
		sdesc.MaxLOD = D3D11_FLOAT32_MAX;
		sdesc.MipLODBias = 0;
		sdesc.MaxAnisotropy = 16;
		//sdesc.ComparisonFunc = NULL;
		memset(sdesc.BorderColor,0, sizeof(sdesc.BorderColor));
		HRESULT hr = pDevice->CreateSamplerState( &sdesc, &ss.m_pd3dSamplerState);
		assert(SUCCEEDED(hr));
				
		m_samplerStates[SamplerState_MipLerp_MinTexelLerp_MagTexelLerp_Wrap] = ss;

		sdesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sdesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sdesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		hr = pDevice->CreateSamplerState( &sdesc, &ss.m_pd3dSamplerState);
		assert(SUCCEEDED(hr));

		m_samplerStates[SamplerState_MipLerp_MinTexelLerp_MagTexelLerp_Clamp] = ss;

		sdesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		sdesc.MaxLOD = 0;
		hr = pDevice->CreateSamplerState( &sdesc, &ss.m_pd3dSamplerState);
		assert(SUCCEEDED(hr));

		m_samplerStates[SamplerState_NoMips_MinTexelLerp_NoMagTexelLerp_Clamp] = ss;

		sdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		hr = pDevice->CreateSamplerState( &sdesc, &ss.m_pd3dSamplerState);
		assert(SUCCEEDED(hr));

		m_samplerStates[SamplerState_NoMips_NoMinTexelLerp_NoMagTexelLerp_Clamp] = ss;

		sdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		hr = pDevice->CreateSamplerState( &sdesc, &ss.m_pd3dSamplerState);
		assert(SUCCEEDED(hr));

		m_samplerStates[SamplerState_NoMips_NoMinTexelLerp_NoMagTexelLerp_Wrap] = ss;

		sdesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sdesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sdesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sdesc.Filter = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		hr = pDevice->CreateSamplerState( &sdesc, &ss.m_pd3dSamplerState);
		assert(SUCCEEDED(hr));

		m_samplerStates[SamplerState_NoMips_NoMinTexelLerp_MagTexelLerp_Clamp] = ss;
#endif	
#if APIABSTRACTION_OGL

		SamplerState ss;
		memset(&ss, 0, sizeof(ss));

		ss.val_GL_TEXTURE_WRAP_S = GL_REPEAT;
		ss.val_GL_TEXTURE_WRAP_T = GL_REPEAT;
		//ss.val_D3DSAMP_ADDRESSW = D3DTADDRESS_WRAP;
		
		ss.val_TEXTURE_BORDER = false;
		ss.val_TEXTURE_BORDER_COLOR = 0x00000000;

		ss.val_GL_TEXTURE_MAG_FILTER = GL_LINEAR;
		ss.val_GL_TEXTURE_MIN_FILTER = GL_LINEAR_MIPMAP_LINEAR;
		
// 		ss.val_D3DSAMP_MIPMAPLODBIAS = 0;
// 		ss.val_D3DSAMP_MAXMIPLEVEL = 0;
// 		ss.val_D3DSAMP_MAXANISOTROPY = 1;
// 		ss.val_D3DSAMP_SRGBTEXTURE = 0;
// 		ss.val_D3DSAMP_ELEMENTINDEX = 0;
// 		ss.val_D3DSAMP_DMAPOFFSET = 0;

		m_samplerStates[SamplerState_MipLerp_MinTexelLerp_MagTexelLerp_Wrap] = ss;

#if APIABSTRACTION_IOS
        ss.val_GL_TEXTURE_WRAP_S = GL_CLAMP_TO_EDGE;
		ss.val_GL_TEXTURE_WRAP_T = GL_CLAMP_TO_EDGE;

#else
		ss.val_GL_TEXTURE_WRAP_S = GL_CLAMP;
		ss.val_GL_TEXTURE_WRAP_T = GL_CLAMP;
#endif
		m_samplerStates[SamplerState_MipLerp_MinTexelLerp_MagTexelLerp_Clamp] = ss;

		ss.val_GL_TEXTURE_MAG_FILTER = GL_NEAREST;
		ss.val_GL_TEXTURE_MIN_FILTER = GL_LINEAR;

		m_samplerStates[SamplerState_NoMips_MinTexelLerp_NoMagTexelLerp_Clamp] = ss;
		
		ss.val_GL_TEXTURE_MIN_FILTER = GL_NEAREST; // GL_NEAREST_MIPMAP_NEAREST would enable mipmapping
		
		m_samplerStates[SamplerState_NoMips_NoMinTexelLerp_NoMagTexelLerp_Clamp] = ss;

		ss.val_GL_TEXTURE_WRAP_S = GL_REPEAT;
		ss.val_GL_TEXTURE_WRAP_T = GL_REPEAT;
		//ss.val_D3DSAMP_ADDRESSW = D3DTADDRESS_WRAP;
		m_samplerStates[SamplerState_NoMips_NoMinTexelLerp_NoMagTexelLerp_Wrap] = ss;
		
#if APIABSTRACTION_IOS
        ss.val_GL_TEXTURE_WRAP_S = GL_CLAMP_TO_EDGE;
		ss.val_GL_TEXTURE_WRAP_T = GL_CLAMP_TO_EDGE;

#else
		ss.val_GL_TEXTURE_WRAP_S = GL_CLAMP;
		ss.val_GL_TEXTURE_WRAP_T = GL_CLAMP;
#endif
		ss.val_GL_TEXTURE_MAG_FILTER = GL_LINEAR;
		m_samplerStates[SamplerState_NoMips_NoMinTexelLerp_MagTexelLerp_Clamp] = ss;

#endif
#if PE_PLAT_IS_PSVITA

#	endif
#if PE_PLAT_IS_PS4

#	endif
	}
#if APIABSTRACTION_D3D11
bool SamplerState::needsMipMaps()
{
	D3D11_SAMPLER_DESC sdescs;
	m_pd3dSamplerState->GetDesc(&sdescs);

	return (sdescs.Filter % 2) > 0;
}
#endif

}; // namespace PE

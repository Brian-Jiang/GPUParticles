#ifndef HLSL_SAPLERHELPER
#define HLSL_SAPLERHELPER

#if APIABSTRACTION_D3D11

#define sample2D(sampler, texCoord, textureDX11) \
	textureDX11.Sample(sampler, texCoord)

#elif APIABSTRACTION_D3D9

// note for DX 9 texture is ignored, because a texture is bound to sampler itself
#define sample2D(sampler, texCoord, textureDX11) \
	tex2D(sampler, texCoord)
	
#elif APIABSTRACTION_IOS

    #define sample2D(sampler, texCoord, textureDX11) \
        texture2D(sampler, texCoord)

#else // OGL CG

#define sample2D(sampler, texCoord, textureDX11) \
	tex2D(sampler, texCoord)

#endif

#if APIABSTRACTION_D3D11

#define sample(sampler, texCoord, textureDX11) \
	textureDX11.Sample(sampler, texCoord)

#elif APIABSTRACTION_D3D9

// note for DX 9 texture is ignored
#define sample(sampler, texCoord, textureDX11) \
	tex2D(sampler, texCoord)
#endif


#if APIABSTRACTION_D3D11

#define sampleCUBE(sampler, texCoord, textureDX11) \
	textureDX11.Sample(sampler, texCoord)

#elif APIABSTRACTION_D3D9

// note for DX 9 texture is ignored
#define sampleCUBE(sampler, texCoord, textureDX11) \
	texCUBE(sampler, texCoord)
#endif


#if APIABSTRACTION_D3D11

#define sampleTEXARRAY(sampler, texCoord, textureDX11) \
	textureDX11.Sample(sampler, texCoord)

#elif APIABSTRACTION_D3D9

// ot implemented
#define sampleTEXARRAY(sampler, texCoord, textureDX11) \
	tex2D(sampler, texCoord)
#endif


#if APIABSTRACTION_D3D11

#define sampleLevel(sampler, texCoord, level, textureDX11) \
	textureDX11.SampleLevel(sampler, texCoord, level)

#elif APIABSTRACTION_D3D9

// not implemented
#define sampleLevel(sampler, texCoord, textureDX11) \
	tex2D(sampler, texCoord)
#endif


#endif

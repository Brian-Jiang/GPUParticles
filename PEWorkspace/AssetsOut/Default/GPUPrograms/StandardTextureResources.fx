#ifndef HLSL_STANDARDTEXTURERESOURCES
#define HLSL_STANDARDTEXTURERESOURCES

#if APIABSTRACTION_D3D11

////////////////////////////
// Per object group
////////////////////////////

Texture2D gDepthMap : register(t0); // for post processing this is the zbuffer after all objects have been drawn
SamplerState gDepthMapSampler : register(s0);
Texture2D gShadowMap : register(t1); // shadow (depth) map from light sources POV
SamplerState gShadowMapSampler : register(s1);


////////////////////////////
// Per object
////////////////////////////

Texture2D gDiffuseMap : register(t2);
SamplerState gDiffuseMapSampler : register(s2);

Texture2DArray gDiffuseMapArray : register(t2);
SamplerState gDiffuseMapArraySampler : register(s2);

TextureCube gCubeMap : register(t2);
SamplerState gCubeMapSampler : register(s2);

Texture2D gGlowMap : register(t3);
SamplerState gGlowMapSampler : register(s3);

Texture2D gBumpMap : register(t4);
SamplerState gBumpMapSampler : register(s4);

Texture2D gSpecularMap : register(t5);
SamplerState gSpecularMapSampler : register(s5);

// special maps
Texture2D gAdditionalDiffuseMap : register(t6);
SamplerState gAdditionalDiffuseMapSampler : register(s6);

Texture2D gHorizontallyBlurredGlowMap : register(t6);
SamplerState gBlurredGlowMapSampler : register(s6);

Texture2D gWindMap : register(t7);
SamplerState gWindMapSampler : register(s7);

Texture1D gRandomMap : register(t7);
SamplerState gRandomMapSampler : register(s7);


// DX compute
struct BufferStruct
{
	float3 pos;
	float2 texCoord;
};

struct TexelStruct
{
	float4 color;
};

#elif APIABSTRACTION_D3D9
////////////////////////////
// Per object group
////////////////////////////

sampler gDepthMapSampler : register(s0);
sampler gShadowMapSampler : register(s1);


////////////////////////////
// Per object
////////////////////////////

sampler gDiffuseMapSampler : register(s2);

sampler gDiffuseMapArraySampler : register(s2);

sampler gCubeMapSampler : register(s2);

sampler gGlowMapSampler : register(s3);

sampler gBumpMapSampler : register(s4);

sampler gSpecularMapSampler : register(s5);

// special maps
sampler gAdditionalDiffuseMapSampler : register(s6);

sampler gBlurredGlowMapSampler : register(s6);

sampler gWindMapSampler : register(s7);

sampler gRandomMapSampler : register(s7);

#else // OGL, CG, PSVITA

API_UNIFORM_CONSTANT sampler2D gShadowMapSampler SAMPLER_REGISTER(1); // : register(s1);

API_UNIFORM_CONSTANT sampler2D gDiffuseMapSampler SAMPLER_REGISTER(2);// : register(s2);

API_UNIFORM_CONSTANT sampler2D gGlowMapSampler SAMPLER_REGISTER(3);
API_UNIFORM_CONSTANT sampler2D gBumpMapSampler SAMPLER_REGISTER(4); // : register(s4);

API_UNIFORM_CONSTANT sampler2D gSpecularMapSampler SAMPLER_REGISTER(5); // : register(s5);

API_UNIFORM_CONSTANT sampler2D gBlurredGlowMapSampler SAMPLER_REGISTER(6); // : register(s6);

#endif



#endif // file guard

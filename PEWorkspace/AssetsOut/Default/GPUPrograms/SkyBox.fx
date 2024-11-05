#include "StdMesh.fx"
#include "ZBufferLookup_DSS.fx"
//------------------------------------------------------------------------------
// structures
//------------------------------------------------------------------------------

struct SKY_MESH_PS_IN
{
	float4 iPosH : SV_POSITION;
	float3 iTexCoord : TEXCOORD;
};

SKY_MESH_PS_IN Sky_VS(STD_MESH_VS_IN vIn)
{
	// make sure we orient the shape with camera
	SKY_MESH_PS_IN vOut;
	vOut.iPosH = mul(mul(make_float4(vIn.iPosL, 1.0f), gView), gViewProj).xyww;
	vOut.iTexCoord = mul(make_float4(vIn.iPosL, 1.0f), gView).xyz - xyzgEyePosW_wDoMotionBlur.xyz;
	return vOut;
}

float4 Sky_PS(SKY_MESH_PS_IN pIn) : SV_Target
{
	float4 diffuse = sampleCUBE(gCubeMapSampler, pIn.iTexCoord, gCubeMap) * 0.8;
	diffuse.a = 0.16f;
	return diffuse;
}

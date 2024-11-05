#ifndef HLSL_REDUCEDMESH_TITLE_PS
#define HLSL_REDUCEDMESH_TITLE_PS

#include "StdMesh_Structs.fx"
#include "StandardConstants.fx"
#include "StandardTextureResources.fx"
#include "samplerhelper.fx"

float4 ReducedMesh_Title_PS(REDUCED_MESH_PS_IN pIn) : SV_Target
{

	float4 color = sample(gDiffuseMapSampler, pIn.iTexCoord, gDiffuseMap);
	//color.r *= color.a;
	//color.g *= color.a;
	//color.b *= color.a;
	//color.a *= 0.2f;
	//clip(color.a - 0.5);
	return color;
}
#endif

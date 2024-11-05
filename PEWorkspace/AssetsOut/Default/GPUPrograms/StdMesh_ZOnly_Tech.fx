#include "StdMesh.fx"
#include "StandardTextureResources.fx"
#include "StandardConstants.fx"

#include "SolidBackCullNoBias_RS.fx"
#include "ZBuffer_DSS.fx"
#include "Replace_BS.fx"

STD_MESH_ZONLY_PS_IN StdMesh_ZOnly_VS(STD_MESH_VS_IN vIn)
{
	STD_MESH_ZONLY_PS_IN vOut;
	vOut.iPosH = mul(make_float4(vIn.iPosL, 1.0f), gWVP);
	return vOut;
}

#ifdef APIABSTRACTION_D3D11
#if APIABSTRACTION_D3D11
float4 StdMesh_ZOnly_PS(STD_MESH_ZONLY_PS_IN pIn)
{return make_float4(1.0f, 0, 0, 0);}
#endif
#else
void StdMesh_ZOnly_PS(STD_MESH_ZONLY_PS_IN pIn)
{}
#endif


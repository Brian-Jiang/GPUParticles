#ifdef HLSL_SEPARATE_LOAD
#undef HLSL_SEPARATE_LOAD

#include "StdMesh.fx"
#include "StdMeshBlendShapes_Structs.fx"

#endif

STD_MESH_ZONLY_PS_IN StdMeshBlendShapes_ZOnly_VS(
	STD_MESH_VS_IN0 vIn0,
	STD_MESH_VS_IN1 vIn1,
	STD_MESH_VS_IN2 vIn2,
	STD_MESH_VS_IN3 vIn3)
{
	vIn0.iPosL = vIn0.iPosL + 
		(vIn1.iPosL - vIn0.iPosL) * gVertexBufferWeights.y +
		(vIn2.iPosL - vIn0.iPosL) * gVertexBufferWeights.z +
		(vIn3.iPosL - vIn0.iPosL) * gVertexBufferWeights.w;
#if 0
	vIn0.iPosL = vIn0.iPosL + 
	(
		(vIn1.iPosL - vIn0.iPosL) + 
		(vIn2.iPosL - vIn0.iPosL) + 
		(vIn3.iPosL - vIn0.iPosL)
	) * (sin(gGameTimes.x) + 1.0f) / 2.0f;
#endif
	return StdMesh_ZOnly_VS(vIn0);
}

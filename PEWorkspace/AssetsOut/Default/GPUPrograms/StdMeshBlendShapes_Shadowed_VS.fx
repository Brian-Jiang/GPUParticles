#ifdef HLSL_SEPARATE_LOAD
#undef HLSL_SEPARATE_LOAD

#include "StdMesh.Impl.fx"
#include "StdMeshBlendShapes_Structs.fx"

#endif

STD_MESH_SHADOWED_PS_IN StdMeshBlendShapes_Shadowed_VS(
	STD_MESH_VS_IN0 vIn0,
	STD_MESH_VS_IN1 vIn1,
	STD_MESH_VS_IN2 vIn2,
	STD_MESH_VS_IN3 vIn3)
{
	
	vIn0.iPosL = vIn0.iPosL + 
		(vIn1.iPosL - vIn0.iPosL) * gVertexBufferWeights.y +
		(vIn2.iPosL - vIn0.iPosL) * abs(sin(gGameTimes.x)) + //gVertexBufferWeights.z + 
		(vIn3.iPosL - vIn0.iPosL) * gVertexBufferWeights.w;
	return StdMesh_Shadowed_VS(vIn0);
}


































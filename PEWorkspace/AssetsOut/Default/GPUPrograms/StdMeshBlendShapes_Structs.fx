//------------------------------------------------------------------------------
// structures
//------------------------------------------------------------------------------

struct STD_MESH_VS_IN0
{
	float3 iPosL    : POSITION0;
	float2 iTexCoord    : TEXCOORD0;
	float3 iNormal		: NORMAL0;
};

struct STD_MESH_VS_IN1
{
	float3 iPosL    : POSITION1;
	float2 iTexCoord    : TEXCOORD1;
	float3 iNormal		: NORMAL1;
};

struct STD_MESH_VS_IN2
{
	float3 iPosL    : POSITION2;
	float2 iTexCoord    : TEXCOORD2;
	float3 iNormal		: NORMAL2;
};

struct STD_MESH_VS_IN3
{
	float3 iPosL    : POSITION3;
	float2 iTexCoord    : TEXCOORD3;
	float3 iNormal		: NORMAL3;
};

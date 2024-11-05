#ifndef HLSL_SOLIDBACKCULLZBIAS_RS
#define HLSL_SOLIDBACKCULLZBIAS_RS
/* after z-only pass*/
/*
RasterizerState SolidBackCullZBias_RS
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
	DepthBias = -1;
	DepthBiasClamp = 0.0000001f;
	SlopeScaledDepthBias = -0.7f;
};
*/

// for when there is no z-only pass
RasterizerState SolidBackCullZBias_RS
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

#endif

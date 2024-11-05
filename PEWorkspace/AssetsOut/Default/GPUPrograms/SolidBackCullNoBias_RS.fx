#ifndef HLSL_SOLIDBACKCULLNOBIAS_RS
#define HLSL_SOLIDBACKCULLNOBIAS_RS
RasterizerState SolidBackCullNoBias_RS
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};
#endif

#ifndef HLSL_RASTERIZERHELPER
#define HLSL_RASTERIZERHELPER

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

RasterizerState SolidNoCullZBiasRasterizerState
{
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockwise = false;
	DepthBias = -1;
	DepthBiasClamp = 0.0000001f;
	SlopeScaledDepthBias = -0.7f;
};
*/


RasterizerState SolidNoCullZBiasRasterizerState
{
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockwise = false;

};

RasterizerState SolidBackCullNoBiasRasterizerState
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

// for when there is no z-only pass
RasterizerState SolidBackCullZ//Bias//RasterizerState
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState SolidNoCullRasterizerState
{
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockwise = false;
};

#endif

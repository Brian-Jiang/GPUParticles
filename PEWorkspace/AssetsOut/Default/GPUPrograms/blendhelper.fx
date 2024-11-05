#ifndef HLSL_BLENDHELPER
#define HLSL_BLENDHELPER
BlendState ReplaceBlendState
{
	BlendEnable[0] = FALSE;
};

BlendState AlphaBlendWithGlowResultBlendState
{
	BlendEnable[0] = TRUE;
	SrcBlend = ONE;
	DestBlend = ZERO;
	BlendOp = ADD;
	
	SrcBlendAlpha = ZERO;
	DestBlendAlpha = ONE;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0f;
};

BlendState AlphaBlendWithNoGlowResultBlendState
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
	BlendOp = ADD;
	
	SrcBlendAlpha = ZERO;
	DestBlendAlpha = ONE;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0f;
};

BlendState AdditiveBlendWithGlowResultBlendState
{
	BlendEnable[0] = TRUE;
	SrcBlend = ONE;
	DestBlend = ONE;
	BlendOp = ADD;
	
	SrcBlendAlpha = ONE;
	DestBlendAlpha = ZERO;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0f;
};

BlendState AdditiveBlendWithAdditiveGlowResultBlendState
{
	BlendEnable[0] = TRUE;
	
	SrcBlend = ONE;
	DestBlend = ONE;
	BlendOp = ADD;
	
	SrcBlendAlpha = ONE;
	DestBlendAlpha = ONE;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0f;
	
};

BlendState BlendWithBlendFactorAndGlowResultBlendState
{
	BlendEnable[0] = TRUE;
	SrcBlend = Blend_Factor;
	DestBlend = Inv_Blend_Factor;
	
	BlendOp = ADD;
	
	SrcBlendAlpha = ONE;
	DestBlendAlpha = ZERO;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0f;
};

BlendState BlendWithBlendFactor1AndGlowResult_Explosion_BlendState
{
	BlendEnable[0] = TRUE;
	SrcBlend = Blend_Factor;
	DestBlend = Blend_Factor;
	
	BlendOp = ADD;
	
	SrcBlendAlpha = ONE;
	DestBlendAlpha = ONE;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0f;
};

BlendState SubtractiveBlendWithGlowResultBlendState
{
	BlendEnable[0] = TRUE;
	SrcBlend = ONE;
	DestBlend = ONE;
	// TODO: BlendOp = 2; // WAS: Rev_Subtract;
	
	SrcBlendAlpha = ONE;
	DestBlendAlpha = ONE;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0f;
};

BlendState MultiplicativeBlendWithGlowResultBlendState
{
	BlendEnable[0] = TRUE;
	SrcBlend = ZERO;
	// TODO: DestBlend = 4; // WAS: Inv_Src_Color;
	BlendOp = ADD;
	
	SrcBlendAlpha = ONE;
	DestBlendAlpha = ONE;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0f;
};

static const float BlurWeights[13] =
{
0.14,
0.17,
0.20,
0.23,
0.25,
0.27,
0.29,
0.27,
0.25,
0.23,
0.20,
0.17,
0.14,
};

#endif

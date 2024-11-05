#ifndef HLSL_DEPTHSTENCILHELPER
#define HLSL_DEPTHSTENCILHELPER
DepthStencilState DepthEnabling
{
	DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};
#if 1
DepthStencilState RenderWithoutDepthEnabled
{
    //DepthEnable = false;
    //DepthWriteMask = Zero;
    //DepthFunc = Less;
};
DepthStencilState RenderWithUsualZBufferState
{
    DepthEnable = true;
    DepthWriteMask = All;
    DepthFunc = Less;
};



DepthStencilState RenderZLookupNoWriteState
{
    DepthEnable = true;
    DepthWriteMask = Zero;
    DepthFunc = Less_Equal;
};
#endif

#endif

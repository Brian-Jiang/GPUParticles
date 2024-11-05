#ifndef HLSL_ZBUFFER_DSS
#define HLSL_ZBUFFER_DSS

DepthStencilState ZBuffer_DSS
{
    DepthEnable = true;
    DepthWriteMask = All;
    DepthFunc = Less;
};
#endif

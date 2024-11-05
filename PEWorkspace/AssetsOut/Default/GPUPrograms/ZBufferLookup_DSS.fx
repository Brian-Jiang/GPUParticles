#ifndef HLSL_ZBUFFERLOOKUP_DSS
#define HLSL_ZBUFFERLOOKUP_DSS

// For reading values from z-only render
/*
DepthStencilState ZBufferLookup_DSS
{
    DepthEnable = true;
    DepthWriteMask = Zero;
    DepthFunc = Less_Equal;
};
*/

DepthStencilState ZBufferLookup_DSS
{
    DepthEnable = true;
    DepthWriteMask = All;
    DepthFunc = Less_Equal;
};
#endif

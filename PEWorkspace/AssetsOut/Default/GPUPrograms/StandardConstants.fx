#ifndef HLSL_STANDARDCONSTANTS
#define HLSL_STANDARDCONSTANTS

#include "APIAbstraction.gpu"

struct Light // 7 registers total
{
	float4	xyzPos_w; // xyz = pos, w = padding
	float4	xyzDir_w; // xyz = dir, w = padding
	float4	ambient;
	float4	diffuse;
	float4	spec;
	float4	xyzAtt_wSpotPwr; // xyz = attenuation, w = spot power
	float4	xRange_yType_zw;//type: 0 = point, 1 = directional, 2 = spot
};

///////////////////////////////////////////////////////////////////////////////
// Per-frame
///////////////////////////////////////////////////////////////////////////////
#define API_BUFFER_INDEX 0
// cbuffer cbPerFrame : register(b0)
BEGIN_BUFFER // total registers: 1 absoulte: [0-0]
	// each register hold 16 bytes
	// each element is 4 bytes
	// x = game time, y = frame time
	API_UNIFORM_CONSTANT float2 gGameTimes API_UNIFORM_CONSTANT_REGISTER_REG(0.x, 0);  // packoffset(c0.x)
	API_UNIFORM_CONSTANT uint gGameFrame API_UNIFORM_CONSTANT_REGISTER_REG(0.z, 0);  // packoffset(c0.z)
END_BUFFER

////////////////////////////////////////////////////////////////////////////////
// Per object group
////////////////////////////////////////////////////////////////////////////////
#undef API_BUFFER_INDEX
#define API_BUFFER_INDEX 1
BEGIN_BUFFER // total registers: 77 absoulte: [1-77]
	API_UNIFORM_CONSTANT matrix gViewProj                 API_UNIFORM_CONSTANT_REGISTER_REG(0, 1);
	API_UNIFORM_CONSTANT matrix gPreviousViewProjMatrix   API_UNIFORM_CONSTANT_REGISTER_REG(4, 5);
	API_UNIFORM_CONSTANT matrix gViewProjInverseMatrix    API_UNIFORM_CONSTANT_REGISTER_REG(8, 9);
	API_UNIFORM_CONSTANT matrix gView                     API_UNIFORM_CONSTANT_REGISTER_REG(12, 13);
	
	API_UNIFORM_CONSTANT matrix gLightWVP                 API_UNIFORM_CONSTANT_REGISTER_REG(16, 17);
	API_UNIFORM_CONSTANT float4 xyzgEyePosW_wDoMotionBlur API_UNIFORM_CONSTANT_REGISTER_REG(20, 21);
	
	API_UNIFORM_CONSTANT Light gLight0                    API_UNIFORM_CONSTANT_REGISTER_REG(21, 22);
	API_UNIFORM_CONSTANT Light gLight1                    API_UNIFORM_CONSTANT_REGISTER_REG(28, 29);
	API_UNIFORM_CONSTANT Light gLight2                    API_UNIFORM_CONSTANT_REGISTER_REG(35, 36);
	API_UNIFORM_CONSTANT Light gLight3                    API_UNIFORM_CONSTANT_REGISTER_REG(42, 43);
	API_UNIFORM_CONSTANT Light gLight4                    API_UNIFORM_CONSTANT_REGISTER_REG(49, 50);
	API_UNIFORM_CONSTANT Light gLight5                    API_UNIFORM_CONSTANT_REGISTER_REG(56, 57);
	API_UNIFORM_CONSTANT Light gLight6                    API_UNIFORM_CONSTANT_REGISTER_REG(63, 64);
	API_UNIFORM_CONSTANT Light gLight7                    API_UNIFORM_CONSTANT_REGISTER_REG(70, 71);
END_BUFFER

////////////////////////////////////////////////////////////////////////////////
// Per object
////////////////////////////////////////////////////////////////////////////////
#undef API_BUFFER_INDEX
#define API_BUFFER_INDEX 2
BEGIN_BUFFER // total registers: 81 absoulte: [78-158]
	API_UNIFORM_CONSTANT matrix gWVP                                   API_UNIFORM_CONSTANT_REGISTER_REG(0, 78);
	API_UNIFORM_CONSTANT matrix gW                                     API_UNIFORM_CONSTANT_REGISTER_REG(4, 82);
	API_UNIFORM_CONSTANT matrix gUnused                                API_UNIFORM_CONSTANT_REGISTER_REG(8, 86);
	
	API_UNIFORM_CONSTANT float4 gVertexBufferWeights                   API_UNIFORM_CONSTANT_REGISTER_REG(12, 90);
	API_UNIFORM_CONSTANT matrix gWVPInverse                            API_UNIFORM_CONSTANT_REGISTER_REG(13, 91);
	
	 // IMPORTANT: we assume that for D3D9 PE_MAX_BONE_COUNT_IN_DRAW_CALL is 16. if not, need to change consecutive registers
	#if !APIABSTRACTION_D3D11
	API_UNIFORM_CONSTANT matrix gJoints[PE_MAX_BONE_COUNT_IN_DRAW_CALL] API_UNIFORM_CONSTANT_REGISTER_REG(17, 95);
	#endif
END_BUFFER

#if APIABSTRACTION_D3D11
/*
tbuffer tbPerObject : register(t65)
{
	matrix gJoints[PE_MAX_BONE_COUNT_IN_DRAW_CALL];  // note in dx11 we store all bones per draw call in tbuffer
};
*/
StructuredBuffer<matrix> gJoints : register(t65);
#endif

////////////////////////////////////////////////////////////////////////////////
// Per object part (by material)
////////////////////////////////////////////////////////////////////////////////
#undef API_BUFFER_INDEX
#define API_BUFFER_INDEX 3
BEGIN_BUFFER  // total registers: 4 absoulte: [159-162]
	API_UNIFORM_CONSTANT float4 xHasNrm_yHasSpec_zHasGlow_wHasDiff   API_UNIFORM_CONSTANT_REGISTER_REG(0, 159);
	API_UNIFORM_CONSTANT float4 gVDiffuse                            API_UNIFORM_CONSTANT_REGISTER_REG(1, 160);
	API_UNIFORM_CONSTANT float4 gxyzVSpecular_w                      API_UNIFORM_CONSTANT_REGISTER_REG(2, 161);
	API_UNIFORM_CONSTANT float4 gxyzVEmissive_wVShininess            API_UNIFORM_CONSTANT_REGISTER_REG(3, 162);
END_BUFFER

#endif // file guard


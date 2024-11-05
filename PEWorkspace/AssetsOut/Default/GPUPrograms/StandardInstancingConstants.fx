#ifndef HLSL_STANDARDINSTANCINGCONSTANTS
#define HLSL_STANDARDINSTANCINGCONSTANTS

#include "APIAbstraction.gpu"

#define APIABSTRACTION_D3D11 1

#if APIABSTRACTION_D3D11

#define PE_MAX_PARTICLE_PER_GROUP 1024
#define PE_MAX_PARTICLE_GROUP 1024

struct PerObjectInstanceData
{
	//matrix WVP;
	float4x3 W;
};

cbuffer cbInstanceControlConstants : register(b5)
{
	int4 gInstanceControl_xInstanceOffset_yzw;
};


struct BoneTQ
{
	float4 m_quat;
	float4 m_translation;
};

struct ParticleEmitter
{
    float4 m_position;
    float m_emissionRate;
};

struct Particle
{
	float4 m_position;
    float4 m_velocity;
    float4 m_acceleration;
    //float4 m_color;
    //float4 m_size;
    float m_age;
    float m_lifetime;
    //float m_random;  // 0 - 1
};

StructuredBuffer<Matrix> gInstancedBones : register(t64); // if cpu computes instanced palettes, they are put here
StructuredBuffer<BoneTQ> gAnimationData : register(t66);

// Compue Shader Map() ///////

#if PE_STORE_CS_MAP_RESULT_AS_MATRIX
	// in case of Compute Reduce, we will store data as Matrix so that we dont have to reconvert it
	RWStructuredBuffer<Matrix> gCSMapComputeTargetUAV : register(u0);
	StructuredBuffer<Matrix> gCSMapComputeTargetView : register(t67);  // used by second pass of compute shader
#else
	RWStructuredBuffer<BoneTQ> gCSMapComputeTargetUAV : register(u0);
	StructuredBuffer<BoneTQ> gCSMapComputeTargetView : register(t67);  // used by second pass of compute shader
#endif

// Particle Emitters, only 1 is used for now
StructuredBuffer<ParticleEmitter> gParticleEmitters : register(t70); // array of particle emitters

// Particle CS buffer 1
RWStructuredBuffer<Particle> gCSMapParticleTarget1UAV : register(u1);  // used by compute shader
StructuredBuffer<Particle> gCSMapParticleComputeTarget1View : register(t71); // same as above, used by second pass

// Particle CS buffer 2
RWStructuredBuffer<Particle> gCSMapParticleTarget2UAV : register(u2);
StructuredBuffer<Particle> gCSMapParticleComputeTarget2View : register(t72);

// Indirect draw args
RWByteAddressBuffer gIndirectDrawArgsUAV : register(u3);

// Indirect dispatch args
RWByteAddressBuffer gIndirectDispatchArgsUAV : register(u4);

//RWByteAddressBuffer gCSFreeParticleBuffer : register(u2);  // used by compute shader to store free particle indices

// Compute Shader Reduce()

struct JointParentData
{
	int m_parentJointIndex;
};

StructuredBuffer<JointParentData> gSkeletonStructureView : register(t68);
StructuredBuffer<Matrix> gBoneInverses : register(t69);
StructuredBuffer<PerObjectInstanceData> gPerInstanceDataBuffer : register(t70);

RWStructuredBuffer<Matrix> gCSReduceComputeTargetUAV : register(u0); // we can reuse same u0 as above since we know one of these will be bound as UAV, not both at at time
StructuredBuffer<Matrix> gCSReduceComputeTargetView : register(t67); // used by VS pass after computing is done. We can resue t register from above since we know that only one will be bound at a time

#else // elif APIABSTRACTION_D3D9 | OGL | PS3

#endif

#endif // file guard

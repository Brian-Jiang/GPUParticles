#ifndef __PYENGINE_2_0_ANIMSET_BUFFER_GPU__
#define __PYENGINE_2_0_ANIMSET_BUFFER_GPU__
// This class works with vertex buffers in GPU memory

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/Render/IRenderer.h"
#include "PrimeEngine/Geometry/PositionBufferCPU/PositionBufferCPU.h"
#include "PrimeEngine/Geometry/TangentBufferCPU/TangentBufferCPU.h"
#include "PrimeEngine/Geometry/TexCoordBufferCPU/TexCoordBufferCPU.h"
#include "PrimeEngine/Geometry/ParticleSystemCPU/ParticleBufferCPU.h"
#include "PrimeEngine/Geometry/SkeletonCPU/SkinWeightsCPU.h"
#include "PrimeEngine/Geometry/SkeletonCPU/AnimationSetCPU.h"

#include "PrimeEngine/Geometry/SkeletonCPU/SkeletonCPU.h"

#include "PrimeEngine/Geometry/NormalBufferCPU/NormalBufferCPU.h"
#include "PrimeEngine/Geometry/ColorBufferCPU.h"
#if APIABSTRACTION_D3D9
#include "PrimeEngine/APIAbstraction/DirectX9/D3D9_GPUBuffers/D3D9_VertexBufferGPU.h"
#elif APIABSTRACTION_D3D11
#include "PrimeEngine/APIAbstraction/DirectX11/D3D11_GPUBuffers/D3D11_VertexBufferGPU.h"
#elif APIABSTRACTION_OGL
#include "PrimeEngine/APIAbstraction/OGL/OGL_GPUBuffers/OGL_VertexBufferGPU.h"
#endif

#include "IndexBufferGPU.h"
#include "BufferInfo.h"

// Sibling/Children includes
namespace PE {
namespace Components
{
struct Effect;
struct DrawList;
}

class AnimSetBufferGPU : public PE::PEAllocatableAndDefragmentable
{
public:

public:

	AnimSetBufferGPU(PE::GameContext &context, PE::MemoryArena arena);

	~AnimSetBufferGPU();

	void createGPUBufferFromAnimSet(AnimationSetCPU &animSetCpu);
	
	static void createGPUBufferForAnimationCSResult(PE::GameContext &ctx);
	
	void createShaderValue(Components::DrawList *pDrawList);

	static void createShaderValueForCSMapUAV(PE::GameContext &context, PE::MemoryArena arena, Components::DrawList *pDrawList);
	static void createShaderValueForVSViewOfCSMap(PE::GameContext &context, PE::MemoryArena arena, Components::DrawList *pDrawList);

	static void createShaderValueForCSReduceUAV(PE::GameContext &context, PE::MemoryArena arena, Components::DrawList *pDrawList);
	static void createShaderValueForVSViewOfCSReduce(PE::GameContext &context, PE::MemoryArena arena, Components::DrawList *pDrawList);

	static void mapCSResultsForCpuRead(PE::GameContext &context, PE::MemoryArena arena);

	void releaseGPUBuffer();
#if PE_API_IS_D3D11

	ID3D11Buffer *m_pBuf;
	ID3D11ShaderResourceView *m_pShaderResourceView;

	static ID3D11ShaderResourceView *s_pAnimationCSMapResultShaderResourceView;
	static ID3D11UnorderedAccessView *s_pAnimationCSMapResultShaderUAView;
	static ID3D11Buffer *s_pAnimationCSResultBuffer;
	static ID3D11Buffer *s_pAnimationCSResultBufferCpuMirror;
	static void *s_pAnimationCSResultBufferCpuMirrorMem;
	static int s_pAnimationCSResultBufferCpuMirrorMemSize;

	static ID3D11UnorderedAccessView *s_pCSReduceComputeTargetUAV;
	static ID3D11ShaderResourceView *s_pCSReduceComputeTargetView;
	static ID3D11Buffer *s_pCSReduceComputeTargetBuffer;
	static ID3D11Buffer *s_pCSReduceComputeTargetStagingBuffer;
	static void *s_pCSReduceComputeTargetCpuMem;
	static int s_pCSReduceComputeTargetCpuMemSize;


	static ID3D11Buffer *s_pAnimationCSResultBuffersCpuStaging[PE_MAX_NUM_OF_BUFFER_STEPS];

	
	
#endif	
	PE::MemoryArena m_arena; PE::GameContext *m_pContext;
	Handle m_hAnimationSetCPU;
};

}; // namespace PE

#endif

#ifndef __PYENGINE_2_0_RESOURCE_BUFFER_GPU__
#define __PYENGINE_2_0_RESOURCE_BUFFER_GPU__
// This class works with vertex buffers in GPU memory

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#if APIABSTRACTION_D3D11

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/Render/IRenderer.h"
#include "../../Geometry/PositionBufferCPU/PositionBufferCPU.h"
#include "../../Geometry/TangentBufferCPU/TangentBufferCPU.h"
#include "../../Geometry/TexCoordBufferCPU/TexCoordBufferCPU.h"
#include "../../Geometry/ParticleSystemCPU/ParticleBufferCPU.h"
#include "../../Geometry/SkeletonCPU/SkinWeightsCPU.h"

#include "../../Geometry/NormalBufferCPU/NormalBufferCPU.h"
#if APIABSTRACTION_D3D9
//#include "../DirectX10/D3D10_GPUBuffers/D3D10_ResourceBufferGPU.h"
#elif APIABSTRACTION_D3D11
#include "../DirectX11/D3D11_GPUBuffers/D3D11_ResourceBufferGPU.h"
#include "VertexBufferGPU.h"
#endif
// Sibling/Children includes

namespace PE {

struct ResourceBufferGPU : public PE::PEAllocatableAndDefragmentable
{
	ResourceBufferGPU(PE::GameContext &context, PE::MemoryArena arena)
	{
		m_arena = arena; m_pContext = &context;
	}
	void createGPUBufferFromVertexBufferGPU(VertexBufferGPU *pVBufGPU);

	void createGPUBuffer(
		void *pData, PrimitiveTypes::UInt32 structSize, PrimitiveTypes::UInt32 numStructs, PrimitiveTypes::Bool constant = true
	);

	void copyIntoVertexBuffer(
		// Input
		VertexBufferGPU *pVBufGPU
	);

	void ResourceBufferGPU::copyIntoVertexBuffer(
		// Input
		ID3D11Buffer *
	);

	void ResourceBufferGPU::copyIntoResourceBuffer(
		// Input
		ID3D11Buffer *pResourceBuffer
	);

	void *mapToPtr();
	void releasePtr();

	D3D11_ResourceBufferGPU m_resourceBufferLowLevel;

	PE::MemoryArena m_arena; PE::GameContext *m_pContext;
};

}; // namespace PE

#endif

#endif

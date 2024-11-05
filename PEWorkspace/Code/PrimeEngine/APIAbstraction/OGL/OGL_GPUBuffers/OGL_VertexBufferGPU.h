#ifndef __PYENGINE_2_0_OGL_VERTEX_BUFFER_GPU__
#define __PYENGINE_2_0_OGL_VERTEX_BUFFER_GPU__
// This class is a helper for VertexBufferGPU
// IMPORTANT: In D3D vertex buffers store all vertex information: position, normals, tex coordinates
// Thus, in D3D there are no Normal buffers, or tex coord buffers per se; they are part of vertex buffers


// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#if APIABSTRACTION_OGL

// Outer-Engine includes

// Inter-Engine includes
#include "../../../PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/Render/GLRenderer.h"
// Sibling/Children includes

#include "../../../Geometry/PositionBufferCPU/PositionBufferCPU.h"
#include "../../../Geometry/ColorBufferCPU.h"
#include "../../../Geometry/TexCoordBufferCPU/TexCoordBufferCPU.h"
#include "../../../Geometry/NormalBufferCPU/NormalBufferCPU.h"
#include "../../../Geometry/TangentBufferCPU/TangentBufferCPU.h"
#include "../../../Geometry/SkeletonCPU/SkinWeightsCPU.h"

#include "../../GPUBuffers/BufferInfo.h"

struct OGL_VertexBufferGPU : public PE::PEAllocatableAndDefragmentable
{
	static GLuint CreateVertexBufferInGPUFromVbNbTb(
		PE::GameContext &context,
		PE::MemoryArena arena,

		PositionBufferCPU *pDataVb,
		ColorBufferCPU *pDataCb,
		TexCoordBufferCPU *pDataTCb,
		NormalBufferCPU *pDataNb,
		TangentBufferCPU *pDataTb,
		
		SkinWeightsCPU *pDataSkinWeights,
		
		PrimitiveTypes::Bool constant = true,
		// Output
		GLuint *out_vbos = NULL
	);

	static void releaseGPUBuffers(GLuint vao, GLuint *vbos, int numVBOs);

	static void setAsCurrent(
		//IDirect3DDevice9 *pD3DDevice, 
		GLuint vaoId, PrimitiveTypes::UInt32 vertexSize, PrimitiveTypes::UInt32 startOffset);
};

#endif // APIAbstraction
#endif // file guard

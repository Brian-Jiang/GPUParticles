#ifndef __PYENGINE_2_0_OGL_INDEX_BUFFER_GPU__
#define __PYENGINE_2_0_OGL_INDEX_BUFFER_GPU__
// This class is a helper for IndexBufferGPU

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#if APIABSTRACTION_OGL

// Inter-Engine includes
#include "PrimeEngine/Render/GLRenderer.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/MemoryManagement/Handle.h"

// Outer-Engine includes


// Sibling/Children includes

struct OGL_IndexBufferGPU : public PE::PEAllocatableAndDefragmentable
{
	static GLuint CreateIndexBufferInGPU(void *indices, PrimitiveTypes::UInt32 size, PrimitiveTypes::Bool constant = true);

	// set as current does nto exist for OGL since all buffers are stored in one mega buffer
	//static void setAsCurrent(IDirect3DDevice9 *pD3DDevice, IDirect3DIndexBuffer9 *pBuf, PrimitiveTypes::UInt32 vertexSize, PrimitiveTypes::UInt32 startOffset);
	
	static void releaseGPUBuffer(GLuint bufId);
};

#endif // APIAbstraction
#endif // file guard

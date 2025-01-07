#ifndef __PYENGINE_2_0_D3D10_INDEX_BUFFER_GPU__
#define __PYENGINE_2_0_D3D10_INDEX_BUFFER_GPU__
// This class is a helper for IndexBufferGPU

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#include "PrimeEngine/Utils/PEClassDecl.h"
#if APIABSTRACTION_D3D9

// Inter-Engine includes
#include "PrimeEngine/Render/D3D9Renderer.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/MemoryManagement/Handle.h"

// Outer-Engine includes
#include <d3d9.h>

// Sibling/Children includes

struct D3D9_IndexBufferGPU : PE::PEAllocatable
{
	static IDirect3DIndexBuffer9 *CreateIndexBufferInGPU(IDirect3DDevice9 *pD3DDevice, void *indices, PrimitiveTypes::UInt32 size, PrimitiveTypes::Bool constant = true);

	static void setAsCurrent(IDirect3DDevice9 *pD3DDevice, IDirect3DIndexBuffer9 *pBuf, PrimitiveTypes::UInt32 vertexSize, PrimitiveTypes::UInt32 startOffset);
	
};

#endif // APIAbstraction
#endif // file guard

#ifndef __PYENGINE_2_0_D3D11_INDEX_BUFFER_GPU__
#define __PYENGINE_2_0_D3D11_INDEX_BUFFER_GPU__
// This class is a helper for IndexBufferGPU

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#if APIABSTRACTION_D3D11

// Inter-Engine includes
#include "PrimeEngine/Render/D3D11Renderer.h"
#include "../../../PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/MemoryManagement/Handle.h"

// Outer-Engine includes


// Sibling/Children includes

struct D3D11_IndexBufferGPU : PE::PEAllocatable
{
	static ID3D11Buffer *CreateIndexBufferInGPU(ID3D11Device *pD3DDevice, void *indices, PrimitiveTypes::UInt32 size, PrimitiveTypes::Bool constant = true);

	static void setAsCurrent(ID3D11DeviceContext *pD3DContext, ID3D11Buffer *pBuf, PrimitiveTypes::UInt32 vertexSize, PrimitiveTypes::UInt32 startOffset);
};

#endif // APIAbstraction
#endif // file guard

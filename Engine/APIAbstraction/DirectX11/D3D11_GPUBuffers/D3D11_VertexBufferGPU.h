#ifndef __PYENGINE_2_0_D3D11_VERTEX_BUFFER_GPU__
#define __PYENGINE_2_0_D3D11_VERTEX_BUFFER_GPU__
// This class is a helper for VertexBufferGPU
// IMPORTANT: In D3D vertex buffers store all vertex information: position, normals, tex coordinates
// Thus, in D3D there are no Normal buffers, or tex coord buffers per se; they are part of index buffers


// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#if APIABSTRACTION_D3D11

// Outer-Engine includes

// Inter-Engine includes
#include "../../../PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/Render//D3D11Renderer.h"
// Sibling/Children includes

#include "../../../Geometry/PositionBufferCPU/PositionBufferCPU.h"
#include "../../../Geometry/TexCoordBufferCPU/TexCoordBufferCPU.h"
#include "../../../Geometry/NormalBufferCPU/NormalBufferCPU.h"
#include "../../../Geometry/TangentBufferCPU/TangentBufferCPU.h"

struct D3D11_VertexBufferGPU : PE::PEAllocatable
{
	static ID3D11Buffer *CreateVertexBufferInGPUFromVb(
		// Input
		ID3D11Device *pD3DDevice, void *pData, PrimitiveTypes::UInt32 size, 
		PrimitiveTypes::Bool constant = true, PrimitiveTypes::Bool streamOutput = false
		// Output
		//ID3D11
	);
	
	static void setAsCurrent(ID3D11DeviceContext *pD3DDeviceContext, ID3D11Buffer *pBuf, PrimitiveTypes::UInt32 vertexSize, PrimitiveTypes::UInt32 startOffset);
	static void setAsCurrent(ID3D11DeviceContext *pD3DDeviceContext, 
		Array<ID3D11Buffer *> &pBufs, 
		Array<PrimitiveTypes::UInt32> &vertexSizes, 
		Array<PrimitiveTypes::UInt32> &startOffsets);
};

#endif // APIAbstraction
#endif // file guard

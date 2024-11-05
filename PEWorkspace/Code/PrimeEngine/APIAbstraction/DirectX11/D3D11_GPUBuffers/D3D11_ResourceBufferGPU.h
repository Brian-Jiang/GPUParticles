#ifndef __PYENGINE_2_0_D3D11_RESOURCE_BUFFER_GPU__
#define __PYENGINE_2_0_D3D11_RESOURCE_BUFFER_GPU__
// this class helps creating resource buffers in GPU, they are like textures, but one-dimentional
// and can hold any structures

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#if APIABSTRACTION_D3D11

// Outer-Engine includes


// Inter-Engine includes
#include "../../../PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/Render/D3D11Renderer.h"
// Sibling/Children includes

#include "D3D11_VertexBufferGPU.h"

#include "../../../Geometry/PositionBufferCPU/PositionBufferCPU.h"
#include "../../../Geometry/TexCoordBufferCPU/TexCoordBufferCPU.h"
#include "../../../Geometry/NormalBufferCPU/NormalBufferCPU.h"
#include "../../../Geometry/TangentBufferCPU/TangentBufferCPU.h"

struct D3D11_ResourceBufferGPU : PE::PEAllocatable
{
	void CreateResourceBufferInGPU(
		// Input
		ID3D11Device *pD3DDevice, void *pData, PrimitiveTypes::UInt32 structSize, PrimitiveTypes::UInt32 numStructs, PrimitiveTypes::Bool constant = true
		// Output
	);

	void CreateResourceBufferInGPUFromVertexBuffer(
		// Input
		ID3D11Device *pD3DDevice, ID3D11DeviceContext *pContext,
		ID3D11Buffer *pBuf,
		PrimitiveTypes::UInt32 structSize, PrimitiveTypes::UInt32 numStructs, PrimitiveTypes::Bool constant = true
	);

	void copyIntoVertexBuffer(
		// Input
		ID3D11DeviceContext *pContext,
		ID3D11Buffer *pBuf
	);
	

	void *mapToPtr(ID3D11Device *pD3DDevice, ID3D11DeviceContext *pContext);
	void releasePtr(ID3D11DeviceContext *pContext);
	
	ID3D11Buffer *m_pBuffer;
	ID3D11Buffer* m_pTmpBuffer;

	ID3D11ShaderResourceView *m_pShaderResourceView;
	ID3D11UnorderedAccessView *m_pShaderResourceUAV;
};

#endif // APIAbstraction
#endif // file guard

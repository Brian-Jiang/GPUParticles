// This class is a helper for VertexBufferGPU
// IMPORTANT: In D3D vertex buffers store all vertex information: position, normals, tex coordinates
// Thus, in D3D there are no Normal buffers, or tex coord buffers per se; they are part of index buffers


// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#if APIABSTRACTION_D3D9

// Outer-Engine includes


// Inter-Engine includes

// Sibling/Children includes
#include "D3D9_VertexBufferGPU.h"

IDirect3DVertexBuffer9 *D3D9_VertexBufferGPU::CreateVertexBufferInGPUFromVb(
	// Input
	IDirect3DDevice9 *pD3DDevice, void *pData, PrimitiveTypes::UInt32 size, PrimitiveTypes::Bool constant //  = true
	// Output
)
{
	IDirect3DVertexBuffer9 *pBuffer;

	pD3DDevice->CreateVertexBuffer(size,
		constant ? 0 : D3DUSAGE_DYNAMIC,
		D3DFVF_XYZ,
		D3DPOOL_MANAGED,
		&pBuffer,
		NULL);

	if (pData != 0)
	{
		VOID* pVoid;
		// lock v_buffer and load the vertices into it
		pBuffer->Lock(0, 0, (void**)&pVoid, 0);
		memcpy(pVoid, pData, size);
		pBuffer->Unlock();
	}
	return pBuffer;
}

void D3D9_VertexBufferGPU::setAsCurrent(IDirect3DDevice9 *pD3DDevice, IDirect3DVertexBuffer9 *pBuf, PrimitiveTypes::UInt32 vertexSize, PrimitiveTypes::UInt32 startOffset)
{
	pD3DDevice->SetStreamSource(0, pBuf, startOffset, vertexSize);
}

void D3D9_VertexBufferGPU::setAsCurrent(IDirect3DDevice9 *pD3DDevice, 
		Array<IDirect3DVertexBuffer9 *> &pBufs, 
		Array<PrimitiveTypes::UInt32> &vertexSizes, 
		Array<PrimitiveTypes::UInt32> &startOffsets)
{
#if 0
	pD3DDevice->IASetVertexBuffers(0, pBufs.m_size, &pBufs[0], &vertexSizes[0], &startOffsets[0]);
#else
	for (unsigned int i = 0; i < pBufs.m_size; i++)
	{
		IDirect3DVertexBuffer9 *ppD3DBuf = pBufs[i];
		PrimitiveTypes::UInt32 *pSize = &vertexSizes[i];
		PrimitiveTypes::UInt32 *pOffset = &startOffsets[i];
		pD3DDevice->SetStreamSource(i, ppD3DBuf, *pOffset, *pSize);
	}
#endif
}

#endif // APIAbstraction

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#if APIABSTRACTION_D3D9

// Inter-Engine includes

// Outer-Engine includes


// Sibling/Children includes
#include "D3D9_IndexBufferGPU.h"

IDirect3DIndexBuffer9 *D3D9_IndexBufferGPU::CreateIndexBufferInGPU(IDirect3DDevice9 *pD3DDevice, void *indices, PrimitiveTypes::UInt32 size, PrimitiveTypes::Bool constant/*DEF = true*/)
{
	IDirect3DIndexBuffer9 *mIB;
	pD3DDevice->CreateIndexBuffer(size, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &mIB, NULL);
	if (indices != 0)
	{
		VOID* pVoid;
		// lock v_buffer and load the vertices into it
		mIB->Lock(0, 0, (void**)&pVoid, 0);
		memcpy(pVoid, indices, size);
		mIB->Unlock();
	}
	return mIB;
}

void D3D9_IndexBufferGPU::setAsCurrent(IDirect3DDevice9 *pD3DDevice, IDirect3DIndexBuffer9 *pBuf, PrimitiveTypes::UInt32 vertexSize, PrimitiveTypes::UInt32 startOffset)
{
	if (startOffset != 0)
	{
		assert(!"DX9 Does not support offset for index buffers");
	}
	pD3DDevice->SetIndices(pBuf);
}


#endif // APIAbstraction

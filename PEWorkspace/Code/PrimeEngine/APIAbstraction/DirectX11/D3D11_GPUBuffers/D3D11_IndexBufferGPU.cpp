// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#if APIABSTRACTION_D3D11

// Inter-Engine includes

// Outer-Engine includes

// Sibling/Children includes
#include "D3D11_IndexBufferGPU.h"

ID3D11Buffer *D3D11_IndexBufferGPU::CreateIndexBufferInGPU(ID3D11Device *pD3DDevice, void *indices, PrimitiveTypes::UInt32 size, PrimitiveTypes::Bool constant/*DEF = true*/)
{
	D3D11_BUFFER_DESC ibd;
		ibd.Usage = constant ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DEFAULT;
		ibd.ByteWidth = size;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
	
	D3D11_SUBRESOURCE_DATA iinitData;
		iinitData.pSysMem = indices;
	
	ID3D11Buffer *mIB;
	pD3DDevice->CreateBuffer(&ibd, &iinitData, &mIB);

	return mIB;
}

void D3D11_IndexBufferGPU::setAsCurrent(ID3D11DeviceContext *pD3DContext, ID3D11Buffer *pBuf, PrimitiveTypes::UInt32 vertexSize, PrimitiveTypes::UInt32 startOffset)
{
	pD3DContext->IASetIndexBuffer(pBuf, DXGI_FORMAT_R16_UINT, startOffset);
}


#endif // APIAbstraction

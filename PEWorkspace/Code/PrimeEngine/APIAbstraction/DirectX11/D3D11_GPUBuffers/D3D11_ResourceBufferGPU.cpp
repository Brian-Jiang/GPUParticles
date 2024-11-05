// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#if APIABSTRACTION_D3D11

// Outer-Engine includes


// Inter-Engine includes

// Sibling/Children includes
#include "D3D11_ResourceBufferGPU.h"

void D3D11_ResourceBufferGPU::CreateResourceBufferInGPU(
	// Input
	ID3D11Device *pD3DDevice, void *pData, PrimitiveTypes::UInt32 structSize, PrimitiveTypes::UInt32 numStructs, PrimitiveTypes::Bool constant
	// Output
)
{
	HRESULT hr;

	D3D11_BUFFER_DESC sbDesc;
		sbDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		sbDesc.CPUAccessFlags = 0;
		sbDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		sbDesc.StructureByteStride = structSize;
		sbDesc.ByteWidth = structSize * numStructs;
		sbDesc.Usage = D3D11_USAGE_DEFAULT;
	D3D11_SUBRESOURCE_DATA  InitData;
		InitData.pSysMem = pData;
	hr = pD3DDevice->CreateBuffer(&sbDesc, &InitData, &m_pBuffer);

	D3D11_UNORDERED_ACCESS_VIEW_DESC sbUAVDesc;
	sbUAVDesc.Buffer.FirstElement = 0;
	sbUAVDesc.Buffer.Flags = 0;
	sbUAVDesc.Buffer.NumElements = numStructs;
	sbUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	sbUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	hr = pD3DDevice->CreateUnorderedAccessView(m_pBuffer, &sbUAVDesc, &m_pShaderResourceUAV);
	
	D3D11_SHADER_RESOURCE_VIEW_DESC sbSRVDesc;
		sbSRVDesc.Buffer.ElementOffset = 0;
		sbSRVDesc.Buffer.ElementWidth = structSize;
		sbSRVDesc.Buffer.FirstElement = 0;
		sbSRVDesc.Buffer.NumElements = numStructs;
		sbSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		sbSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	hr = pD3DDevice->CreateShaderResourceView(m_pBuffer, &sbSRVDesc, &m_pShaderResourceView);
}

void *D3D11_ResourceBufferGPU::mapToPtr(ID3D11Device *pD3DDevice, ID3D11DeviceContext *pContext)
{
	m_pTmpBuffer = NULL;

	D3D11_BUFFER_DESC desc;
	memset( &desc, 0, sizeof(desc) );
	m_pBuffer->GetDesc( &desc );
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	pD3DDevice->CreateBuffer(&desc, NULL, &m_pTmpBuffer);

	pContext->CopyResource( m_pTmpBuffer, m_pBuffer );

	D3D11_MAPPED_SUBRESOURCE MappedResource; 
	pContext->Map( m_pTmpBuffer, 0, D3D11_MAP_READ, 0, &MappedResource );
	return MappedResource.pData;
}

void D3D11_ResourceBufferGPU::releasePtr(ID3D11DeviceContext *pContext)
{
	pContext->Unmap( m_pTmpBuffer, 0 );
	m_pTmpBuffer->Release();
}

void D3D11_ResourceBufferGPU::CreateResourceBufferInGPUFromVertexBuffer(
	// Input
	ID3D11Device *pD3DDevice, ID3D11DeviceContext *pContext,
	ID3D11Buffer *pBuffer,
	PrimitiveTypes::UInt32 structSize, PrimitiveTypes::UInt32 numStructs, PrimitiveTypes::Bool
)
{
	D3D11_BUFFER_DESC desc;
	memset( &desc, 0, sizeof(desc) );
	pBuffer->GetDesc( &desc );
	desc.CPUAccessFlags = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	desc.StructureByteStride = structSize;

	assert(desc.ByteWidth == structSize * numStructs); // if fails here, means that the buffer stored in vertex buffer
	// does not have the amount of data that developer thinks

	HRESULT hr = pD3DDevice->CreateBuffer(&desc, NULL, &m_pBuffer); // no init data

	pContext->CopyResource( m_pBuffer, pBuffer ); // copy values

	D3D11_UNORDERED_ACCESS_VIEW_DESC sbUAVDesc;
	sbUAVDesc.Buffer.FirstElement = 0;
	sbUAVDesc.Buffer.Flags = 0;
	sbUAVDesc.Buffer.NumElements = numStructs;
	sbUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	sbUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	hr = pD3DDevice->CreateUnorderedAccessView(m_pBuffer, &sbUAVDesc, &m_pShaderResourceUAV);
	
	D3D11_SHADER_RESOURCE_VIEW_DESC sbSRVDesc;
		sbSRVDesc.Buffer.ElementOffset = 0;
		sbSRVDesc.Buffer.ElementWidth = structSize;
		sbSRVDesc.Buffer.FirstElement = 0;
		sbSRVDesc.Buffer.NumElements = numStructs;
		sbSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		sbSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	hr = pD3DDevice->CreateShaderResourceView(m_pBuffer, &sbSRVDesc, &m_pShaderResourceView);
}

void D3D11_ResourceBufferGPU::copyIntoVertexBuffer(
		// Input
	ID3D11DeviceContext *pContext,
	ID3D11Buffer *pBuffer
)
{
	pContext->CopyResource( pBuffer, m_pBuffer); // copy values

}

	
#endif // APIAbstraction


// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#if APIABSTRACTION_D3D11

#include "ResourceBufferGPU.h"

// Outer-Engine includes

// Inter-Engine includes

namespace PE {

void ResourceBufferGPU::createGPUBuffer(
	void *pData, PrimitiveTypes::UInt32 structSize, PrimitiveTypes::UInt32 numStructs, PrimitiveTypes::Bool constant
)
{

#		if APIABSTRACTION_D3D9
		// will use D3D9_VertexBufferGPU

		// retrieve device from IRenderer
		ID3D10Device *pDevice = IRenderer::Instance()->m_device;

#		elif APIABSTRACTION_D3D11
		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

		m_resourceBufferLowLevel.CreateResourceBufferInGPU(pDevice,
			pData, structSize, numStructs, constant);
#		endif
}
void *ResourceBufferGPU::mapToPtr()
{
#		if APIABSTRACTION_D3D9
		
		// retrieve device from IRenderer
		ID3D10Device *pDevice = IRenderer::Instance()->m_device;
		assert(!"Not Implemented!");
#		elif APIABSTRACTION_D3D11
	
		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

		return m_resourceBufferLowLevel.mapToPtr(pDevice, pDeviceContext);
#		endif

}

void ResourceBufferGPU::releasePtr()
{
#		if APIABSTRACTION_D3D9
		
		// retrieve device from IRenderer
		ID3D10Device *pDevice = IRenderer::Instance()->m_device;
		assert(!"Not Implemented!");
#		elif APIABSTRACTION_D3D11
		
		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

		return m_resourceBufferLowLevel.releasePtr(pDeviceContext);
#		endif

}

void ResourceBufferGPU::createGPUBufferFromVertexBufferGPU(VertexBufferGPU *pVBufGPU)
{
	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

	m_resourceBufferLowLevel.CreateResourceBufferInGPUFromVertexBuffer(pDevice, pDeviceContext,
		pVBufGPU->m_pBuf, pVBufGPU->m_vertexSize, pVBufGPU->m_length, true);

}

void ResourceBufferGPU::copyIntoVertexBuffer(
	// Input
	VertexBufferGPU *pDestVBufGPU
)
{
	assert(!"Not implemented!");
#if 0
	ID3D11DeviceContext *pDeviceContext = IRenderer::Instance()->m_deviceContext;
	m_resourceBufferLowLevel.copyIntoVertexBuffer(pDeviceContext, pDestVBufGPU->m_pBuf>());
#endif
}
void ResourceBufferGPU::copyIntoVertexBuffer(
	// Input
	ID3D11Buffer *pDestBuffer
)
{
	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

	m_resourceBufferLowLevel.copyIntoVertexBuffer(pDeviceContext, pDestBuffer);
}

void ResourceBufferGPU::copyIntoResourceBuffer(
	// Input
	ID3D11Buffer *pResourceBuffer
)
{
	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

	m_resourceBufferLowLevel.copyIntoVertexBuffer(pDeviceContext, pResourceBuffer);
}

}; // namespace PE
#endif

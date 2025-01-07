// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/AnimSetBufferGPU.h"
#include "PrimeEngine/Scene/MeshInstance.h"
#include "PrimeEngine/Scene/Skeleton.h"
#include "PrimeEngine/Scene/DefaultAnimationSM.h"


// Sibling/Children includes
#include "SA_SetAndBind_ConstResource_PerInstanceData.h"

namespace PE {
using namespace Components;

#if PE_API_IS_D3D11

void SA_SetAndBind_ConstResource_PerInstanceData::bindToPipeline(Effect *pCurEffect /* = NULL*/)
{
#	if APIABSTRACTION_D3D11
	/*
			typedef enum D3D11_MAP {
				D3D11_MAP_READ                 = 1,
				D3D11_MAP_WRITE                = 2,
				D3D11_MAP_READ_WRITE           = 3,
				D3D11_MAP_WRITE_DISCARD        = 4,
				D3D11_MAP_WRITE_NO_OVERWRITE   = 5 
			} D3D11_MAP; */

	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(pCurEffect->m_pContext->getGPUScreen());
	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

	PrimitiveTypes::UInt32 slotId = 70;
	ID3D11ShaderResourceView *pView = NULL;
	ID3D11Resource *pBuf = NULL;

	{
		slotId = 70;
		pView = s_pShaderResourceView;
		pBuf = SA_SetAndBind_ConstResource_PerInstanceData::s_pBuffer;
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pDeviceContext->Map(
		pBuf, // resource
		0, // subresource
		D3D11_MAP_WRITE_DISCARD, 
		0, 
		&mappedResource);
	assert(SUCCEEDED(hr));
	// set value
	int size = sizeof(PerObjectInstanceData) * m_numInstances;

	memcpy(mappedResource.pData, &m_data, size);
	
	pDeviceContext->Unmap(pBuf, 0);
	
	pDeviceContext->VSSetShaderResources(slotId, 1, &pView);
	pDeviceContext->GSSetShaderResources(slotId, 1, &pView);
	pDeviceContext->PSSetShaderResources(slotId, 1, &pView);
	pDeviceContext->CSSetShaderResources(slotId, 1, &pView);

#	endif
}

void SA_SetAndBind_ConstResource_PerInstanceData::unbindFromPipeline(Components::Effect *pCurEffect /* = NULL*/)
{
	PrimitiveTypes::UInt32 slotId = 70;
	ID3D11ShaderResourceView *pView = NULL;

	{
		slotId = 70;
	}

	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(pCurEffect->m_pContext->getGPUScreen());
	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;


	pDeviceContext->VSSetShaderResources(slotId, 1, &pView);
	pDeviceContext->GSSetShaderResources(slotId, 1, &pView);
	pDeviceContext->PSSetShaderResources(slotId, 1, &pView);
	pDeviceContext->CSSetShaderResources(slotId, 1, &pView);
}

void SA_SetAndBind_ConstResource_PerInstanceData::releaseData()
{
}

#endif
}; // namespace PE

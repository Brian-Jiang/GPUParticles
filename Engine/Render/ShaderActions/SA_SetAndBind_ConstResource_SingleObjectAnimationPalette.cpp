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
#include "SA_SetAndBind_ConstResource_SingleObjectAnimationPalette.h"
namespace PE {
using namespace Components;
#if PE_API_IS_D3D11

void SA_SetAndBind_ConstResource_SingleObjectAnimationPalette::bindToPipeline(Effect *pCurEffect /* = NULL*/)
{
#	if APIABSTRACTION_D3D11
	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(pCurEffect->m_pContext->getGPUScreen());
	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

	//Effect::setConstantBuffer(s_pBuffer, 4, &m_data, sizeof(Data));

	/*
			typedef enum D3D11_MAP {
				D3D11_MAP_READ                 = 1,
				D3D11_MAP_WRITE                = 2,
				D3D11_MAP_READ_WRITE           = 3,
				D3D11_MAP_WRITE_DISCARD        = 4,
				D3D11_MAP_WRITE_NO_OVERWRITE   = 5 
			} D3D11_MAP; */

	ID3D11ShaderResourceView *pView = NULL;
	ID3D11Resource *pBuf = NULL;
	
	pView = s_pShaderResourceViewSinglePalette;
	pBuf = s_pBufferSinglePalette;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pDeviceContext->Map(
		pBuf, // resource
		0, // subresource
		D3D11_MAP_WRITE_DISCARD, 
		0, 
		&mappedResource);
	assert(SUCCEEDED(hr));
	// set value
	int size = sizeof(m_data);

	memcpy(mappedResource.pData, &m_data, size);
	
	pDeviceContext->Unmap(pBuf, 0);

	// buffer has been set, now bind it
	SA_Bind_Resource::bindToPipeline(pCurEffect);
#	endif
}

void SA_SetAndBind_ConstResource_SingleObjectAnimationPalette::releaseData()
{
	SA_Bind_Resource::releaseData();
}
#endif
}; // namespace PE

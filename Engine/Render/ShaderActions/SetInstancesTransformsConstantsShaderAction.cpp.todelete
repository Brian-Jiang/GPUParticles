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
#include "SetInstancesTransformsConstantsShaderAction.h"
namespace PE {
using namespace Components;

void SetInstancesTransformsConstantsShaderAction::bindToPipeline(Effect *pCurEffect /* = NULL*/)
{
#if PE_PLAT_IS_PSVITA
	PSVitaRenderer *pPSVitaRenderer = static_cast<PSVitaRenderer *>(pCurEffect->m_pContext->getGPUScreen());
	memcpy(s_pBuffer, &m_data, sizeof(m_data));
	SceGxmErrorCode errCode = sceGxmSetVertexUniformBuffer(pPSVitaRenderer->m_context, 4, s_pBuffer); // todo: use buferring
	PEASSERT(errCode == SCE_OK, "Error setting unfiorm buffer");
#	elif APIABSTRACTION_D3D11

	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(pCurEffect->m_pContext->getGPUScreen());
	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

	Effect::setConstantBuffer(pDevice, pDeviceContext, s_pBuffer, 4, &m_data, sizeof(Data));
#	endif
}

void SetInstancesTransformsConstantsShaderAction::unbindFromPipeline(Components::Effect *pCurEffect /* = NULL*/)
{}

void SetInstancesTransformsConstantsShaderAction::releaseData()
{}

}; // namespace PE

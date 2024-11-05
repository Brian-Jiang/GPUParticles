
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/ResourceBufferGPU.h"
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"


// Sibling/Children includes
#include "SetPerFrameConstantsShaderAction.h"

namespace PE {
using namespace Components;

void SetPerFrameConstantsShaderAction::bindToPipeline(Effect *pCurEffect /* = NULL*/)
{
#if PE_PLAT_IS_PSVITA
#	elif APIABSTRACTION_D3D11
	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

	Effect::setConstantBuffer(pDevice, pDeviceContext, s_pBuffer, 0, &m_data, sizeof(Data));
#	elif APIABSTRACTION_D3D9
	D3D9Renderer *pD3D9Renderer = static_cast<D3D9Renderer *>(m_pContext->getGPUScreen());
	LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;
		
	pDevice->SetVertexShaderConstantF(0, (const float *)(&m_data.gGameTimes[0]), 1);
	pDevice->SetPixelShaderConstantF(0, (const float *)(&m_data.gGameTimes[0]), 1);
#	endif
}

void SetPerFrameConstantsShaderAction::unbindFromPipeline(Components::Effect *pCurEffect/* = NULL*/)
{}

void SetPerFrameConstantsShaderAction::releaseData()
{}

}; // namespace PE

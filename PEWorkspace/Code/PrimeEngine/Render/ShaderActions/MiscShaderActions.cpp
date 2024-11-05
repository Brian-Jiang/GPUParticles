#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/ResourceBufferGPU.h"
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"

#include "PrimeEngine/Render/ShaderActions/SetInstanceControlConstantsShaderAction.h"
#include "PrimeEngine/Render/ShaderActions/SA_SetAndBind_ConstResource_PerInstanceData.h"
#include "PrimeEngine/Render/ShaderActions/SA_SetAndBind_ConstResource_SingleObjectAnimationPalette.h"
#include "PrimeEngine/Render/ShaderActions/SA_SetAndBind_ConstResource_InstancedObjectsAnimationPalettes.h"

// Sibling/Children includes
#include "MiscShaderActions.h"
namespace PE {
using namespace Components;
#if PE_PLAT_IS_PSVITA

void * SetPerFrameConstantsShaderAction::s_pBuffer = NULL;
void * SetPerObjectGroupConstantsShaderAction::s_pBuffer = NULL;
void * SetPerObjectConstantsShaderAction::s_pBuffer = NULL;
void * SetPerMaterialConstantsShaderAction::s_pBuffer = NULL;

#elif APIABSTRACTION_D3D11
ID3D11Buffer * SetPerFrameConstantsShaderAction::s_pBuffer = NULL;
ID3D11Buffer * SetPerObjectGroupConstantsShaderAction::s_pBuffer = NULL;
ID3D11Buffer * SetPerObjectConstantsShaderAction::s_pBuffer = NULL;
ID3D11Buffer * SA_SetAndBind_ConstResource_PerInstanceData::s_pBuffer = NULL;
ID3D11ShaderResourceView * SA_SetAndBind_ConstResource_PerInstanceData::s_pShaderResourceView = NULL;
ID3D11Buffer * SetPerMaterialConstantsShaderAction::s_pBuffer = NULL;

ID3D11Buffer * SA_SetAndBind_ConstResource_InstancedObjectsAnimationPalettes::s_pBuffer = NULL;
ID3D11ShaderResourceView *SA_SetAndBind_ConstResource_InstancedObjectsAnimationPalettes::s_pShaderResourceView = NULL;

ID3D11Buffer * SA_SetAndBind_ConstResource_SingleObjectAnimationPalette::s_pBufferSinglePalette = NULL;
ID3D11ShaderResourceView *SA_SetAndBind_ConstResource_SingleObjectAnimationPalette::s_pShaderResourceViewSinglePalette = NULL;

ID3D11Buffer *SetInstanceControlConstantsShaderAction::s_pBuffer = NULL;

#endif

// DX11-only shader actions


void hlslDX11_SetVertexBufferValueFromHandleAction::bindToPipeline(Effect *pCurEffect /* = NULL*/)
{
#	if APIABSTRACTION_D3D11

		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(pCurEffect->m_pContext->getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

		ID3D11Buffer *pTmpBuffer = 0;
		void *pMappedPtr = VertexBufferGPU::s_mapToPtr(pDevice, pDeviceContext, m_pBuffer, pTmpBuffer);
		void *src = m_data.getObject();
		memcpy(pMappedPtr, src, m_trueSize);
		VertexBufferGPU::s_releasePtrAndCopyBack(pDevice, pDeviceContext, m_pBuffer, pTmpBuffer);
#	else
		assert(!"Not Implemented");
#	endif
}

void hlslDX11_SetVertexBufferValueFromHandleAction::unbindFromPipeline(Components::Effect *pCurEffect/* = NULL*/)
{} // no action needed, other texture can override this gpu slot

void hlslDX11_SetVertexBufferValueFromHandleAction::releaseData()
{} // no data held


void hlslDX11_SetVertexBufferValueFromResourceBufferAction::bindToPipeline(Effect *pCurEffect /* = NULL*/)
{
#	if APIABSTRACTION_D3D11
		pSrcResourceBufferGPU->copyIntoVertexBuffer(m_pBuffer);
#	else
		assert(!"Not Implemented");
#	endif
}
 
void hlslDX11_SetVertexBufferValueFromResourceBufferAction::unbindFromPipeline(Components::Effect *pCurEffect /* = NULL*/)
{} // no action needed, other texture can override this gpu slot

void hlslDX11_SetVertexBufferValueFromResourceBufferAction::releaseData()
{} // no data held


void hlslDX11_SetResourceBufferValueFromResourceBufferAction::bindToPipeline(Effect *pCurEffect /* = NULL*/)
{
#	if APIABSTRACTION_D3D11
		pSrcResourceBufferGPU->copyIntoResourceBuffer(m_pBuffer);
#	else
		assert(!"Not Implemented");
#endif
}

void hlslDX11_SetResourceBufferValueFromResourceBufferAction::unbindFromPipeline(Components::Effect *pCurEffect /* = NULL*/)
{} // no action needed, other texture can override this gpu slot

void hlslDX11_SetResourceBufferValueFromResourceBufferAction::releaseData()
{} // no data held



}; // namespace PE

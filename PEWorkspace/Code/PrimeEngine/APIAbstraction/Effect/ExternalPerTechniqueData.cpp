
#include "ExternalPerTechniqueData.h"

#include "Effect.h"

#include "EffectManager.h"

#include "PrimeEngine/FileSystem/FileWriter.h"
#include "../../Lua/LuaEnvironment.h"
#include "../../Lua/EventGlue/EventDataCreators.h"
#include "PrimeEngine/Utils/ErrorHandling.h"

#include "PrimeEngine/Game/Common/GameContext.h"

#include "PrimeEngine/APIAbstraction/GPUBuffers/VertexBufferGPUManager.h"


namespace PE {

ExternalPerTechniqueData::ExternalPerTechniqueData(PE::GameContext &context, PE::MemoryArena arena)
{
	#if APIABSTRACTION_D3D11
		memset(m_apiVertexInputLayouts, 0, sizeof(m_apiVertexInputLayouts));	
	#endif
}

void ExternalPerTechniqueData::initVertexAttributeBindings(Components::Effect *pEffect)
{
#if APIABSTRACTION_IOS
	{
		m_glslAttribLocations.initialize(pEffect);
	}
#endif

}

void ExternalPerTechniqueData::init(Components::Effect *pEffect
#if APIABSTRACTION_D3D11
, ID3DBlob* pVertexShaderBuffer // loaded bytecode of the file
#endif
)
{
	#if APIABSTRACTION_D3D11
	
	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(pEffect->m_pContext->getGPUScreen());
	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;


	if (pVertexShaderBuffer)
	{
		PEVertexBufferInfo *layouts = VertexBufferGPUManager::Instance()->m_vertexBufferInfos;
		
		for (unsigned int i = 0; i < PEVertexFormatLayout_Count; ++i)
		{
			EPEVertexFormatLayout layout = (EPEVertexFormatLayout)i;
			//one format can be represented by several layouts
			EPEVertexFormat format = VertexBufferGPUManager::Instance()->m_layoutToFormatMap[layout];

			if (format == pEffect->m_vsVertexFormat)
			{
				PEVertexBufferInfo &vertexBufferStorageLayout = layouts[layout];
				Array<D3D11_INPUT_ELEMENT_DESC> &intermediateApiInfo = vertexBufferStorageLayout.m_intermediateApiInfo;
				
				ID3D11InputLayout *pLayout = NULL;
				HRESULT hr = pDevice->CreateInputLayout(
					intermediateApiInfo.getFirstPtr(), intermediateApiInfo.m_size,
					pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(), &pLayout);

				assert(SUCCEEDED(hr));
				m_apiVertexInputLayouts[layout] = pLayout; // store a pointer of something like StdMesh_B0__P0f3_TC0f2_N0f3 and StdMesh_B0__P0f3_B1__TC0f2_B2__N0f3
				PEINFO("Vertex input layout added: %s", toString(layout));
			}
		}
	}
	#endif

	#if APIABSTRACTION_OGL
	// need to store handles to shader constants
	// since we have no access to registers with cg
	{
		m_cbPerObjectGroup_c1_cgparameters.initialize(pEffect);
		m_cbPerObject_c2_cgparameters.initialize(pEffect);
		m_BindResourceToGPUPipelineAction_cgparameter.initialize(pEffect);
		mcbPerMaterial_c3_cgparameters.initialize(pEffect);
		
	}
	#endif
}

}; // namespace PE

#ifndef __PE_ExternalPerTechniqueData__
#define __PE_ExternalPerTechniqueData__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes

#include "PrimeEngine/Render/IRenderer.h"
#include "PrimeEngine/Math/Matrix4x4.h"
#include "../Threading/Threading.h"
#include "PrimeEngine/Utils/PEMap.h"
// Sibling/Children includes
#include "EffectEnums.h"

#include "PrimeEngine/APIAbstraction/GPUBuffers/BufferInfo.h"
#include "PrimeEngine/Render/ShaderActions/SetPerFrameConstantsShaderAction.h"
#include "PrimeEngine/Render/ShaderActions/SetPerObjectGroupConstantsShaderAction.h"
#include "PrimeEngine/Render/ShaderActions/SA_Bind_Resource.h"
#include "PrimeEngine/Render/ShaderActions/SetPerObjectConstantsShaderAction.h"
#include "PrimeEngine/Render/ShaderActions/SetPerMaterialConstantsShaderAction.h"

#include "GLSLAttributes.h"

#include "SpeShader.h"

namespace PE {
namespace Components {

struct Effect;
};
struct ExternalPerTechniqueData
{
	ExternalPerTechniqueData(PE::GameContext &context, PE::MemoryArena arena);

	void init(Components::Effect *pEffect
		#if APIABSTRACTION_D3D11
		, ID3DBlob* pVertexShaderBuffer // loaded bytecode of the file
		#endif
	);

	void initVertexAttributeBindings(Components::Effect *pEffect);
	

#if APIABSTRACTION_D3D11
	//store D3D11 Vertex Input Layout for all different VertexBufferInfos
	ID3D11InputLayout* m_apiVertexInputLayouts[PEVertexFormatLayout_Count]; // not memory optimal but fast lookup
#endif


#if APIABSTRACTION_OGL
	// need to store handles to shader constants
	// since we have no access to registers with cg
	SetPerObjectGroupConstantsShaderAction::PerEffectBindIds m_cbPerObjectGroup_c1_cgparameters;
	SetPerObjectConstantsShaderAction::PerEffectBindIds m_cbPerObject_c2_cgparameters;
	SA_Bind_Resource::PerEffectBindIds m_BindResourceToGPUPipelineAction_cgparameter;
	SetPerMaterialConstantsShaderAction::PerEffectBindIds mcbPerMaterial_c3_cgparameters;
	void initCGParameters(Components::Effect *pEffect);
#endif

#if APIABSTRACTION_IOS
	GLSLAttributeLocations m_glslAttribLocations;
#endif

};

}; // namespace PE

#endif

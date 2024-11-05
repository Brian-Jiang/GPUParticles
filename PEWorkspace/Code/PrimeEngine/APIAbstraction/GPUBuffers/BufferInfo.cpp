
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "BufferInfo.h"
#include "PrimeEngine/Render/D3D9Renderer.h"
#include "PrimeEngine/Utils/ErrorHandling.h"


const char *toString(EPEVertexFormat f)
{
	switch(f)
	{
	case PEVertexFormat_ColoredMinimalMesh: return "ColoredMinimalMesh";
	case PEVertexFormat_ReducedMesh: return "ReducedMesh";
	case PEVertexFormat_StdMesh: return "StdMesh";
	case PEVertexFormat_DetailedMesh: return "DetailedMesh";
	case PEVertexFormat_MinimalSkin: return "MinimalSkin";
	case PEVertexFormat_ReducedSkin: return "ReducedSkin";
	case PEVertexFormat_StdSkin: return "StdSkin";
	case PEVertexFormat_DetailedSkin: return "DetailedSkin";
	
	default: PEASSERT(false, "Unknown format. Have you added new one and not modify this function?"); return "";
	};
}

EPEVertexFormat getFormatFromVSName(const char *vsFilename)
{
	if (StringOps::startsswith(vsFilename, "ColoredMinimalMesh_"))
		return PEVertexFormat_ColoredMinimalMesh;
	if (StringOps::startsswith(vsFilename, "ReducedMesh_"))
		return PEVertexFormat_ReducedMesh;
	if (StringOps::startsswith(vsFilename, "StdMesh_"))
		return PEVertexFormat_StdMesh;
	if (StringOps::startsswith(vsFilename, "DetailedMesh_"))
		return PEVertexFormat_DetailedMesh;
	if (StringOps::startsswith(vsFilename, "DetailedSkin_"))
		return PEVertexFormat_DetailedSkin;

	return PEVertexFormat_Count;
}


const char *toString(EPEVertexFormatLayout v)
{
	switch(v)
	{
	case PEVertexFormatLayout_ColoredMinimalMesh0_B0__P0f3_C0f3: return "PEVertexFormatLayout_ColoredMinimalMesh0_B0__P0f3_C0f3";
	case PEVertexFormatLayout_ColoredMinimalMesh1_B0__P0f3_B1__C0f3: return "PEVertexFormatLayout_ColoredMinimalMesh1_B0__P0f3_B1__C0f3";
	case PEVertexFormatLayout_ReducedMesh_B0__P0f3_TC0f2: return "PEVertexFormatLayout_ReducedMesh_B0__P0f3_TC0f2";
	case PEVertexFormatLayout_StdMesh_B0__P0f3_TC0f2_N0f3: return "PEVertexFormatLayout_StdMesh_B0__P0f3_TC0f2_N0f3";
	case PEVertexFormatLayout_StdMesh_B0__P0f3_B1__TC0f2_B2__N0f3: return "PEVertexFormatLayout_StdMesh_B0__P0f3_B1__TC0f2_B2__N0f3";
	case PEVertexFormatLayout_DetailedMesh_B0__P0f3_TC0f2_N0f3_T0f3: return "PEVertexFormatLayout_DetailedMesh_B0__P0f3_TC0f2_N0f3_T0f3";
	case PEVertexFormatLayout_DetailedMesh_B0__P0f3_B1__TC0f2_B2__N0f3_B3__T0f3: return "PEVertexFormatLayout_DetailedMesh_B0__P0f3_B1__TC0f2_B2__N0f3_B3__T0f3";
	case PEVertexFormatLayout_MinimalSkin0_B0__P0f3_BW0f4_BW1f4_BI0f4_BI1f4: return "PEVertexFormatLayout_MinimalSkin0_B0__P0f3_BW0f4_BW1f4_BI0f4_BI1f4";
	case PEVertexFormatLayout_ReducedSkin0_B0__P0f3_BW0f4_BW1f4_BI0f4_BI1f4_TC0f2: return "PEVertexFormatLayout_ReducedSkin0_B0__P0f3_BW0f4_BW1f4_BI0f4_BI1f4_TC0f2";
	case PEVertexFormatLayout_StdSkin_B0__P0f3_BW0f4_BW1f4_BI0f4_BI1f4_TC0f2_N0f3: return "PEVertexFormatLayout_StdSkin_B0__P0f3_BW0f4_BW1f4_BI0f4_BI1f4_TC0f2_N0f3";
    case PEVertexFormatLayout_StdSkin_B0__P0f3_B1__TC0f2_B2__N0f3_B3__BW0f4_B4__BW1f4_B5__BI0f4_B6__BI1f4: return "PEVertexFormatLayout_StdSkin_B0__P0f3_B1__TC0f2_B2__N0f3_B3__BW0f4_B4__BW1f4_B5__BI0f4_B6__BI1f4";
	case PEVertexFormatLayout_DetailedSkin_B0__P0f3_BW0f4_BW1f4_BI0f4_BI1f4_TC0f2_N0f3_T0f3: return "PEVertexFormatLayout_DetailedSkin_B0__P0f3_BW0f4_BW1f4_BI0f4_BI1f4_TC0f2_N0f3_T0f3";
	#if !PE_PLAT_IS_PSVITA // vita supports up to 4 vertex streams
	case PEVertexFormatLayout_DetailedSkin_B0__P0f3_B1__TC0f2_B2__N0f3_B3__T0f3_B4__BW0f4_B5__BW1f4_B6__BI0f4_B7__BI1f4: return "PEVertexFormatLayout_DetailedSkin_B0__P0f3_B1__TC0f2_B2__N0f3_B3__T0f3_B4__BW0f4_B5__BW1f4_B6__BI0f4_B7__BI1f4";
	#endif
	default: PEASSERT(false, "Unknown layout. Have you added new one and not modify this function?"); return "";
	};
}


PEVertexAttributeInfo::PEVertexAttributeInfo()
	: m_semanticStr(NULL)
	, m_scalarType(PEScalarType_Undefined)
	, m_numScalars(0)
	, m_offset(0)
	, m_apiScalarType(0)
	, m_apiSemantic(0)
	, m_apiSemanticOrder(0)
{
	m_semantic = PESemanticType_Undefined;
	m_semanticOrder = 0;
	#if APIABSTRACTION_D3D9

	#elif APIABSTRACTION_OGL
		
	#endif
}

PEVertexAttributeInfo::PEVertexAttributeInfo(int offset, PEScalarType scalarType, int numScalars, PESemanticType semType, const char *semStr, int semOrder)
	: m_apiScalarType(0)
	, m_apiSemantic(0)
	, m_apiSemanticOrder(0)
{
	m_offset = offset;
	m_scalarType = scalarType;
	m_numScalars = numScalars;
	m_semantic = semType;
	m_semanticStr = semStr;
	m_semanticOrder = semOrder;
}

PEVertexAttributeBufferInfo::PEVertexAttributeBufferInfo()
{
	m_numAttributes = 0;
}
void PEVertexAttributeBufferInfo::setAPIValues()
{
	for (int i =0; i < m_numAttributes; i++)
	{
		m_attributeInfos[i].setAPIValues();
	}
}

void PEVertexAttributeInfo::setAPIValues()
{
	elementSemanticToApiSemantic(m_semantic, m_semanticOrder, m_apiSemantic, m_apiSemanticOrder);
#if APIABSTRACTION_OGL
	m_apiScalarType = elementScalarTypeToGLElementScalarType(m_scalarType);
#elif APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11
	m_apiScalarType = elementScalarTypeAndNumScalarsToD3DElementScalarType(m_scalarType, m_numScalars);
#elif PE_PLAT_IS_PSVITA
	m_apiScalarType = elementScalarTypeToPSVitaElementScalarType(m_scalarType, m_numScalars, m_numBytes);
#endif
	
}
#if APIABSTRACTION_OGL
GLint PEVertexAttributeInfo::elementScalarTypeToGLElementScalarType(PEScalarType peScalar)
{
	switch(peScalar)
	{
	case PEScalarType_Float: return GL_FLOAT;
	case PEScalarType_UInt16: return GL_UNSIGNED_SHORT;
	default: assert(!"This PEScalarType is not supported, make sure value is correct and if needed add support for this type"); break;
	};
	return 0;
}
#endif

#if PE_PLAT_IS_PSVITA
#endif

void PEVertexAttributeInfo::elementSemanticToApiSemantic(PESemanticType sem, int semOrder, int &out_sem, int &out_semOrder)
{
#if PE_PLAT_IS_PSVITA
	
#elif APIABSTRACTION_OGL
	switch(sem)
		{
		case PESemanticType_Position: 
			if (semOrder == 0) { out_sem = GL_VERTEX_ARRAY; out_semOrder = 0; return; }
			else { assert(!"This semantic order for this type is not supported. Make sure value is correct and add support if needed."); return; }
		case PESemanticType_Normal:
			if (semOrder == 0) { out_sem = GL_TEXTURE_COORD_ARRAY; out_semOrder = 2; return;}
			else { assert(!"This semantic order for this type is not supported. Make sure value is correct and add support if needed."); return;}
		case PESemanticType_TexCoord:
			if (semOrder == 0){ out_sem = GL_TEXTURE_COORD_ARRAY; out_semOrder = 0; return;}
			else { assert(!"This semantic order for this type is not supported. Make sure value is correct and add support if needed."); return;}
		case PESemanticType_Color:
			if (semOrder == 0) { out_sem = GL_COLOR_ARRAY; out_semOrder = 0; return;}
			else {assert(!"This semantic order for this type is not supported. Make sure value is correct and add support if needed.");return;}
		case PESemanticType_Tangent:
			if (semOrder == 0) { out_sem = GL_TEXTURE_COORD_ARRAY; out_semOrder = 1;return;}
			else { assert(!"This semantic order for this type is not supported. Make sure value is correct and add support if needed."); return;}
		case PESemanticType_JointWeights:
			if (semOrder == 0){out_sem = GL_TEXTURE_COORD_ARRAY;out_semOrder = 3;return;}
			else if (semOrder == 1){out_sem = GL_TEXTURE_COORD_ARRAY;out_semOrder = 4;return;}
			else{assert(!"This semantic order for this type is not supported. Make sure value is correct and add support if needed.");return;}
		case PESemanticType_JointIndices:
			if (semOrder == 0){out_sem = GL_TEXTURE_COORD_ARRAY;out_semOrder = 5;return;}
			else if (semOrder == 1){out_sem = GL_TEXTURE_COORD_ARRAY;out_semOrder = 6;return;}
			else{assert(!"This semantic order for this type is not supported. Make sure value is correct and add support if needed.");return;}
		default: assert(!"This PESemanticType is not supported, make sure value is correct and if needed add support for this type"); return;
		}; // switch
#elif APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11

	switch(sem)
	{
	case PESemanticType_Position: 
		if (semOrder == 0){out_sem = API_CHOOSE_DX11_DX9(0 /*dx11 tracks semantics by strings*/, D3DDECLUSAGE_POSITION);out_semOrder = 0;return;}
		else{assert(!"This semantic order for this type is not supported. Make sure value is correct and add support if needed.");return;}
	case PESemanticType_Normal:
		if (semOrder == 0){out_sem = API_CHOOSE_DX11_DX9(0 /*dx11 tracks semantics by strings*/, D3DDECLUSAGE_NORMAL);out_semOrder = 0;return;}
		else{assert(!"This semantic order for this type is not supported. Make sure value is correct and add support if needed.");return;}
	case PESemanticType_TexCoord:
		if (semOrder == 0){out_sem = API_CHOOSE_DX11_DX9(0 /*dx11 tracks semantics by strings*/, D3DDECLUSAGE_TEXCOORD);out_semOrder = 0;return;}
		else{assert(!"This semantic order for this type is not supported. Make sure value is correct and add support if needed.");return;}
	case PESemanticType_Color:
		if (semOrder == 0){out_sem = API_CHOOSE_DX11_DX9(0 /*dx11 tracks semantics by strings*/, D3DDECLUSAGE_COLOR);out_semOrder = 0;return;}
		else{assert(!"This semantic order for this type is not supported. Make sure value is correct and add support if needed.");return;}
	case PESemanticType_Tangent:
			if (semOrder == 0){out_sem = API_CHOOSE_DX11_DX9(0 /*dx11 tracks semantics by strings*/, D3DDECLUSAGE_TANGENT);out_semOrder = 0;return;}
			else{assert(!"This semantic order for this type is not supported. Make sure value is correct and add support if needed.");return;}
	case PESemanticType_JointWeights:
			if (semOrder == 0){out_sem = API_CHOOSE_DX11_DX9(0 /*dx11 tracks semantics by strings*/, D3DDECLUSAGE_BLENDWEIGHT);out_semOrder = 0;return;}
			else if (semOrder == 1){out_sem = API_CHOOSE_DX11_DX9(0 /*dx11 tracks semantics by strings*/, D3DDECLUSAGE_BLENDWEIGHT);out_semOrder = 1;return;}
			else{assert(!"This semantic order for this type is not supported. Make sure value is correct and add support if needed.");return;}
	case PESemanticType_JointIndices:
			if (semOrder == 0){out_sem = API_CHOOSE_DX11_DX9(0 /*dx11 tracks semantics by strings*/, D3DDECLUSAGE_BLENDINDICES);out_semOrder = 0;return;}
			else if (semOrder == 1){out_sem = API_CHOOSE_DX11_DX9(0 /*dx11 tracks semantics by strings*/, D3DDECLUSAGE_BLENDINDICES);out_semOrder = 1;return;}
			else{assert(!"This semantic order for this type is not supported. Make sure value is correct and add support if needed.");return;}
	default: assert(!"This PESemanticType is not supported, make sure value is correct and if needed add support for this type"); return;
	}; // switch
#endif

}


#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11
int PEVertexAttributeInfo::elementScalarTypeAndNumScalarsToD3DElementScalarType(PEScalarType peScalar, int numScalars)
{
	switch(peScalar)
	{
	case PEScalarType_Float:
		switch(numScalars)
		{
		case 1: return API_CHOOSE_DX11_DX9(DXGI_FORMAT_R32_FLOAT, D3DDECLTYPE_FLOAT1);
		case 2: return API_CHOOSE_DX11_DX9(DXGI_FORMAT_R32G32_FLOAT, D3DDECLTYPE_FLOAT2);
		case 3: return API_CHOOSE_DX11_DX9(DXGI_FORMAT_R32G32B32_FLOAT, D3DDECLTYPE_FLOAT3);
		case 4: return API_CHOOSE_DX11_DX9(DXGI_FORMAT_R32G32B32A32_FLOAT, D3DDECLTYPE_FLOAT4);
		default: assert(!"This number of PEScalarType is not supported, make sure value is correct and if needed add support for this number fo scalars"); break;
		};
		break;
	default:assert(!"This PEScalarType is not supported, make sure value is correct and if needed add support for this type"); break;
	}
	return 0;
}
#endif

PEVertexBufferInfo::PEVertexBufferInfo()
: m_bufferInfos( *((PE::GameContext*)(NULL)), PE::MemoryArena_Invalid)
, m_arena(PE::MemoryArena_Invalid)
, m_pContext(NULL)
#if APIABSTRACTION_D3D11
, m_intermediateApiInfo(*((PE::GameContext*)(NULL)), PE::MemoryArena_Invalid)
#elif PE_PLAT_IS_PSVITA
#endif
{
#if APIABSTRACTION_D3D9
	m_apiVertexDeclaration = NULL;
#endif
}

PEVertexBufferInfo::PEVertexBufferInfo(PE::GameContext &context, PE::MemoryArena arena, EPEVertexFormatLayout layout)
: m_bufferInfos(context, arena)
, m_arena(arena)
, m_pContext(&context)
, m_vertexFormatLayout(layout)
#if APIABSTRACTION_D3D11
, m_intermediateApiInfo(context, arena)
#elif PE_PLAT_IS_PSVITA
#endif
{
#if APIABSTRACTION_D3D9
	m_apiVertexDeclaration = NULL;
#endif
}
void PEVertexBufferInfo::setAPIValues()
{
	for (unsigned int ib = 0; ib < m_bufferInfos.m_size; ib++)
	{
		m_bufferInfos[ib].setAPIValues();
	}
#if APIABSTRACTION_D3D9
	// prepare d3d9vertexelement data structs

	//calculate total number of vertex elements
	int numVertexElements = 0;
	for (unsigned int ib = 0; ib < m_bufferInfos.m_size; ib++)
	{
		PEVertexAttributeBufferInfo &vb = m_bufferInfos[ib];
		numVertexElements += vb.m_numAttributes;
	}
	Array<D3DVERTEXELEMENT9> apiInfo(*m_pContext, m_arena);
	apiInfo.reset(numVertexElements+1); // last one will be empty, thats how d3d knows the end of list

	for (unsigned int ib = 0; ib < m_bufferInfos.m_size; ib++)
	{
		
		PEVertexAttributeBufferInfo &vb = m_bufferInfos[ib];
		for (int iElement = 0; iElement < vb.m_numAttributes; iElement++)
		{
			PEVertexAttributeInfo &elementInfo = vb.m_attributeInfos[iElement];

			D3DVERTEXELEMENT9 d3delement;
			memset(&d3delement, 0, sizeof(D3DVERTEXELEMENT9));

			d3delement.Stream = ib;
			d3delement.Offset = elementInfo.m_offset;
			d3delement.Type = elementInfo.m_apiScalarType;
			d3delement.Method = D3DDECLMETHOD_DEFAULT;
			d3delement.Usage = elementInfo.m_apiSemantic;
			d3delement.UsageIndex = elementInfo.m_apiSemanticOrder;

			apiInfo.add(d3delement);
		}
	}
	D3DVERTEXELEMENT9 d3delement = D3DDECL_END();
	apiInfo.add(d3delement);

	HRESULT hr;
	
	PE::D3D9Renderer *pD3D9Renderer = static_cast<PE::D3D9Renderer *>(m_pContext->getGPUScreen());
	LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;

	D3DVERTEXELEMENT9 *pElems = apiInfo.getFirstPtr();
	hr = pDevice->CreateVertexDeclaration(pElems, &m_apiVertexDeclaration);
	PEASSERT(SUCCEEDED(hr), "CreateVertexDeclaration failed");
	if (FAILED(hr))
		return;
	apiInfo.reset(0);
#elif APIABSTRACTION_D3D11

	// prepare D3D11_INPUT_ELEMENT_DESC data structs

	//calculate total number of vertex elements
	int numVertexElements = 0;
	for (unsigned int ib = 0; ib < m_bufferInfos.m_size; ib++)
	{
		PEVertexAttributeBufferInfo &vb = m_bufferInfos[ib];
		numVertexElements += vb.m_numAttributes;
	}
	m_intermediateApiInfo.reset(numVertexElements);

	for (unsigned int ib = 0; ib < m_bufferInfos.m_size; ib++)
	{
		
		PEVertexAttributeBufferInfo &vb = m_bufferInfos[ib];
		for (int iElement = 0; iElement < vb.m_numAttributes; iElement++)
		{
			PEVertexAttributeInfo &elementInfo = vb.m_attributeInfos[iElement];

			D3D11_INPUT_ELEMENT_DESC d3delement;
			memset(&d3delement, 0, sizeof(D3D11_INPUT_ELEMENT_DESC));

			d3delement.SemanticName = elementInfo.m_semanticStr;
			d3delement.SemanticIndex = elementInfo.m_apiSemanticOrder;
			d3delement.Format = (DXGI_FORMAT)(elementInfo.m_apiScalarType);
			d3delement.InputSlot = ib;
			d3delement.AlignedByteOffset = elementInfo.m_offset;
			d3delement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; // todo: will be different for instancing
			d3delement.InstanceDataStepRate = 0; // todo: will be different for instancing

			m_intermediateApiInfo.add(d3delement);
		}
	}

	// the info will be properly initialized per effect
	
#elif PE_PLAT_IS_PSVITA

#elif APIABSTRACTION_OGL

#endif
}

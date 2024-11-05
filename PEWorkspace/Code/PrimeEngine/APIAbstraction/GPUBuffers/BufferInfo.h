#ifndef __PYENGINE_2_0_SINGLE_BUFFER_GPU__
#define __PYENGINE_2_0_SINGLE_BUFFER_GPU__
// This class works with vertex buffers in GPU memory

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/Render/IRenderer.h"


#include "PrimeEngine/Render/D3D11Renderer.h"
#include "PrimeEngine/Render/GLRenderer.h"
#include "PrimeEngine/Render/D3D9Renderer.h"

#include "PrimeEngine/Utils/Array/Array.h"
// Sibling/Children includes

enum PEScalarType
{
	PEScalarType_Undefined,
	PEScalarType_Float,
	PEScalarType_UInt16
};

enum PESemanticType
{
	PESemanticType_Undefined,

	PESemanticType_Position,
	PESemanticType_TexCoord,
	PESemanticType_Normal,
	PESemanticType_Tangent,
	PESemanticType_JointWeights,
	PESemanticType_JointIndices,
	PESemanticType_Color,
};

enum PEPrimitveTopology
{
	PEPrimitveTopology_POINTS,
	PEPrimitveTopology_TRIANGLES,
	PEPrimitveTopology_QUADS,
	PEPrimitveTopology_LINES,
	PEPrimitveTopology_LINESTRIP,
};

enum EPEVertexFormat
{
	PEVertexFormat_ColoredMinimalMesh = 0,
	PEVertexFormat_ReducedMesh,
	PEVertexFormat_StdMesh,
	PEVertexFormat_DetailedMesh,
	PEVertexFormat_MinimalSkin,
	PEVertexFormat_ReducedSkin,
	PEVertexFormat_StdSkin,
	PEVertexFormat_DetailedSkin,
	PEVertexFormat_Count
};

const char *toString(EPEVertexFormat f);

EPEVertexFormat getFormatFromVSName(const char *tech);

enum EPEVertexFormatLayout
{
	PEVertexFormatLayout_ColoredMinimalMesh0_B0__P0f3_C0f3 = 0,
	PEVertexFormatLayout_ColoredMinimalMesh1_B0__P0f3_B1__C0f3,
	PEVertexFormatLayout_ReducedMesh_B0__P0f3_TC0f2,
	PEVertexFormatLayout_StdMesh_B0__P0f3_TC0f2_N0f3,
	PEVertexFormatLayout_StdMesh_B0__P0f3_B1__TC0f2_B2__N0f3,
	PEVertexFormatLayout_DetailedMesh_B0__P0f3_TC0f2_N0f3_T0f3,
	PEVertexFormatLayout_DetailedMesh_B0__P0f3_B1__TC0f2_B2__N0f3_B3__T0f3,
	PEVertexFormatLayout_MinimalSkin0_B0__P0f3_BW0f4_BW1f4_BI0f4_BI1f4,
	PEVertexFormatLayout_ReducedSkin0_B0__P0f3_BW0f4_BW1f4_BI0f4_BI1f4_TC0f2,
	PEVertexFormatLayout_StdSkin_B0__P0f3_BW0f4_BW1f4_BI0f4_BI1f4_TC0f2_N0f3,
    PEVertexFormatLayout_StdSkin_B0__P0f3_B1__TC0f2_B2__N0f3_B3__BW0f4_B4__BW1f4_B5__BI0f4_B6__BI1f4,
	PEVertexFormatLayout_DetailedSkin_B0__P0f3_BW0f4_BW1f4_BI0f4_BI1f4_TC0f2_N0f3_T0f3,
	#if !PE_PLAT_IS_PSVITA 
	PEVertexFormatLayout_DetailedSkin_B0__P0f3_B1__TC0f2_B2__N0f3_B3__T0f3_B4__BW0f4_B5__BW1f4_B6__BI0f4_B7__BI1f4,
	#endif
	PEVertexFormatLayout_Count
};

const char *toString(EPEVertexFormatLayout v);

struct PEVertexAttributeInfo
{
	PEVertexAttributeInfo();
	PEVertexAttributeInfo(int offset, PEScalarType scalarType, int numScalars, PESemanticType semType, const char *semStr, int semOrder);
	void setAPIValues();
#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11
	static int elementScalarTypeAndNumScalarsToD3DElementScalarType(PEScalarType peScalar, int numScalars);
#elif APIABSTRACTION_OGL
	static GLint elementScalarTypeToGLElementScalarType(PEScalarType peScalar);
#elif PE_PLAT_IS_PSVITA
#endif
	static void elementSemanticToApiSemantic(PESemanticType sem, int semOrder, int &out_sem, int &out_semOrder);

	const char *m_semanticStr;

	PEScalarType m_scalarType;
	int m_numScalars;
	/// where in buffer this element is
	int m_offset;
	/// how many bytes does this element take up
	int m_numBytes; 

	int m_apiScalarType;

	/// defines which semantic to use : NORMAL0 vs NORMAL1, TEXCOORD0 vs TEXCOORD1, etc
	int m_semanticOrder;
	PESemanticType m_semantic;

	int m_apiSemantic;
	int m_apiSemanticOrder;

};

/// stores single buffer information
/// the buffer is likely to be an array of floats. however it can store several vertex elements (semantics)
/// example 1 - buffer that stores positions = [p0x, p0y, p0z, p1x, ... pnz] , would have one semantic
/// example 2 - buffer that stores position and tex coord = [p0x,p0y,p0z, tc0x, tc0y, p1x...] would have two semantics (position - 3 floats offset 0, tex coord - 2 floats offset 3)
/// in DirectX this information is a list of D3DVERTEXELEMENT9
/// in GL we manually set this by calling different gl commands (glNormal.. glTexCoord..)
struct PEVertexAttributeBufferInfo
{
	PEVertexAttributeBufferInfo();
	void setAPIValues();

	static const int MAX_ATTRIBUTES = 16;
	PEVertexAttributeInfo m_attributeInfos[MAX_ATTRIBUTES];
	int m_numAttributes;
};

/// stores information about combination of buffers
/// usually a mesh will have only one of these that represents the whole mesh geometry
struct PEVertexBufferInfo
{
	PEVertexBufferInfo();
		
	PEVertexBufferInfo(PE::GameContext &context, PE::MemoryArena arena, EPEVertexFormatLayout layout);
	Array<PEVertexAttributeBufferInfo> m_bufferInfos;

	#if APIABSTRACTION_D3D9
		LPDIRECT3DVERTEXDECLARATION9 m_apiVertexDeclaration;
	#elif APIABSTRACTION_D3D11
		Array<D3D11_INPUT_ELEMENT_DESC> m_intermediateApiInfo; // used to feed into D3D11 runtime and get ID3D11InputLayout* per technique
	#elif PE_PLAT_IS_PSVITA
	#endif
	EPEVertexFormatLayout m_vertexFormatLayout;		
		
	/// after the struct is filled in, we build api specific data to pass to gfx driver
	/// using this function
	void setAPIValues();

	PE::MemoryArena m_arena;
	PE::GameContext *m_pContext;
};



#endif

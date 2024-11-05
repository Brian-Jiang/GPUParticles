#ifndef __PYENGINE_2_0_INDEX_BUFFER_GPU__
#define __PYENGINE_2_0_INDEX_BUFFER_GPU__
// This class works with index buffers in GPU memory

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/Render/IRenderer.h"
#include "BufferInfo.h"
#include "PrimeEngine/Utils/ErrorHandling.h"
#include "PrimeEngine/Geometry/IndexBufferCPU/IndexBufferCPU.h"
#if APIABSTRACTION_D3D9
#include "PrimeEngine/APIAbstraction/DirectX9/D3D9_GPUBuffers/D3D9_IndexBufferGPU.h"
#elif APIABSTRACTION_D3D11
#include "PrimeEngine/APIAbstraction/DirectX11/D3D11_GPUBuffers/D3D11_IndexBufferGPU.h"
#elif APIABSTRACTION_OGL
#include "PrimeEngine/APIAbstraction/OGL/OGL_GPUBuffers/OGL_IndexBufferGPU.h"
#elif PE_PLAT_IS_PSVITA

#endif

// Sibling/Children includes

namespace PE {

struct IndexBufferGPU : public PE::PEAllocatableAndDefragmentable
{
	IndexBufferGPU(PE::GameContext &context, PE::MemoryArena arena):m_indexRanges(context, arena), m_vertsPerFacePerRange(context, arena)
	{
		m_arena = arena; m_pContext = &context;
        m_dbgName[0] = '\0';
	}
	~IndexBufferGPU();

	void createGPUBuffer(IndexBufferCPU &ib);

	void releaseGPUBuffer();

	void setAsCurrent();

	void unbindFromPipeline()
	{
#		if APIABSTRACTION_D3D9
#		elif APIABSTRACTION_D3D11
#		elif APIABSTRACTION_OGL
#			if !OGL_USE_VERTEX_BUFFER_ARRAYS
				// index
#				if !APIABSTRACTION_PS3 && !APIABSTRACTION_IOS
					glDisableClientState(GL_INDEX_ARRAY);
#				endif
#			endif
#		endif
	}

	void draw(PrimitiveTypes::UInt32 rangeId, PrimitiveTypes::UInt32 boneSegmentId, PrimitiveTypes::UInt32 instanceCount, PrimitiveTypes::UInt32 instanceOffset);
	void draw(PrimitiveTypes::UInt32 rangeId, PrimitiveTypes::UInt32 instanceCount, PrimitiveTypes::UInt32 instanceOffset);
	void draw(PrimitiveTypes::UInt32 instanceCount, PrimitiveTypes::UInt32 instanceOffset);
	void drawIndirect(ID3D11Buffer* buffer, PrimitiveTypes::UInt32 bufferOffset);

	static int PEPrimitiveTopologyToApiEnum(PEPrimitveTopology top);

	void setApiValues();

	#if APIABSTRACTION_D3D9
		IDirect3DIndexBuffer9 *m_pBuf;
	#elif APIABSTRACTION_D3D11
		ID3D11Buffer *m_pBuf;
	#elif APIABSTRACTION_OGL
		GLuint m_buf;
	#elif PE_PLAT_IS_PSVITA
		SceUID m_basicIndiceUid;
		void *m_basicIndices;
	#endif

	// unlike vertex buffer, since all info about index buffers relates to particular index buffer
	// i.e. we cant share some common data between multiple index buffers
	// we store the buffer info right in the index buffer
	PrimitiveTypes::UInt32 m_length;
	Array<IndexRange> m_indexRanges;
	Array<PrimitiveTypes::UInt32> m_vertsPerFacePerRange;
	int m_minVertexIndex;
	int m_maxVertexIndex;
	PEPrimitveTopology m_primitiveTopology;
	PrimitiveTypes::Int32 m_verticesPerPolygon;

	int m_apiTopology;

    char m_dbgName[256];
	PE::MemoryArena m_arena; PE::GameContext *m_pContext;

};

}; // namespace PE

#endif

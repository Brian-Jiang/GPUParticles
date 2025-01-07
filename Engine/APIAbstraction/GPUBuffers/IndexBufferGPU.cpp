#define NOMINMAX

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "IndexBufferGPU.h"

namespace PE {

IndexBufferGPU::~IndexBufferGPU()
{
	releaseGPUBuffer();
	for (PrimitiveTypes::UInt32 iRange = 0; iRange < m_indexRanges.m_size; iRange++)
	{
		m_indexRanges[iRange].m_boneSegments.reset(0);
	}
	m_indexRanges.reset(0);
	m_vertsPerFacePerRange.reset(0);

}

void IndexBufferGPU::createGPUBuffer(IndexBufferCPU &ib)
{
    strcpy(&m_dbgName[0], &ib.m_dbgName[0]);
    
	m_primitiveTopology = ib.m_primitiveTopology;
	m_verticesPerPolygon = ib.m_verticesPerPolygon;
	m_minVertexIndex = ib.m_minVertexIndex;
	m_maxVertexIndex = ib.m_maxVertexIndex;

	m_indexRanges.reset(ib.m_indexRanges.m_size);
	m_vertsPerFacePerRange.reset(ib.m_indexRanges.m_size);
	for (PrimitiveTypes::UInt32 iRange = 0; iRange < ib.m_indexRanges.m_size; iRange++)
	{
		m_indexRanges.add(ib.m_indexRanges[iRange]);
		m_vertsPerFacePerRange.add(ib.m_vertsPerFacePerRange[iRange]);
	}

	#if PE_PLAT_IS_PSVITA
	#elif APIABSTRACTION_D3D9
		D3D9Renderer *pD3D9Renderer = static_cast<D3D9Renderer *>(m_pContext->getGPUScreen());
		LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;

		m_pBuf = D3D9_IndexBufferGPU::CreateIndexBufferInGPU(pDevice,
			ib.getStartAddress(), ib.getByteSize());
		m_length = ib.getByteSize() / sizeof(PrimitiveTypes::UInt16);
	#elif APIABSTRACTION_D3D11
		
		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

		// Note that we use an abstract gpu handle to avoid a bunch of #if ..#endif
		m_pBuf = D3D11_IndexBufferGPU::CreateIndexBufferInGPU(pDevice,
			ib.getStartAddress(), ib.getByteSize());
		m_length = ib.getByteSize() / sizeof(PrimitiveTypes::UInt16);
	#elif APIABSTRACTION_OGL
		m_buf = OGL_IndexBufferGPU::CreateIndexBufferInGPU(ib.getStartAddress(), ib.getByteSize());
		m_length = ib.getByteSize() / sizeof(PrimitiveTypes::UInt16);
	#endif

	setApiValues();
}

void IndexBufferGPU::releaseGPUBuffer()
{
	#if PE_PLAT_IS_PSVITA
		graphicsFree(m_basicIndiceUid);
	#elif APIABSTRACTION_OGL
		if (!m_buf)
			return;

		OGL_IndexBufferGPU::releaseGPUBuffer(m_buf);
		m_buf = 0;
	#else
		if (!m_pBuf)
			return;
		m_pBuf->Release();
		m_pBuf = 0;
	#endif
}



void IndexBufferGPU::setAsCurrent()
{
	#if PE_PLAT_IS_PSVITA
	#elif APIABSTRACTION_D3D9
		D3D9Renderer *pD3D9Renderer = static_cast<D3D9Renderer *>(m_pContext->getGPUScreen());
		LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;

		D3D9_IndexBufferGPU::setAsCurrent(pDevice, m_pBuf, 2, 0);
	#elif APIABSTRACTION_D3D11
		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;
#if DEBUG_DRAW_CALLS
		PEINFO("IndexBufferGPU::setAsCurrent(): dbgName %s\n", m_dbgName);
#endif	
		D3D11_IndexBufferGPU::setAsCurrent(pDeviceContext, m_pBuf, 2, 0);
	#elif APIABSTRACTION_OGL
		#if !OGL_USE_VERTEX_BUFFER_ARRAYS
			// index
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buf);
			#if !APIABSTRACTION_PS3 && !APIABSTRACTION_IOS
				glEnableClientState(GL_INDEX_ARRAY);
			#endif
		#else
			// we dont need this since we are using VAOs for win32 ogl
		#endif
	#endif
}

void IndexBufferGPU::draw(PrimitiveTypes::UInt32 rangeId, PrimitiveTypes::UInt32 boneSegmentId, PrimitiveTypes::UInt32 instanceCount, PrimitiveTypes::UInt32 instanceOffset)
{
	IndexRange &r = m_indexRanges[rangeId];
	PrimitiveTypes::UInt32 start, end;
	start = r.m_boneSegments[boneSegmentId].m_start;
	end = r.m_boneSegments[boneSegmentId].m_end;
	PEDebugAssert(end - start + 1 > 0, "Trying to draw empty index buffer. count = %d", end - start + 1);

	#if PE_PLAT_IS_PSVITA
	#elif APIABSTRACTION_D3D9
		D3D9Renderer *pD3D9Renderer = static_cast<D3D9Renderer *>(m_pContext->getGPUScreen());
		LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;

		pDevice->DrawIndexedPrimitive(
			(D3DPRIMITIVETYPE)(m_apiTopology),
			// todo: optimize to see if we use less vertices
			0, r.m_minVertIndex, r.m_maxVertIndex - r.m_minVertIndex + 1, start, (end - start + 1) / m_verticesPerPolygon);
	#elif APIABSTRACTION_D3D11

		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;
#if DEBUG_DRAW_CALLS
		PEINFO("IndexBufferGPU::draw(): range[%d] bone segment[%d] \n", rangeId, boneSegmentId);
		PEINFO("IndexBufferGPU::draw(): DrawIndexedInstanced() #indexes: %d x #instances: %d starting at %d/%d \n", r.m_end - r.m_start + 1, instanceCount, r.m_start, m_length);
#endif		
		//todo: call this function only if different topology
		pDeviceContext->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)(m_apiTopology));
		//pDeviceContext->DrawIndexed(end - start + 1, start, 0);
		pDeviceContext->DrawIndexedInstanced(end - start + 1, instanceCount, start, 0, 0);
	#elif APIABSTRACTION_OGL
		int count = (end - start + 1);
		#if !APIABSTRACTION_IOS
			glDrawRangeElements(m_apiTopology, r.m_minVertIndex, r.m_maxVertIndex, count, GL_UNSIGNED_SHORT, (char*)NULL + start * sizeof(unsigned short) );
		#else
            glDrawElements(m_apiTopology, count, GL_UNSIGNED_SHORT, (char*)NULL + start * sizeof(unsigned short) );
		#endif
	#endif
}

void IndexBufferGPU::draw(PrimitiveTypes::UInt32 rangeId, PrimitiveTypes::UInt32 instanceCount, PrimitiveTypes::UInt32 instanceOffset)
{
	IndexRange &r = m_indexRanges[rangeId];
	PrimitiveTypes::UInt32 start, end;
	start = r.m_start;
	end = r.m_end;
	PEDebugAssert(end - start + 1 > 0, "Trying to draw empty index buffer. count = %d", end - start + 1);

	#if PE_PLAT_IS_PSVITA
	#elif APIABSTRACTION_D3D9
		D3D9Renderer *pD3D9Renderer = static_cast<D3D9Renderer *>(m_pContext->getGPUScreen());
		LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;

		bool suc = SUCCEEDED(pDevice->DrawIndexedPrimitive(
			(D3DPRIMITIVETYPE)(m_apiTopology),
			0, r.m_minVertIndex, r.m_maxVertIndex - r.m_minVertIndex + 1, start, (end - start + 1) / m_verticesPerPolygon));
		assert(suc);
	#elif APIABSTRACTION_D3D11
		
		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;
#if DEBUG_DRAW_CALLS
		PEINFO("IndexBufferGPU::draw(): range[%d] \n", rangeId);
		PEINFO("IndexBufferGPU::draw(): DrawIndexed()  #indexes: %d x #instances starting at %d/%d \n", r.m_end - r.m_start + 1, instanceCount, r.m_start, m_length);
#endif
		//todo: call this function only if different topology
		pDeviceContext->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY )(m_apiTopology));
		//pDeviceContext->DrawIndexed(r.m_end - r.m_start + 1, r.m_start, 0);
		pDeviceContext->DrawIndexedInstanced(r.m_end - r.m_start + 1, instanceCount, r.m_start, 0, 0);
	#elif APIABSTRACTION_OGL
		int count = (r.m_end - r.m_start + 1);
		#if !APIABSTRACTION_IOS
			glDrawRangeElements(m_apiTopology, r.m_minVertIndex, r.m_maxVertIndex, count, GL_UNSIGNED_SHORT, (char*)NULL + r.m_start * sizeof(unsigned short) );
		#else
			glDrawElements(m_apiTopology, count, GL_UNSIGNED_SHORT, (char*)NULL + r.m_start * sizeof(unsigned short) );
		#endif
	#endif
}

void IndexBufferGPU::draw(PrimitiveTypes::UInt32 instanceCount, PrimitiveTypes::UInt32 instanceOffset)
{
	#if PE_PLAT_IS_PSVITA
	#elif APIABSTRACTION_D3D9
		D3D9Renderer *pD3D9Renderer = static_cast<D3D9Renderer *>(m_pContext->getGPUScreen());
		LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;

		pDevice->DrawIndexedPrimitive(
			(D3DPRIMITIVETYPE)(m_apiTopology),
			0, m_minVertexIndex, m_maxVertexIndex-m_minVertexIndex+1, 0, m_length / m_verticesPerPolygon);
	#elif APIABSTRACTION_D3D11
		
		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;
#if DEBUG_DRAW_CALLS
		PEINFO("IndexBufferGPU::draw(): DrawIndexedInstanced() #indexes: %d x #instances: %d\n", m_length, instanceCount);
#endif
		//todo: call this function only if different topology
		pDeviceContext->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)(m_apiTopology));
		//pDeviceContext->DrawIndexed(m_length, 0, 0);
		pDeviceContext->DrawIndexedInstanced(m_length, instanceCount, 0, 0, 0);
	#elif APIABSTRACTION_OGL
		int count = m_length;
		#if !APIABSTRACTION_IOS
			glDrawRangeElements(m_apiTopology, m_minVertexIndex, m_maxVertexIndex, count, GL_UNSIGNED_SHORT, (char*)NULL + 0 * sizeof(unsigned short) );
		#else
			glDrawElements(m_apiTopology, count, GL_UNSIGNED_SHORT, (char*)NULL + 0 * sizeof(unsigned short) );
		#endif
	#endif
}

void IndexBufferGPU::drawIndirect(ID3D11Buffer* buffer, PrimitiveTypes::UInt32 bufferOffset) {
#if APIABSTRACTION_D3D11
	D3D11Renderer* pD3D11Renderer = static_cast<D3D11Renderer*>(m_pContext->getGPUScreen());
	ID3D11Device* pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext* pDeviceContext = pD3D11Renderer->m_pD3DContext;

	pDeviceContext->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)(m_apiTopology));
    pDeviceContext->DrawIndexedInstancedIndirect(buffer, bufferOffset);
#endif
}

int IndexBufferGPU::PEPrimitiveTopologyToApiEnum(PEPrimitveTopology top)
{
	#if APIABSTRACTION_D3D9
		switch (top)
		{
		case PEPrimitveTopology_LINES: return D3DPT_LINELIST;
		case PEPrimitveTopology_LINESTRIP: return D3DPT_LINESTRIP;
		case PEPrimitveTopology_POINTS: return D3DPT_POINTLIST;
		case PEPrimitveTopology_TRIANGLES: return D3DPT_TRIANGLELIST;
		default: assert(!"PEPrimitiveTopologyToApiEnum: unsupported topology. check value and add support if needed");
		};
	#elif APIABSTRACTION_OGL
		switch (top)
		{
		case PEPrimitveTopology_LINES: return GL_LINES;
		case PEPrimitveTopology_LINESTRIP: return GL_LINE_STRIP;
		case PEPrimitveTopology_POINTS: return GL_POINTS;
		case PEPrimitveTopology_TRIANGLES: return GL_TRIANGLES;
		default: assert(!"PEPrimitiveTopologyToApiEnum: unsupported topology. check value and add support if needed");
		};
	#elif APIABSTRACTION_D3D11
		switch (top)
		{
		case PEPrimitveTopology_LINES: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		case PEPrimitveTopology_LINESTRIP: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case PEPrimitveTopology_POINTS: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		case PEPrimitveTopology_TRIANGLES: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		default: assert(!"PEPrimitiveTopologyToApiEnum: unsupported topology. check value and add support if needed");
		};
	#elif PE_PLAT_IS_PSVITA
	#endif

	assert(!"PEPrimitiveTopologyToApiEnum: unsupported topology. check value and add support if needed");
	return -1;
}

void IndexBufferGPU::setApiValues()
{
	m_apiTopology = PEPrimitiveTopologyToApiEnum(m_primitiveTopology);
}

}; // namespace PE

#define		NOMINMAX

#include "VertexBufferGPU.h"

#include "PrimeEngine/APIAbstraction/Effect/Effect.h"
#include "VertexBufferGPUManager.h"
namespace PE {

VertexBufferGPU::VertexBufferGPU(PE::GameContext &context, PE::MemoryArena arena)
{
	m_isStreamOutput = false;
	m_weight = 1.0f;
	m_pBufferSetInfo = NULL;
	m_arena = arena; m_pContext = &context;
}
VertexBufferGPU::~VertexBufferGPU()
{
	releaseGPUBuffer();
}

void VertexBufferGPU::internalCreateGPUBufferFromCombined(PositionBufferCPU &vb, PrimitiveTypes::UInt32 vertexSize, WRITE_MODES writeMode/* = CONTANT*/)
{
	#if PE_PLAT_IS_PSVITA

	#elif APIABSTRACTION_D3D9
		D3D9Renderer *pD3D9Renderer = static_cast<D3D9Renderer *>(m_pContext->getGPUScreen());
		LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;

		m_pBuf = D3D9_VertexBufferGPU::CreateVertexBufferInGPUFromVb(
			pDevice, vb.getStartAddress(), vb.getByteSize());
		m_vertexSize = vertexSize;
		m_length = vb.getByteSize() / vertexSize;
	#elif APIABSTRACTION_D3D11
		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

		// Note that we use an abstract gpu handle to avoid a bunch of #if ..#endif
		if (writeMode == CONSTANT)
			m_pBuf = D3D11_VertexBufferGPU::CreateVertexBufferInGPUFromVb(
			pDevice, vb.getStartAddress(), vb.getByteSize());
		else if (writeMode == WRITABLE_BY_API)
			m_pBuf = D3D11_VertexBufferGPU::CreateVertexBufferInGPUFromVb(
			pDevice, vb.getStartAddress(), vb.getByteSize(),
			false, false); // not constant, not stream output
		else if (writeMode == STREAM_OUTPUT)
			m_pBuf = D3D11_VertexBufferGPU::CreateVertexBufferInGPUFromVb(
			pDevice, vb.getStartAddress(), vb.getByteSize(),
			false, true); // not constant, stream output

		m_vertexSize = vertexSize;
		m_length = vb.getByteSize() / vertexSize;
	#endif
}


void VertexBufferGPU::createGPUBufferFromSize(PrimitiveTypes::UInt32 stride, PrimitiveTypes::UInt32 numStrides, WRITE_MODES writeMode /* = CONSTANT*/)
{
	#if PE_PLAT_IS_PSVITA
	#elif APIABSTRACTION_D3D9
	D3D9Renderer *pD3D9Renderer = static_cast<D3D9Renderer *>(m_pContext->getGPUScreen());
	LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;

	m_pBuf = D3D9_VertexBufferGPU::CreateVertexBufferInGPUFromVb(
		pDevice, 0, stride * numStrides);
	m_vertexSize = stride;
	m_length = numStrides;
#		elif APIABSTRACTION_D3D11
	// will use D3D11_VertexBufferGPU
	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

	// Note that we use an abstract gpu handle to avoid a bunch of #if ..#endif
	if (writeMode == CONSTANT)
		m_pBuf = D3D11_VertexBufferGPU::CreateVertexBufferInGPUFromVb(
		pDevice, 0, stride * numStrides);
	else if (writeMode == WRITABLE_BY_API)
		m_pBuf = D3D11_VertexBufferGPU::CreateVertexBufferInGPUFromVb(
		pDevice, 0, stride * numStrides, false, false); // not constant, not stream output
	else if (writeMode == STREAM_OUTPUT)
		m_pBuf =  D3D11_VertexBufferGPU::CreateVertexBufferInGPUFromVb(
		pDevice, 0, stride * numStrides,
		false, true);; // not constant, stream output

	m_vertexSize = stride;
	m_length = numStrides;
#		endif
	setAPIValues();
}

void VertexBufferGPU::createStreamOutputGPUBuffer(PrimitiveTypes::UInt32 maxSize, PrimitiveTypes::UInt32 stride)
{
	m_isStreamOutput = true;
	#if APIABSTRACTION_D3D11
		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

		// Note that we use an abstract gpu handle to avoid a bunch of #if ..#endif
		m_pBuf = D3D11_VertexBufferGPU::CreateVertexBufferInGPUFromVb(
			pDevice, NULL, maxSize, false);
		m_vertexSize = stride;
		m_length = 0;
	#else
		PEASSERT(false, "Not supported!");
	#endif
	setAPIValues();
}

void *VertexBufferGPU::mapToPtr()
{
#		if APIABSTRACTION_D3D9 | APIABSTRACTION_OGL | APIABSTRACTION_D3D11 | PE_PLAT_IS_PSVITA
		
		assert(!"Not Implemented!");
		return 0;
#		elif APIABSTRACTION_D3D11
		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

		m_pTmpBuffer = NULL;

		D3D11_BUFFER_DESC desc;
		memset( &desc, 0, sizeof(desc) );
		pBuf->GetDesc( &desc );
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = 0;
		desc.MiscFlags = 0;
		pDevice->CreateBuffer(&desc, NULL, &m_pTmpBuffer);

		pDeviceContext->CopyResource( m_pTmpBuffer, m_pBuf );

		D3D11_MAPPED_SUBRESOURCE MappedResource; 
		pDeviceContext->Map( m_pTmpBuffer, 0, D3D11_MAP_READ, 0, &MappedResource );
		return MappedResource.pData;

		//return m_resourceBufferLowLevel.mapToPtr(pDevice, pDeviceContext);

#		endif

}

void VertexBufferGPU::releasePtrAndCopyBack()
{
#		if APIABSTRACTION_D3D9
		
		assert(!"Not Implemented!");
#		elif APIABSTRACTION_D3D11
		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

		pDeviceContext->Unmap( m_pTmpBuffer, 0 );

		pDeviceContext->CopyResource( m_pBuf, m_pTmpBuffer); // copy values

		m_pTmpBuffer->Release();

		//return m_resourceBufferLowLevel.releasePtr(pDeviceContext);
#else
	PEASSERT(false, "Not supported!");
#		endif

}
#if APIABSTRACTION_D3D11
void *VertexBufferGPU::s_mapToPtr(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContext, ID3D11Buffer *pSrcBuf, ID3D11Buffer * &tmpBuf)
{
#		if APIABSTRACTION_D3D9
		
		// retrieve device from IRenderer
		ID3D10Device *pDevice = IRenderer::Instance()->m_device;
		assert(!"Not Implemented!");
#		elif APIABSTRACTION_D3D11
		ID3D11Buffer *pBuf = pSrcBuf;
		
		tmpBuf = NULL;

		D3D11_BUFFER_DESC desc;
		memset( &desc, 0, sizeof(desc) );
		pBuf->GetDesc( &desc );
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = 0;
		desc.MiscFlags = 0;
		pDevice->CreateBuffer(&desc, NULL, &tmpBuf);

		pDeviceContext->CopyResource( tmpBuf, pBuf );

		D3D11_MAPPED_SUBRESOURCE MappedResource; 
		pDeviceContext->Map( tmpBuf, 0, D3D11_MAP_READ, 0, &MappedResource );
		return MappedResource.pData;
		//return m_resourceBufferLowLevel.mapToPtr(pDevice, pDeviceContext);

#		endif

}

void VertexBufferGPU::s_releasePtrAndCopyBack(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContext, ID3D11Buffer *pSrcBuf, ID3D11Buffer * tmpBuf)
{
#		if APIABSTRACTION_D3D9
		
		// retrieve device from IRenderer
		ID3D10Device *pDevice = IRenderer::Instance()->m_device;
		assert(!"Not Implemented!");
#		elif APIABSTRACTION_D3D11
		pDeviceContext->Unmap( tmpBuf, 0 );

		ID3D11Buffer *pBuf = pSrcBuf;
		
		pDeviceContext->CopyResource( pBuf, tmpBuf); // copy values

		tmpBuf->Release();

		//return m_resourceBufferLowLevel.releasePtr(pDeviceContext);
#		endif

}
#endif

void VertexBufferGPU::setAsCurrent(PE::GameContext &context, PE::MemoryArena arena, Array<Handle> &hBuffersGPU)
{
#		if APIABSTRACTION_D3D9
		Array<IDirect3DVertexBuffer9 *> pBufs(context, arena, hBuffersGPU.m_size);
		Array<PrimitiveTypes::UInt32> strides(context, arena, hBuffersGPU.m_size);
		Array<PrimitiveTypes::UInt32> offsets(context, arena, hBuffersGPU.m_size);
		//PEINFO(L"Setting buffers\n");
		for (PrimitiveTypes::UInt32 ib = 0; ib < hBuffersGPU.m_size; ib++)
		{
			VertexBufferGPU *pBufGPU = hBuffersGPU[ib].getObject<VertexBufferGPU>();
			IDirect3DVertexBuffer9 *pBuf = pBufGPU->m_pBuf;
			pBufs.add(pBuf);
			strides.add(pBufGPU->m_vertexSize);
			offsets.add(0);
		}

		D3D9Renderer *pD3D9Renderer = static_cast<D3D9Renderer *>(context.getGPUScreen());
		LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;

		D3D9_VertexBufferGPU::setAsCurrent(pDevice, pBufs, strides, offsets);

		pBufs.reset(0);
		strides.reset(0);
		offsets.reset(0);
#		elif APIABSTRACTION_D3D11
	
		Array<ID3D11Buffer *> pBufs(context, arena, hBuffersGPU.m_size);
		Array<PrimitiveTypes::UInt32> strides(context, arena, hBuffersGPU.m_size);
		Array<PrimitiveTypes::UInt32> offsets(context, arena, hBuffersGPU.m_size);
		for (PrimitiveTypes::UInt32 ib = 0; ib < hBuffersGPU.m_size; ib++)
		{
			VertexBufferGPU *pBufGPU = hBuffersGPU[ib].getObject<VertexBufferGPU>();
			ID3D11Buffer *pBuf = pBufGPU->m_pBuf;
			pBufs.add(pBuf);
			strides.add(pBufGPU->m_vertexSize);
			offsets.add(0);
		}

		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(context.getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

		D3D11_VertexBufferGPU::setAsCurrent(pDeviceContext, pBufs, strides, offsets);

		pBufs.reset(0);
		strides.reset(0);
		offsets.reset(0);
#else
		PEASSERT(false, "Not Supported!");
#		endif
}
void VertexBufferGPU::createGPUBufferFromSource_ColoredMinimalMesh(PositionBufferCPU &vb, ColorBufferCPU &cb, WRITE_MODES writeMode /* = CONSTANT*/)
{

	//OGL is still using old way of forwarding all source buffers to low level helper implementation
#if APIABSTRACTION_OGL
	memset(m_bufs, 0, sizeof(m_bufs));
	m_buf = OGL_VertexBufferGPU::CreateVertexBufferInGPUFromVbNbTb(
		*m_pContext, m_arena,
		&vb, &cb, NULL, NULL, NULL, NULL, true, &m_bufs[0]);
	m_vertexSize = sizeof(PrimitiveTypes::Float32) * 6; 
	m_length = vb.getByteSize() / m_vertexSize;

	m_pBufferSetInfo = &VertexBufferGPUManager::Instance()->m_vertexBufferInfos[PEVertexFormatLayout_ColoredMinimalMesh1_B0__P0f3_B1__C0f3];
#else

	PositionBufferCPU combined(*m_pContext, m_arena);
	combined.m_values.reset((3 + 3) * (vb.m_values.m_size / 3));
	PrimitiveTypes::UInt32 ivbval = 0;
	PrimitiveTypes::UInt32 icbval = 0;

	for (PrimitiveTypes::UInt32 iv = 0; iv < vb.m_values.m_size / 3; iv++)
	{
		PrimitiveTypes::Float32 x, y, z, r, g, b;
		x = vb.m_values[ivbval++];
		y = vb.m_values[ivbval++];
		z = vb.m_values[ivbval++];

		r = cb.m_values[icbval++];
		g = cb.m_values[icbval++];
		b = cb.m_values[icbval++];

		combined.m_values.add(x, y, z);
		combined.m_values.add(r, g, b);
	}

	internalCreateGPUBufferFromCombined(combined, sizeof(PrimitiveTypes::Float32) * 6, writeMode);

	combined.m_values.reset(0);

	m_pBufferSetInfo = &VertexBufferGPUManager::Instance()->m_vertexBufferInfos[PEVertexFormatLayout_ColoredMinimalMesh0_B0__P0f3_C0f3];

#endif
	setAPIValues();
}
void VertexBufferGPU::createGPUBufferFromSource_ReducedMesh(PositionBufferCPU &vb, TexCoordBufferCPU &tcb, WRITE_MODES writeMode)
{
	PositionBufferCPU combined(*m_pContext, m_arena);
	combined.m_values.reset((3 + 2) * (vb.m_values.m_size / 3));
	PrimitiveTypes::UInt32 ivbval = 0;
	PrimitiveTypes::UInt32 itcbval = 0;

	for (PrimitiveTypes::UInt32 iv = 0; iv < vb.m_values.m_size / 3; iv++)
	{
		PrimitiveTypes::Float32 x, y, z, u, v;
		x = vb.m_values[ivbval++];
		y = vb.m_values[ivbval++];
		z = vb.m_values[ivbval++];

		u = tcb.m_values[itcbval++];
		v = tcb.m_values[itcbval++];
		
		combined.m_values.add(x, y, z);
		combined.m_values.add(u);
		combined.m_values.add(v);
	}

	internalCreateGPUBufferFromCombined(combined, sizeof(PrimitiveTypes::Float32) * 5, writeMode);
	
	combined.m_values.reset(0);

	m_pBufferSetInfo = &VertexBufferGPUManager::Instance()->m_vertexBufferInfos[PEVertexFormatLayout_ReducedMesh_B0__P0f3_TC0f2];

	setAPIValues();
}

void VertexBufferGPU::createGPUBuffer(PositionBufferCPU &vb, TexCoordBufferCPU &tcb,  TexCoordBufferCPU &tcb1)
{
	PositionBufferCPU combined(*m_pContext, m_arena);
	combined.m_values.reset((3 + 2 + 2) * (vb.m_values.m_size / 3));
	PrimitiveTypes::UInt32 ivbval = 0;
	PrimitiveTypes::UInt32 itcbval = 0;
	PrimitiveTypes::UInt32 itcbval1 = 0;

	for (PrimitiveTypes::UInt32 iv = 0; iv < vb.m_values.m_size / 3; iv++)
	{
		PrimitiveTypes::Float32 x, y, z, u, v, u1, v1;
		x = vb.m_values[ivbval++];
		y = vb.m_values[ivbval++];
		z = vb.m_values[ivbval++];

		u = tcb.m_values[itcbval++];
		v = tcb.m_values[itcbval++];

		u1 = tcb1.m_values[itcbval1++];
		v1 = tcb1.m_values[itcbval1++];


		combined.m_values.add(x, y, z);
		combined.m_values.add(u); combined.m_values.add(v);
		combined.m_values.add(u1); combined.m_values.add(v1);
	}

	internalCreateGPUBufferFromCombined(combined, sizeof(PrimitiveTypes::Float32) * 7);
	combined.m_values.reset(0);
	setAPIValues();
}

void VertexBufferGPU::createGPUBufferFromSource_MinimalSkin(PositionBufferCPU &vb, SkinWeightsCPU &weights)
{
	PEASSERT(false, "Need to implement for DEFAULT_SKIN_WEIGHTS_PER_VERTEX. seee detailed skin");
	
	PositionBufferCPU res(*m_pContext, m_arena);
	res.m_values.reset((vb.m_values.m_size / 3) * (3 + 4 + 4));

	for (PrimitiveTypes::UInt32 iv = 0; iv < vb.m_values.m_size / 3; iv++)
	{
		PrimitiveTypes::Float32 x,y,z;
		PrimitiveTypes::UInt32 curInex = iv * 3;
		x = vb.m_values[curInex]; y = vb.m_values[curInex + 1]; z = vb.m_values[curInex + 2];

		PrimitiveTypes::Float32 vweights[4] = {-1.0f, -1.0f, -1.0f, -1.0f};
		PrimitiveTypes::Float32 jointIndices[4] = {-1.0f, -1.0f, -1.0f, -1.0f};
		assert(weights.m_weightsPerVertex[iv].m_size <= 4);
		for (PrimitiveTypes::UInt32 ivw = 0; ivw < weights.m_weightsPerVertex[iv].m_size; ivw++)
		{
			vweights[ivw] = (PrimitiveTypes::Float32)(weights.m_weightsPerVertex[iv][ivw].m_weight);
			jointIndices[ivw] = (PrimitiveTypes::Float32)(weights.m_weightsPerVertex[iv][ivw].m_jointIndex);
		}

		res.m_values.add(x); res.m_values.add(y); res.m_values.add(z);
		res.m_values.add(vweights[0]); res.m_values.add(vweights[1]); res.m_values.add(vweights[2]); res.m_values.add(vweights[3]);
		res.m_values.add(jointIndices[0]); res.m_values.add(jointIndices[1]); res.m_values.add(jointIndices[2]); res.m_values.add(jointIndices[3]);
	}

	internalCreateGPUBufferFromCombined(res, sizeof(PrimitiveTypes::Float32) * (3 + 4 + 4));

	res.m_values.reset(0);

	m_pBufferSetInfo = &VertexBufferGPUManager::Instance()->m_vertexBufferInfos[PEVertexFormatLayout_MinimalSkin0_B0__P0f3_BW0f4_BW1f4_BI0f4_BI1f4];

	setAPIValues();
}

void VertexBufferGPU::createGPUBufferFromSource_ReducedSkin(PositionBufferCPU &vb, TexCoordBufferCPU &tcb, SkinWeightsCPU &weights)
{
	PEASSERT(false, "Need to implement for DEFAULT_SKIN_WEIGHTS_PER_VERTEX. seee detailed skin");
	
	PositionBufferCPU res(*m_pContext, m_arena);
	res.m_values.reset((vb.m_values.m_size / 3) * (3 + 4 + 4 + 4 + 4 + 2));

	PrimitiveTypes::UInt32 itcval = 0;

	for (PrimitiveTypes::UInt32 iv = 0; iv < vb.m_values.m_size / 3; iv++)
	{
		PrimitiveTypes::Float32 x,y,z, u, v;
		PrimitiveTypes::UInt32 curInex = iv * 3;
		x = vb.m_values[curInex]; y = vb.m_values[curInex + 1]; z = vb.m_values[curInex + 2];
		u = tcb.m_values[itcval++]; v = tcb.m_values[itcval++];

		PrimitiveTypes::Float32 vweights[8] = {-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f};
		PrimitiveTypes::Float32 jointIndices[8] = {-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f};
		assert(weights.m_weightsPerVertex[iv].m_size <= 5);
		PrimitiveTypes::UInt32 nWeights = weights.m_weightsPerVertex[iv].m_size;
		
		for (PrimitiveTypes::UInt32 ivw = 0; ivw < nWeights; ivw++)
		{
			vweights[ivw] = (PrimitiveTypes::Float32)(weights.m_weightsPerVertex[iv][ivw].m_weight);
			jointIndices[ivw] = (PrimitiveTypes::Float32)(weights.m_weightsPerVertex[iv][ivw].m_localJointIndex);
		}

		res.m_values.add(x); res.m_values.add(y); res.m_values.add(z);
		res.m_values.add(vweights[0]); res.m_values.add(vweights[1]); res.m_values.add(vweights[2]); res.m_values.add(vweights[3]);
		res.m_values.add(vweights[4]); res.m_values.add(vweights[5]); res.m_values.add(vweights[6]); res.m_values.add(vweights[7]);
		res.m_values.add(jointIndices[0]); res.m_values.add(jointIndices[1]); res.m_values.add(jointIndices[2]); res.m_values.add(jointIndices[3]);
		res.m_values.add(jointIndices[4]); res.m_values.add(jointIndices[5]); res.m_values.add(jointIndices[6]); res.m_values.add(jointIndices[7]);
		res.m_values.add(u); res.m_values.add(v);
	}

	internalCreateGPUBufferFromCombined(res, sizeof(PrimitiveTypes::Float32) * (3 + 4 + 4 + 4 + 4 + 2));
	res.m_values.reset(0);
	
	m_pBufferSetInfo = &VertexBufferGPUManager::Instance()->m_vertexBufferInfos[PEVertexFormatLayout_ReducedSkin0_B0__P0f3_BW0f4_BW1f4_BI0f4_BI1f4_TC0f2];

	setAPIValues();
}

void VertexBufferGPU::createGPUBufferFromSource_StdSkin(PositionBufferCPU &vb, TexCoordBufferCPU &tcb, SkinWeightsCPU &weights, NormalBufferCPU &nb)
{
	int vertexScalarSize = 3 + 4 + 4 + 2 + 3 + 3;
	if (DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 4)
	{
		vertexScalarSize = 3 + 4 + 4 + 2 + 3;
	}
	else if (DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 8)
	{
		vertexScalarSize = 3 + 4 + 4 + 4 + 4 + 2 + 3;
	}
	else
	{
		PEASSERT(false, "Invlaid default number of skin weights");
	}
#if APIABSTRACTION_OGL
	memset(m_bufs, 0, sizeof(m_bufs));
	m_buf = OGL_VertexBufferGPU::CreateVertexBufferInGPUFromVbNbTb(
                                                                   *m_pContext, m_arena,
                                                                   &vb, NULL, &tcb, &nb, NULL, &weights, true, &m_bufs[0]);
	m_vertexSize = sizeof(PrimitiveTypes::Float32) * vertexScalarSize;
	m_length = vb.getByteSize() / 3 / sizeof(PrimitiveTypes::Float32);
	
	m_pBufferSetInfo = &VertexBufferGPUManager::Instance()->m_vertexBufferInfos[PEVertexFormatLayout_StdSkin_B0__P0f3_B1__TC0f2_B2__N0f3_B3__BW0f4_B4__BW1f4_B5__BI0f4_B6__BI1f4];
    
#else
    PositionBufferCPU res(*m_pContext, m_arena);
    
    
    
	res.m_values.reset((vb.m_values.m_size / 3) * vertexScalarSize);

	PrimitiveTypes::UInt32 itcval = 0;

	for (PrimitiveTypes::UInt32 iv = 0; iv < vb.m_values.m_size / 3; iv++)
	{
		PrimitiveTypes::Float32 x,y,z, u, v, nx, ny, nz;
		PrimitiveTypes::UInt32 curInex = iv * 3;
		x = vb.m_values[curInex]; y = vb.m_values[curInex + 1]; z = vb.m_values[curInex + 2];
		u = tcb.m_values[itcval++]; v = tcb.m_values[itcval++];
		nx = nb.m_values[curInex]; ny = nb.m_values[curInex + 1]; nz = nb.m_values[curInex + 2];

		PrimitiveTypes::Float32 vweights[8] = {-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f};
		PrimitiveTypes::Float32 jointIndices[8] = {-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f};
		assert(weights.m_weightsPerVertex[iv].m_size <= DEFAULT_SKIN_WEIGHTS_PER_VERTEX);
		PrimitiveTypes::UInt32 nWeights = weights.m_weightsPerVertex[iv].m_size;
		
		for (PrimitiveTypes::UInt32 ivw = 0; ivw < nWeights; ivw++)
		{
			vweights[ivw] = (PrimitiveTypes::Float32)(weights.m_weightsPerVertex[iv][ivw].m_weight);
			jointIndices[ivw] = (PrimitiveTypes::Float32)(weights.m_weightsPerVertex[iv][ivw].m_localJointIndex);
		}

		res.m_values.add(x); res.m_values.add(y); res.m_values.add(z);
		res.m_values.add(vweights[0]); res.m_values.add(vweights[1]); res.m_values.add(vweights[2]); res.m_values.add(vweights[3]);
        
		if (DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 8)
		{
            res.m_values.add(vweights[4]); res.m_values.add(vweights[5]); res.m_values.add(vweights[6]); res.m_values.add(vweights[7]);
		}
        
        res.m_values.add(jointIndices[0]); res.m_values.add(jointIndices[1]); res.m_values.add(jointIndices[2]); res.m_values.add(jointIndices[3]);
        
		if (DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 8)
		{
            res.m_values.add(jointIndices[4]); res.m_values.add(jointIndices[5]); res.m_values.add(jointIndices[6]); res.m_values.add(jointIndices[7]);
        }
		res.m_values.add(u); res.m_values.add(v);
		res.m_values.add(nx); res.m_values.add(ny); res.m_values.add(nz);
	}

	internalCreateGPUBufferFromCombined(res, sizeof(PrimitiveTypes::Float32) * (3 + 4 + 4 + 4 + 4 + 2 + 3));
	res.m_values.reset(0);
	
	m_pBufferSetInfo = &VertexBufferGPUManager::Instance()->m_vertexBufferInfos[PEVertexFormatLayout_StdSkin_B0__P0f3_BW0f4_BW1f4_BI0f4_BI1f4_TC0f2_N0f3];
#endif
	setAPIValues();
}
void VertexBufferGPU::createGPUBufferFromSource_DetailedSkin(PositionBufferCPU &vb, TexCoordBufferCPU &tcb, SkinWeightsCPU &weights, NormalBufferCPU &nb, TangentBufferCPU &tb)
{
    int vertexScalarSize = 3 + 4 + 4 + 2 + 3 + 3;
	if (DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 4)
	{
		vertexScalarSize = 3 + 4 + 4 + 2 + 3 + 3;
	}
	else if (DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 8)
	{
		vertexScalarSize = 3 + 4 + 4 + 4 + 4+ 2 + 3 + 3;
	}
	else
	{
		PEASSERT(false, "Invlaid default number of skin weights");
	}
    
	//OGL is still using old way of forwarding all source buffers to low level helper implementation
#if APIABSTRACTION_OGL
	memset(m_bufs, 0, sizeof(m_bufs));
	m_buf = OGL_VertexBufferGPU::CreateVertexBufferInGPUFromVbNbTb(
		*m_pContext, m_arena,
		&vb, NULL, &tcb, &nb, &tb, &weights, true, &m_bufs[0]);
	m_vertexSize = sizeof(PrimitiveTypes::Float32) * vertexScalarSize; 
	m_length = vb.getByteSize() / 3 / sizeof(PrimitiveTypes::Float32);
	
	m_pBufferSetInfo = &VertexBufferGPUManager::Instance()->m_vertexBufferInfos[PEVertexFormatLayout_DetailedSkin_B0__P0f3_B1__TC0f2_B2__N0f3_B3__T0f3_B4__BW0f4_B5__BW1f4_B6__BI0f4_B7__BI1f4];

#else

	PositionBufferCPU res(*m_pContext, m_arena);

	

	res.m_values.reset((vb.m_values.m_size / 3) * vertexScalarSize);

	PrimitiveTypes::UInt32 itcval = 0;

	for (PrimitiveTypes::UInt32 iv = 0; iv < vb.m_values.m_size / 3; iv++)
	{
		PrimitiveTypes::Float32 x,y,z, u, v, nx, ny, nz, tx, ty, tz;
		PrimitiveTypes::UInt32 curInex = iv * 3;
		x = vb.m_values[curInex]; y = vb.m_values[curInex + 1]; z = vb.m_values[curInex + 2];
		u = tcb.m_values[itcval++]; v = tcb.m_values[itcval++]; nx = nb.m_values[curInex];
		ny = nb.m_values[curInex + 1]; nz = nb.m_values[curInex + 2];
		tx = tb.m_values[curInex]; ty = tb.m_values[curInex + 1]; tz = tb.m_values[curInex + 2];

		PrimitiveTypes::Float32 vweights[8] = {-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f};
		PrimitiveTypes::Float32 jointIndices[8] = {-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f};
		assert(weights.m_weightsPerVertex[iv].m_size <= DEFAULT_SKIN_WEIGHTS_PER_VERTEX);
		PrimitiveTypes::UInt32 nWeights = weights.m_weightsPerVertex[iv].m_size;
		
		for (PrimitiveTypes::UInt32 ivw = 0; ivw < nWeights; ivw++)
		{
			vweights[ivw] = (PrimitiveTypes::Float32)(weights.m_weightsPerVertex[iv][ivw].m_weight);
			jointIndices[ivw] = (PrimitiveTypes::Float32)(weights.m_weightsPerVertex[iv][ivw].m_localJointIndex);
		}

		res.m_values.add(x); res.m_values.add(y); res.m_values.add(z);
		
		res.m_values.add(vweights[0]); res.m_values.add(vweights[1]); res.m_values.add(vweights[2]); res.m_values.add(vweights[3]);
		if (DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 8)
		{
			res.m_values.add(vweights[4]); res.m_values.add(vweights[5]); res.m_values.add(vweights[6]); res.m_values.add(vweights[7]);
		}
		
		res.m_values.add(jointIndices[0]); res.m_values.add(jointIndices[1]); res.m_values.add(jointIndices[2]); res.m_values.add(jointIndices[3]);
		if (DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 8)
		{
			res.m_values.add(jointIndices[4]); res.m_values.add(jointIndices[5]); res.m_values.add(jointIndices[6]); res.m_values.add(jointIndices[7]);
		}
		res.m_values.add(u); res.m_values.add(v);
		res.m_values.add(nx); res.m_values.add(ny); res.m_values.add(nz);
		res.m_values.add(tx); res.m_values.add(ty); res.m_values.add(tz);
	}

	internalCreateGPUBufferFromCombined(res, sizeof(PrimitiveTypes::Float32) * vertexScalarSize);
	res.m_values.reset(0);
	m_pBufferSetInfo = &VertexBufferGPUManager::Instance()->m_vertexBufferInfos[PEVertexFormatLayout_DetailedSkin_B0__P0f3_BW0f4_BW1f4_BI0f4_BI1f4_TC0f2_N0f3_T0f3];

#endif
	setAPIValues();
}

void VertexBufferGPU::createGPUBuffer(PositionBufferCPU &vb, NormalBufferCPU &nb)
{
	PositionBufferCPU res(*m_pContext, m_arena);
	res.m_values.reset((vb.m_values.m_size / 3) * (3 + 3));

    for (PrimitiveTypes::UInt32 iv = 0; iv < vb.m_values.m_size / 3; iv++)
	{
		PrimitiveTypes::Float32 x,y,z, nx, ny, nz;
		PrimitiveTypes::UInt32 curInex = iv * 3;
		x = vb.m_values[curInex]; y = vb.m_values[curInex + 1]; z = vb.m_values[curInex + 2];
		ny = nb.m_values[curInex + 1]; nz = nb.m_values[curInex + 2];

		res.m_values.add(x); res.m_values.add(y); res.m_values.add(z);
		res.m_values.add(nx); res.m_values.add(ny); res.m_values.add(nz);
	}
	internalCreateGPUBufferFromCombined(res, sizeof(PrimitiveTypes::Float32) * (3 + 3));
	res.m_values.reset(0);
	setAPIValues();
}

void VertexBufferGPU::createGPUBufferFromSource_DetailedMesh(PositionBufferCPU &vb, TexCoordBufferCPU &tcb, NormalBufferCPU &nb, TangentBufferCPU &tb)
{
//OGL is still using old way of forwarding all source buffers to low level helper implementation
#if APIABSTRACTION_OGL
	memset(m_bufs, 0, sizeof(m_bufs));
	m_buf = OGL_VertexBufferGPU::CreateVertexBufferInGPUFromVbNbTb(
		*m_pContext, m_arena,
		&vb, NULL, &tcb, &nb, &tb, NULL, true, &m_bufs[0]);
	m_vertexSize = sizeof(PrimitiveTypes::Float32) * 11; 
	m_length = vb.getByteSize() / 3 / sizeof(PrimitiveTypes::Float32);

	m_pBufferSetInfo = &VertexBufferGPUManager::Instance()->m_vertexBufferInfos[PEVertexFormatLayout_DetailedMesh_B0__P0f3_B1__TC0f2_B2__N0f3_B3__T0f3];
#else
	PositionBufferCPU res(*m_pContext, m_arena);
	res.m_values.reset((vb.m_values.m_size / 3) * (3 + 2 + 3 + 3));

	PrimitiveTypes::UInt32 itcval = 0;

	for (PrimitiveTypes::UInt32 iv = 0; iv < vb.m_values.m_size / 3; iv++)
	{
		PrimitiveTypes::Float32 x,y,z, u, v, nx, ny, nz, tx, ty, tz;
		PrimitiveTypes::UInt32 curInex = iv * 3;
		x = vb.m_values[curInex]; y = vb.m_values[curInex + 1]; z = vb.m_values[curInex + 2];
		u = tcb.m_values[itcval++]; v = tcb.m_values[itcval++]; nx = nb.m_values[curInex];
		ny = nb.m_values[curInex + 1]; nz = nb.m_values[curInex + 2];
		tx = tb.m_values[curInex]; ty = tb.m_values[curInex + 1]; tz = tb.m_values[curInex + 2];

		res.m_values.add(x); res.m_values.add(y); res.m_values.add(z);
		res.m_values.add(u); res.m_values.add(v);
		res.m_values.add(nx); res.m_values.add(ny); res.m_values.add(nz);
		res.m_values.add(tx); res.m_values.add(ty); res.m_values.add(tz);
	}

	internalCreateGPUBufferFromCombined(res, sizeof(PrimitiveTypes::Float32) * (3 + 2 + 3 + 3));
	res.m_values.reset(0);

	m_pBufferSetInfo = &VertexBufferGPUManager::Instance()->m_vertexBufferInfos[PEVertexFormatLayout_DetailedMesh_B0__P0f3_TC0f2_N0f3_T0f3];

#endif
	
	setAPIValues();
}

void VertexBufferGPU::createGPUBufferFromSource_StdMesh(PositionBufferCPU &vb, TexCoordBufferCPU &tcb, NormalBufferCPU &nb)
{
	//ogl is still using old way of passing all buffers into lower level implmentation
	// we should be able to support multiple buffer and single buffer implementations
	#if APIABSTRACTION_OGL
		memset(m_bufs, 0, sizeof(m_bufs));
		m_buf = OGL_VertexBufferGPU::CreateVertexBufferInGPUFromVbNbTb(
			*m_pContext, m_arena,
			&vb, NULL, &tcb, &nb, NULL, NULL, true, &m_bufs[0]);
		m_vertexSize = sizeof(PrimitiveTypes::Float32) * 8; 
		m_length = vb.getByteSize() / 3 / sizeof(PrimitiveTypes::Float32);

		m_pBufferSetInfo = &VertexBufferGPUManager::Instance()->m_vertexBufferInfos[PEVertexFormatLayout_StdMesh_B0__P0f3_B1__TC0f2_B2__N0f3];
	#else
		PositionBufferCPU res(*m_pContext, m_arena);
		res.m_values.reset((vb.m_values.m_size / 3) * (3 + 2 + 3));

		PrimitiveTypes::UInt32 itcval = 0;

		for (PrimitiveTypes::UInt32 iv = 0; iv < vb.m_values.m_size / 3; iv++)
		{
			PrimitiveTypes::Float32 x,y,z, u, v, nx, ny, nz;
			PrimitiveTypes::UInt32 curInex = iv * 3;
			x = vb.m_values[curInex]; y = vb.m_values[curInex + 1]; z = vb.m_values[curInex + 2];
			u = tcb.m_values[itcval++]; v = tcb.m_values[itcval++]; nx = nb.m_values[curInex];
			ny = nb.m_values[curInex + 1]; nz = nb.m_values[curInex + 2];
			
			res.m_values.add(x); res.m_values.add(y); res.m_values.add(z);
			res.m_values.add(u); res.m_values.add(v);
			res.m_values.add(nx); res.m_values.add(ny); res.m_values.add(nz);
		}

		internalCreateGPUBufferFromCombined(res, sizeof(PrimitiveTypes::Float32) * (3 + 2 + 3));
		res.m_values.reset(0);
		m_pBufferSetInfo = &VertexBufferGPUManager::Instance()->m_vertexBufferInfos[PEVertexFormatLayout_StdMesh_B0__P0f3_TC0f2_N0f3];
	#endif

	
	setAPIValues();
}


void VertexBufferGPU::setAPIValues()
{
#if APIABSTRACTION_OGL

#endif
}


void VertexBufferGPU::releaseGPUBuffer()
{
	#if APIABSTRACTION_OGL
		OGL_VertexBufferGPU::releaseGPUBuffers(m_buf, &m_bufs[0], MAX_BUFFERS); 
		memset(m_bufs, 0, sizeof(m_bufs));
		m_buf = 0;
	#elif PE_PLAT_IS_PSVITA
	#else
		if (!m_pBuf)
			return;
		m_pBuf->Release();
		m_pBuf = 0;
	#endif
}


void VertexBufferGPU::setAsCurrent(PE::Components::Effect *pEffect)
{
	#if PE_PLAT_IS_PSVITA
	#elif APIABSTRACTION_D3D9
		D3D9Renderer *pD3D9Renderer = static_cast<D3D9Renderer *>(m_pContext->getGPUScreen());
		LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;

		D3D9_VertexBufferGPU::setAsCurrent(pDevice, m_pBuf, m_vertexSize, 0);
		PEASSERT(m_pBufferSetInfo != NULL, "Vertex Buffer is missing buffer set info");
		if (m_pBufferSetInfo)
		{
			pDevice->SetFVF(NULL);
			pDevice->SetVertexDeclaration(m_pBufferSetInfo->m_apiVertexDeclaration);
		}
#		elif APIABSTRACTION_D3D11

		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;
		D3D11_VertexBufferGPU::setAsCurrent(pDeviceContext, m_pBuf, m_vertexSize, 0);

		
#if DEBUG_DRAW_CALLS	
		D3D11_BUFFER_DESC desc;
		m_pBuf->GetDesc(&desc);
	
		PEINFO("VertexBufferGPU::setAsCurrent(): buf resource %p vertex size %d size %d B -> #verts %d\n", m_pBuf, m_vertexSize, desc.ByteWidth, desc.ByteWidth / m_vertexSize);
		PEASSERT(desc.ByteWidth / m_vertexSize == m_length, "mismatch of DX11 and internal PE data\n");
#endif
		//find appropriate D3D11 Layout object for this vertex buffer for this technique
		PEASSERT(m_pBufferSetInfo->m_vertexFormatLayout != PEVertexFormatLayout_Count, "Invlid vertex buffer storage layout");
		ID3D11InputLayout *pLayout =
		pEffect->m_externalPerTechniqueData.m_apiVertexInputLayouts[m_pBufferSetInfo->m_vertexFormatLayout]; 
		
#if DEBUG_DRAW_CALLS
		PEINFO("VertexBufferGPU::setAsCurrent(): IASetInputLayout %p vertex format layout %s \n", pLayout, toString(m_pBufferSetInfo->m_vertexFormatLayout));
#endif
		if (!pLayout) {
			pLayout = pEffect->m_externalPerTechniqueData.m_apiVertexInputLayouts[PEVertexFormatLayout_DetailedMesh_B0__P0f3_TC0f2_N0f3_T0f3];
		}
		pDeviceContext->IASetInputLayout(pLayout);

#		elif APIABSTRACTION_OGL
	
		// Bind our Vertex Array Object as the current used object
#			if OGL_USE_VERTEX_BUFFER_ARRAYS
			glBindVertexArray(m_buf);
#			else
			// we need to decribe a link between shader and gpu-bound data
			assert(m_pBufferSetInfo);
            int numBufferInfos = m_pBufferSetInfo->m_bufferInfos.m_size;
    
            //dbg
            //if (numBufferInfos == 7)
            //{
            //    PEINFO("numBufferInfos == 7\n");
            //}
    
    
			for (unsigned int i = 0; i < numBufferInfos; i++)
			{
				if (!m_bufs[i])
					continue;
				// each buffer might contain multiple semantics
				for (int isem = 0; isem < m_pBufferSetInfo->m_bufferInfos[i].m_numAttributes; isem++)
				{
					PEVertexAttributeInfo &semInfo = m_pBufferSetInfo->m_bufferInfos[i].m_attributeInfos[isem];
					if (semInfo.m_semanticStr && !semInfo.m_apiSemantic)
					{
						// CGFX_API CGparameter CGENTRY cgGetEffectParameterBySemantic(CGeffect, const char *);
#if !APIABSTRACTION_IOS
						CGparameter param = cgGetNamedParameter(pEffect->m_cgVertexProgram, semInfo.m_semanticStr);
						
						cgGLEnableClientState(param);
						IRenderer::checkForErrors("");

						glBindBuffer(GL_ARRAY_BUFFER, m_bufs[i]);
						IRenderer::checkForErrors("");

						cgGLSetParameterPointer(param, semInfo.m_numScalars, semInfo.m_apiScalarType, 0, NULL);
						IRenderer::checkForErrors("");

						glBindBuffer(GL_ARRAY_BUFFER, 0); // no current buffer
						IRenderer::checkForErrors("");
#endif
					}
					else
					{
						glBindBuffer(GL_ARRAY_BUFFER, m_bufs[i]);
                        ExternalPerTechniqueData &data = pEffect->m_externalPerTechniqueData;

						switch(semInfo.m_apiSemantic)
						{
                        case GL_VERTEX_ARRAY:
							{
								#if APIABSTRACTION_IOS
									glVertexAttribPointer(data.m_glslAttribLocations.vIn_iPosL_Attrib, semInfo.m_numScalars,  semInfo.m_apiScalarType, GL_FALSE, 0, 0);
									glEnableVertexAttribArray(data.m_glslAttribLocations.vIn_iPosL_Attrib);
								#else
									glEnableClientState(semInfo.m_apiSemantic); // GL_VERTEX_ARRAY = 0x8074, GL_NORMAL_ARRAY = 0x8075, GL_TEXTURE_COORD_ARRAY = 0x8078
									glVertexPointer(semInfo.m_numScalars, semInfo.m_apiScalarType,0,NULL); 
								#endif
								break;
							}
						case GL_NORMAL_ARRAY:
							{
								#if APIABSTRACTION_IOS
									glEnableVertexAttribArray(data.m_glslAttribLocations.vIn_iNormal_Attrib);
									glVertexAttribPointer(data.m_glslAttribLocations.vIn_iNormal_Attrib, semInfo.m_numScalars,  semInfo.m_apiScalarType, GL_FALSE, 0, 0);
								#else
									//why are we using TEXCOORD2 for cg semantic? test the normal buffer input in shader!
									glEnableClientState(semInfo.m_apiSemantic); // GL_VERTEX_ARRAY, GL_NORMAL_ARRAY, GL_TEXTURE_COORD_ARRAY
									glNormalPointer(semInfo.m_apiScalarType,0,NULL);
								#endif
								break; //glDisableClientState(GL_NORMAL_ARRAY);
							}
						case GL_COLOR_ARRAY:
							{
								#if APIABSTRACTION_IOS
									glEnableVertexAttribArray(data.m_glslAttribLocations.vIn_iColor_Attrib);
									glVertexAttribPointer(data.m_glslAttribLocations.vIn_iColor_Attrib, semInfo.m_numScalars,  semInfo.m_apiScalarType, GL_FALSE, 0, 0);
								#else
									glEnableClientState(semInfo.m_apiSemantic); // GL_VERTEX_ARRAY, GL_NORMAL_ARRAY, GL_TEXTURE_COORD_ARRAY
									glColorPointer(semInfo.m_numScalars, semInfo.m_apiScalarType, 0, NULL);
								#endif
								break;
							}
						case GL_TEXTURE_COORD_ARRAY:
							{
								#if APIABSTRACTION_IOS
									if (semInfo.m_apiSemanticOrder == 0)
									{
                                        if (data.m_glslAttribLocations.vIn_iTexCoord_Attrib != GLuint(-1))
										{
                                            glEnableVertexAttribArray(data.m_glslAttribLocations.vIn_iTexCoord_Attrib);
                                            glVertexAttribPointer(data.m_glslAttribLocations.vIn_iTexCoord_Attrib, semInfo.m_numScalars, semInfo.m_apiScalarType, GL_FALSE, 0, 0);
                                        }
									}
									else if (semInfo.m_apiSemanticOrder == 1)
									{
                                        if (data.m_glslAttribLocations.vIn_iTangent_Attrib != GLuint(-1))
										{
                                            glEnableVertexAttribArray(data.m_glslAttribLocations.vIn_iTangent_Attrib);
                                            glVertexAttribPointer(data.m_glslAttribLocations.vIn_iTangent_Attrib, semInfo.m_numScalars, semInfo.m_apiScalarType, GL_FALSE, 0, 0);
                                        }
									}
                                    else if (semInfo.m_apiSemanticOrder == 2)
									{
                                        if (data.m_glslAttribLocations.vIn_iNormal_Attrib != GLuint(-1))
										{ 
                                            glEnableVertexAttribArray(data.m_glslAttribLocations.vIn_iNormal_Attrib);
                                            glVertexAttribPointer(data.m_glslAttribLocations.vIn_iNormal_Attrib, semInfo.m_numScalars, semInfo.m_apiScalarType, GL_FALSE, 0, 0);  
                                        }
									}
                                    else if (semInfo.m_apiSemanticOrder == 3)
									{
                                        if (data.m_glslAttribLocations.vIn_jointWeights_Attrib != GLuint(-1))
										{ 
                                            glEnableVertexAttribArray(data.m_glslAttribLocations.vIn_jointWeights_Attrib);
                                            glVertexAttribPointer(data.m_glslAttribLocations.vIn_jointWeights_Attrib, semInfo.m_numScalars, semInfo.m_apiScalarType, GL_FALSE, 0, 0);  
                                        }
									}
                                    else if (semInfo.m_apiSemanticOrder == 4)
									{
                                        if (data.m_glslAttribLocations.vIn_jointWeights1_Attrib != GLuint(-1))
										{ 
                                            glEnableVertexAttribArray(data.m_glslAttribLocations.vIn_jointWeights1_Attrib);
                                            glVertexAttribPointer(data.m_glslAttribLocations.vIn_jointWeights1_Attrib, semInfo.m_numScalars, semInfo.m_apiScalarType, GL_FALSE, 0, 0);  
                                        }
									}
                                    else if (semInfo.m_apiSemanticOrder == 5)
									{
                                        if (data.m_glslAttribLocations.vIn_jointIndices_Attrib != GLuint(-1))
										{ 
                                            glEnableVertexAttribArray(data.m_glslAttribLocations.vIn_jointIndices_Attrib);
                                            glVertexAttribPointer(data.m_glslAttribLocations.vIn_jointIndices_Attrib, semInfo.m_numScalars, semInfo.m_apiScalarType, GL_FALSE, 0, 0);  
                                        }
									}
                                    else if (semInfo.m_apiSemanticOrder == 6)
									{
                                        if (data.m_glslAttribLocations.vIn_jointIndices1_Attrib != GLuint(-1))
										{ 
                                            glEnableVertexAttribArray(data.m_glslAttribLocations.vIn_jointIndices1_Attrib);
                                            glVertexAttribPointer(data.m_glslAttribLocations.vIn_jointIndices1_Attrib, semInfo.m_numScalars, semInfo.m_apiScalarType, GL_FALSE, 0, 0);  
                                        }
									}
                                    else
                                    {
                                        assert(!"Unhandled semantic order!");
                                    }
								#else

									if (semInfo.m_apiSemanticOrder)
									{
										glClientActiveTexture(GL_TEXTURE0 + semInfo.m_apiSemanticOrder);
									}
									else
										glClientActiveTexture(GL_TEXTURE0);

									glEnableClientState(semInfo.m_apiSemantic); // GL_VERTEX_ARRAY, GL_NORMAL_ARRAY, GL_TEXTURE_COORD_ARRAY
									glTexCoordPointer(semInfo.m_numScalars, semInfo.m_apiScalarType,0,NULL);
									glClientActiveTexture(GL_TEXTURE0);
								#endif
								break; //glDisableClientState(GL_TEXTURE_COORD_ARRAY);
							}
                        default: assert(!"Unhandled api semantic!");
						}
						glBindBuffer(GL_ARRAY_BUFFER, 0); // no current buffer
					}
					IRenderer::checkForErrors("setting buffer\n");
				} // for each semantic in buffer
			} //for each buffer
/*
			//debug printouts
			OGL_GPUScreen::outputGLBoolean("GL_VERTEX_ARRAY: ", GL_VERTEX_ARRAY, "\n");
			glClientActiveTexture(GL_TEXTURE0);
			OGL_GPUScreen::outputGLBoolean("GL_TEXTURE_COORD_ARRAY[0]: ", GL_TEXTURE_COORD_ARRAY, "\n");
			glClientActiveTexture(GL_TEXTURE1);
			OGL_GPUScreen::outputGLBoolean("GL_TEXTURE_COORD_ARRAY[1]: ", GL_TEXTURE_COORD_ARRAY, "\n");
			glClientActiveTexture(GL_TEXTURE2);
			OGL_GPUScreen::outputGLBoolean("GL_TEXTURE_COORD_ARRAY[2]: ", GL_TEXTURE_COORD_ARRAY, "\n");
			glClientActiveTexture(GL_TEXTURE3);
			OGL_GPUScreen::outputGLBoolean("GL_TEXTURE_COORD_ARRAY[3]: ", GL_TEXTURE_COORD_ARRAY, "\n");
			OGL_GPUScreen::outputGLBoolean("GL_NORMAL_ARRAY: ", GL_NORMAL_ARRAY, "\n");
			glClientActiveTexture(GL_TEXTURE0);
*/
			/*
			//reference
			cgParamPositionV = cgGetNamedParameter(cgProgramV, "position");
			cgParamNormalV = cgGetNamedParameter(cgProgramV, "normal");
			
			cgGLSetParameterPointer(cgParamPositionV, 3, GL_FLOAT, 0, pVertices);
			cgGLEnableClientState(cgParamPositionV);
			cgGLSetParameterPointer(cgParamNormalV, 3, GL_FLOAT, 0, pNormals);
			cgGLEnableClientState(cgParamNormalV);
			*/


#			endif

#		endif
}

void VertexBufferGPU::unbindFromPipeline(PE::Components::Effect *pEffect)
{
#	if APIABSTRACTION_OGL
	
		// Bind our Vertex Array Object as the current used object
#		if !OGL_USE_VERTEX_BUFFER_ARRAYS

			// we need to decribe a link between shader and gpu-bound data
			for (unsigned int i = 0; i < m_pBufferSetInfo->m_bufferInfos.m_size; i++)
			{
				if (!m_bufs[i])
					continue;
				for (int isem = 0; isem < m_pBufferSetInfo->m_bufferInfos[i].m_numAttributes; isem++)
				{
					PEVertexAttributeInfo &semInfo = m_pBufferSetInfo->m_bufferInfos[i].m_attributeInfos[isem];
					if (semInfo.m_semanticStr && !semInfo.m_apiSemantic)
					{
						// CGFX_API CGparameter CGENTRY cgGetEffectParameterBySemantic(CGeffect, const char *);
#if !APIABSTRACTION_IOS
						CGparameter param = cgGetNamedParameter(pEffect->m_cgVertexProgram, semInfo.m_semanticStr);
						
						cgGLDisableClientState(param);
						IRenderer::checkForErrors("");
#endif
					}
					else
					{
						
						switch(semInfo.m_apiSemantic)
						{
						case GL_VERTEX_ARRAY:
							{
                                #if APIABSTRACTION_IOS
                                    glDisableVertexAttribArray(0);
                                #else
                                    glDisableClientState(semInfo.m_apiSemantic); // GL_VERTEX_ARRAY, GL_NORMAL_ARRAY, GL_TEXTURE_COORD_ARRAY
                                #endif
								break;
							}
						case GL_NORMAL_ARRAY:
							{
                                #if APIABSTRACTION_IOS
                                    glDisableVertexAttribArray(1);
                                #else
                                    glDisableClientState(semInfo.m_apiSemantic); // GL_VERTEX_ARRAY, GL_NORMAL_ARRAY, GL_TEXTURE_COORD_ARRAY
                                #endif
								break; //glDisableClientState(GL_NORMAL_ARRAY);
							}
						case GL_COLOR_ARRAY:
							{
								#if APIABSTRACTION_IOS
								glDisableVertexAttribArray(1);
								#else
								glDisableClientState(semInfo.m_apiSemantic); // GL_VERTEX_ARRAY, GL_NORMAL_ARRAY, GL_TEXTURE_COORD_ARRAY
								#endif
								break; //glDisableClientState(GL_NORMAL_ARRAY);
							}
						case GL_TEXTURE_COORD_ARRAY:
							{
                                #if APIABSTRACTION_IOS
                                #else
                                    if (semInfo.m_apiSemanticOrder)
                                        glClientActiveTexture(GL_TEXTURE0 + semInfo.m_apiSemanticOrder);
                                    else
                                        glClientActiveTexture(GL_TEXTURE0);

                                    glDisableClientState(semInfo.m_apiSemantic); // GL_VERTEX_ARRAY, GL_NORMAL_ARRAY, GL_TEXTURE_COORD_ARRAY
                                    glClientActiveTexture(GL_TEXTURE0);
                                #endif
								break; //glDisableClientState(GL_TEXTURE_COORD_ARRAY);
							}
						}
					}
				}
			}
/*
			//debug printouts
			OGL_GPUScreen::outputGLBoolean("GL_VERTEX_ARRAY: ", GL_VERTEX_ARRAY, "\n");
			glClientActiveTexture(GL_TEXTURE0);
			OGL_GPUScreen::outputGLBoolean("GL_TEXTURE_COORD_ARRAY[0]: ", GL_TEXTURE_COORD_ARRAY, "\n");
			glClientActiveTexture(GL_TEXTURE1);
			OGL_GPUScreen::outputGLBoolean("GL_TEXTURE_COORD_ARRAY[1]: ", GL_TEXTURE_COORD_ARRAY, "\n");
			glClientActiveTexture(GL_TEXTURE2);
			OGL_GPUScreen::outputGLBoolean("GL_TEXTURE_COORD_ARRAY[2]: ", GL_TEXTURE_COORD_ARRAY, "\n");
			glClientActiveTexture(GL_TEXTURE3);
			OGL_GPUScreen::outputGLBoolean("GL_TEXTURE_COORD_ARRAY[3]: ", GL_TEXTURE_COORD_ARRAY, "\n");
			OGL_GPUScreen::outputGLBoolean("GL_NORMAL_ARRAY: ", GL_NORMAL_ARRAY, "\n");
			glClientActiveTexture(GL_TEXTURE0);
*/
			/*
			//reference
			cgParamPositionV = cgGetNamedParameter(cgProgramV, "position");
			cgParamNormalV = cgGetNamedParameter(cgProgramV, "normal");
			
			cgGLSetParameterPointer(cgParamPositionV, 3, GL_FLOAT, 0, pVertices);
			cgGLEnableClientState(cgParamPositionV);
			cgGLSetParameterPointer(cgParamNormalV, 3, GL_FLOAT, 0, pNormals);
			cgGLEnableClientState(cgParamNormalV);
			*/


#			endif

#		endif
}
}; // namespace PE

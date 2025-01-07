#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "VertexBufferGPUManager.h"
#include "../GPUMaterial/GPUMaterialSet.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/AnimSetBufferGPU.h"

namespace PE {

Handle VertexBufferGPUManager::s_myHandle;

VertexBufferGPUManager::VertexBufferGPUManager(PE::GameContext &context, PE::MemoryArena arena)
: m_map(context, arena, 1024)
, m_matSetGPUMap(context, arena, 512)
, m_IndexGPUMap(context, arena, 1024)
{
	m_arena = arena; m_pContext = &context;
}
// Reads the specified buffer from file
Handle VertexBufferGPUManager::createGPUBuffer(Handle hvb, Handle htcb, Handle hnb, bool useBufferRegistry)
{
	Handle handles[] = {hvb, htcb, Handle(), hnb, Handle()};
	Handle res;
	
	if (useBufferRegistry)
	{
		res = m_map.findHandle(handles);
		if (res.isValid())
		{
			// already have it
			return res;
		}
	}

	res = Handle("VERTEX_BUFFER_GPU", sizeof(VertexBufferGPU));
	VertexBufferGPU *pvbgpu = new(res) VertexBufferGPU(*m_pContext, m_arena);

	pvbgpu->createGPUBufferFromSource_StdMesh(
		*hvb.getObject<PositionBufferCPU>(),
		*htcb.getObject<TexCoordBufferCPU>(),
		*hnb.getObject<NormalBufferCPU>());

	if (useBufferRegistry)
		m_map.add(handles, res);
	
	return res;
}

Handle VertexBufferGPUManager::createGPUBufferFromVBufTCBufNBufTBuf(Handle hvb, Handle htcb, Handle hnb, Handle hTBuf, bool useBufferRegistry)
{
	Handle handles[] = {hvb, htcb, Handle(), hnb, hTBuf};

	Handle res;
	
	if (useBufferRegistry)
	{
		res = m_map.findHandle(handles);
		if (res.isValid())
		{
			// already have it
			return res;
		}
	}

	res = Handle("VERTEX_BUFFER_GPU", sizeof(VertexBufferGPU));
	VertexBufferGPU *pvbgpu = new(res) VertexBufferGPU(*m_pContext, m_arena);

	pvbgpu->createGPUBufferFromSource_DetailedMesh(
		*hvb.getObject<PositionBufferCPU>(), 
		*htcb.getObject<TexCoordBufferCPU>(),
		*hnb.getObject<NormalBufferCPU>(),
		*hTBuf.getObject<TangentBufferCPU>());

	if (useBufferRegistry)
		m_map.add(handles, res);
	
	return res;
}

Handle VertexBufferGPUManager::createGPUBufferFromVBufTCBufSWBufNBufTBuf(Handle hvb, Handle htcb, Handle hSWBuf, Handle hnb, Handle hTBuf, bool useBufferRegistry)
{
	Handle handles[] = {hvb, htcb, hSWBuf, hnb, hTBuf};

	Handle res;
	
	if (useBufferRegistry)
	{
		res = m_map.findHandle(handles);
		if (res.isValid())
		{
			// already have it
			return res;
		}
	}
	
	res = Handle("VERTEX_BUFFER_GPU", sizeof(VertexBufferGPU));
	VertexBufferGPU *pvbgpu = new(res) VertexBufferGPU(*m_pContext, m_arena);

	pvbgpu->createGPUBufferFromSource_DetailedSkin(
		*hvb.getObject<PositionBufferCPU>(), 
		*htcb.getObject<TexCoordBufferCPU>(),
		*hSWBuf.getObject<SkinWeightsCPU>(),
		*hnb.getObject<NormalBufferCPU>(),
		*hTBuf.getObject<TangentBufferCPU>());

	if (useBufferRegistry)
		m_map.add(handles, res);
	
	return res;
}

Handle VertexBufferGPUManager::createGPUBufferFromVBufTCBufSWBufNBuf(Handle hvb, Handle htcb, Handle hSWBuf, Handle hnb, bool useBufferRegistry)
{
	Handle handles[] = {hvb, htcb, hSWBuf, hnb, Handle()};

	Handle res;
	
	if (useBufferRegistry)
	{
		res = m_map.findHandle(handles);
		if (res.isValid())
		{
			// already have it
			return res;
		}
	}

	res = Handle("VERTEX_BUFFER_GPU", sizeof(VertexBufferGPU));
	VertexBufferGPU *pvbgpu = new(res) VertexBufferGPU(*m_pContext, m_arena);

	pvbgpu->createGPUBufferFromSource_StdSkin(
		*hvb.getObject<PositionBufferCPU>(), 
		*htcb.getObject<TexCoordBufferCPU>(),
		*hSWBuf.getObject<SkinWeightsCPU>(),
		*hnb.getObject<NormalBufferCPU>()
	);

	if (useBufferRegistry)
		m_map.add(handles, res);
	
	return res;
}

Handle VertexBufferGPUManager::createFromSource_ColoredMinimalMesh(Handle hpb, Handle hcb, bool useBufferRegistry)
{
	PEASSERT(!useBufferRegistry, "Since right now colored minimal meshes are for debug purposes only, we dotn allow using bufer registry, add support for it if needed to cache the buffer");

	Handle res = Handle("VERTEX_BUFFER_GPU", sizeof(VertexBufferGPU));
	VertexBufferGPU *pvbgpu = new(res) VertexBufferGPU(*m_pContext, m_arena);

	pvbgpu->createGPUBufferFromSource_ColoredMinimalMesh(
		*hpb.getObject<PositionBufferCPU>(), 
		*hcb.getObject<ColorBufferCPU>());

	return res;
}


Handle VertexBufferGPUManager::createMatSetGPUFromMatSetCPU(Handle hMatSetCPU)
{
	Handle handles[] = {hMatSetCPU};
	Handle res = m_matSetGPUMap.findHandle(handles);
	if (res.isValid())
	{
		// already have it
		return res;
	}

	res  = Handle("MATERIAL_SET_GPU", sizeof(GPUMaterialSet));
	GPUMaterialSet *pmsgpu = new(res) GPUMaterialSet(*m_pContext, m_arena);
	pmsgpu->createFromMaterialSetCPU(*hMatSetCPU.getObject<MaterialSetCPU>());

	m_matSetGPUMap.add(handles, res);
	return res;
}

Handle VertexBufferGPUManager::createIndexGPUBuffer(Handle hib, bool useBufferRegistry)
{
	Handle res;
	Handle handles[] = {hib};
	if (useBufferRegistry)
	{
		res = m_IndexGPUMap.findHandle(handles);
		if (res.isValid())
		{
			// already have it
			return res;
		}
	}

	res = Handle("INDEX_BUFFER_GPU", sizeof(IndexBufferGPU));
	IndexBufferGPU *pibgpu = new(res) IndexBufferGPU(*m_pContext, m_arena);
	pibgpu->createGPUBuffer(*hib.getObject<IndexBufferCPU>());
	if(useBufferRegistry)
		m_IndexGPUMap.add(handles, res);
	return res;
}

void VertexBufferGPUManager::setupVertexBufferInfos()
{
	// these are default BufferSet layout infos. Each buffer set can create its own if needed but for most
	// part they should be able to reuse one of the infos below

	// ColoredMinimalMesh (version 0) has only one buffer with all elements within one stride
	{
		PEVertexBufferInfo info(*m_pContext, m_arena, PEVertexFormatLayout_ColoredMinimalMesh0_B0__P0f3_C0f3);
		info.m_bufferInfos.reset(1);
		PEVertexAttributeBufferInfo buf0;
		// position
		buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 3, PESemanticType_Position, "position", 0);
		buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(3 * 4, PEScalarType_Float, 3, PESemanticType_Color, "color", 0);
		info.m_bufferInfos.add(buf0);

		info.setAPIValues();

		m_vertexBufferInfos[info.m_vertexFormatLayout] = info;
		m_layoutToFormatMap[info.m_vertexFormatLayout] = PEVertexFormat_ColoredMinimalMesh;
	}

	// ColoredMinimalMesh (version 1) one buffer per vertex element(semantic)
	{
		PEVertexBufferInfo info(*m_pContext, m_arena, PEVertexFormatLayout_ColoredMinimalMesh1_B0__P0f3_B1__C0f3);
		info.m_bufferInfos.reset(2);
		PEVertexAttributeBufferInfo buf0, buf1;
		// position
		buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 3, PESemanticType_Position, "position", 0);
		buf1.m_attributeInfos[buf1.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 3, PESemanticType_Color, "color", 0);
		info.m_bufferInfos.add(buf0, buf1);

		info.setAPIValues();

		m_vertexBufferInfos[info.m_vertexFormatLayout] = info;
		m_layoutToFormatMap[info.m_vertexFormatLayout] = PEVertexFormat_ColoredMinimalMesh;
	}

	// ReducedMesh has only one buffer with all elements within one stride
	{
		PEVertexBufferInfo info(*m_pContext, m_arena, PEVertexFormatLayout_ReducedMesh_B0__P0f3_TC0f2);
		info.m_bufferInfos.reset(1);
		PEVertexAttributeBufferInfo buf0;
		// position
		buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 3, PESemanticType_Position, "position", 0);
		buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(3 * 4, PEScalarType_Float, 2, PESemanticType_TexCoord, "texcoord", 0);
		info.m_bufferInfos.add(buf0);

		info.setAPIValues();

		m_vertexBufferInfos[info.m_vertexFormatLayout] = info;
		m_layoutToFormatMap[info.m_vertexFormatLayout] = PEVertexFormat_ReducedMesh;
	}

	// StdMesh has only one buffer with all elements within one stride
	{
		PEVertexBufferInfo info(*m_pContext, m_arena, PEVertexFormatLayout_StdMesh_B0__P0f3_TC0f2_N0f3);
		info.m_bufferInfos.reset(1);
		PEVertexAttributeBufferInfo buf0;
		// position
		buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 3, PESemanticType_Position, "position", 0);
		buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(3 * 4, PEScalarType_Float, 2, PESemanticType_TexCoord, "texcoord", 0);
		buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(5 * 4, PEScalarType_Float, 3, PESemanticType_Normal, "normal", 0);
		info.m_bufferInfos.add(buf0);

		info.setAPIValues();

		m_vertexBufferInfos[info.m_vertexFormatLayout] = info;
		m_layoutToFormatMap[info.m_vertexFormatLayout] = PEVertexFormat_StdMesh;
	}
	
	//StdMesh with all attributes in separate buffers
	{
		PEVertexBufferInfo info(*m_pContext, m_arena, PEVertexFormatLayout_StdMesh_B0__P0f3_B1__TC0f2_B2__N0f3);
		info.m_bufferInfos.reset(3);
		PEVertexAttributeBufferInfo buf0, buf1, buf2;
		// position
		buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 3, PESemanticType_Position, "position", 0);
		buf1.m_attributeInfos[buf1.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 2, PESemanticType_TexCoord, "texcoord", 0);
		buf2.m_attributeInfos[buf2.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 3, PESemanticType_Normal, "normal", 0);
		info.m_bufferInfos.add(buf0, buf1, buf2);

		info.setAPIValues();

		m_vertexBufferInfos[info.m_vertexFormatLayout] = info;
		m_layoutToFormatMap[info.m_vertexFormatLayout] = PEVertexFormat_StdMesh;
	}
	
	// DetailedMesh has only one buffer with all elements(attributes) within one stride
	{
		PEVertexBufferInfo info(*m_pContext, m_arena, PEVertexFormatLayout_DetailedMesh_B0__P0f3_TC0f2_N0f3_T0f3);
		info.m_bufferInfos.reset(1);
		PEVertexAttributeBufferInfo buf0;
		// position
		buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(0, PEScalarType_Float, 3, PESemanticType_Position, "position", 0);
		buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(3 * 4, PEScalarType_Float, 2, PESemanticType_TexCoord, "texcoord", 0);
		buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(5 * 4, PEScalarType_Float, 3, PESemanticType_Normal, "normal", 0);
		buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(8 * 4, PEScalarType_Float, 3, PESemanticType_Tangent, "tangent", 0);
		
		info.m_bufferInfos.add(buf0);
		info.setAPIValues();

		m_vertexBufferInfos[info.m_vertexFormatLayout] = info;
		m_layoutToFormatMap[info.m_vertexFormatLayout] = PEVertexFormat_DetailedMesh;
	}
	// DetailedMesh with all vertex elements(attributes) in separate buffers
	{
		PEVertexBufferInfo info(*m_pContext, m_arena, PEVertexFormatLayout_DetailedMesh_B0__P0f3_B1__TC0f2_B2__N0f3_B3__T0f3);
		info.m_bufferInfos.reset(4);
		PEVertexAttributeBufferInfo buf0, buf1, buf2, buf3;
		// position
		buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(0, PEScalarType_Float, 3, PESemanticType_Position, "position", 0);
		buf1.m_attributeInfos[buf1.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 2, PESemanticType_TexCoord, "texcoord", 0);
		buf2.m_attributeInfos[buf2.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 3, PESemanticType_Normal, "normal", 0);
		buf3.m_attributeInfos[buf3.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 3, PESemanticType_Tangent, "tangent", 0);

		info.m_bufferInfos.add(buf0, buf1, buf2, buf3);
		info.setAPIValues();

		m_vertexBufferInfos[info.m_vertexFormatLayout] = info;
		m_layoutToFormatMap[info.m_vertexFormatLayout] = PEVertexFormat_DetailedMesh;
	}

	// MinimalSkin (version 0) has only one buffer with all elements within one stride
	{
		PEVertexBufferInfo info(*m_pContext, m_arena, PEVertexFormatLayout_MinimalSkin0_B0__P0f3_BW0f4_BW1f4_BI0f4_BI1f4);
		info.m_bufferInfos.reset(1);
		PEVertexAttributeBufferInfo buf0;
		// position
		buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(0, PEScalarType_Float, 3, PESemanticType_Position, "position", 0);
		if (DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 4)
		{
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(3 * 4, PEScalarType_Float, 4, PESemanticType_JointWeights, "jointWeights", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(7 * 4, PEScalarType_Float, 4, PESemanticType_JointIndices, "jointIndices", 0);
		}
		else if (DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 8)
		{
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(3 * 4, PEScalarType_Float, 4, PESemanticType_JointWeights, "jointWeights", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(7 * 4, PEScalarType_Float, 4, PESemanticType_JointWeights, "jointWeights", 1);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(11 * 4, PEScalarType_Float, 4, PESemanticType_JointIndices, "jointIndices", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(15 * 4, PEScalarType_Float, 4, PESemanticType_JointIndices, "jointIndices", 1);
		}
		
		info.m_bufferInfos.add(buf0);
		info.setAPIValues();

		m_vertexBufferInfos[info.m_vertexFormatLayout] = info;
		m_layoutToFormatMap[info.m_vertexFormatLayout] = PEVertexFormat_MinimalSkin;
	}

	// ReducedSkin (version 0) has only one buffer with all elements within one stride
	{
		PEVertexBufferInfo info(*m_pContext, m_arena, PEVertexFormatLayout_ReducedSkin0_B0__P0f3_BW0f4_BW1f4_BI0f4_BI1f4_TC0f2);
		info.m_bufferInfos.reset(1);
		PEVertexAttributeBufferInfo buf0;
		// position
		buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(0, PEScalarType_Float, 3, PESemanticType_Position, "position", 0);
		

		if (DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 4)
		{
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(3 * 4, PEScalarType_Float, 4, PESemanticType_JointWeights, "jointWeights", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(7 * 4, PEScalarType_Float, 4, PESemanticType_JointIndices, "jointIndices", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(11 * 4, PEScalarType_Float, 2, PESemanticType_TexCoord, "texcoord", 0);
		}
		else if (DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 8)
		{
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(3 * 4, PEScalarType_Float, 4, PESemanticType_JointWeights, "jointWeights", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(7 * 4, PEScalarType_Float, 4, PESemanticType_JointWeights, "jointWeights", 1);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(11 * 4, PEScalarType_Float, 4, PESemanticType_JointIndices, "jointIndices", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(15 * 4, PEScalarType_Float, 4, PESemanticType_JointIndices, "jointIndices", 1);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(19 * 4, PEScalarType_Float, 2, PESemanticType_TexCoord, "texcoord", 0);
		}

		
		info.m_bufferInfos.add(buf0);
		info.setAPIValues();

		m_vertexBufferInfos[info.m_vertexFormatLayout] = info;
		m_layoutToFormatMap[info.m_vertexFormatLayout] = PEVertexFormat_ReducedSkin;
	}

	// StdSkin (version 0) has only one buffer with all elements within one stride
	{
		PEVertexBufferInfo info(*m_pContext, m_arena, PEVertexFormatLayout_StdSkin_B0__P0f3_BW0f4_BW1f4_BI0f4_BI1f4_TC0f2_N0f3);
		info.m_bufferInfos.reset(1);
		PEVertexAttributeBufferInfo buf0;
		// position
		buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(0, PEScalarType_Float, 3, PESemanticType_Position, "position", 0);
		
		if (DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 4)
		{
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(3 * 4, PEScalarType_Float, 4, PESemanticType_JointWeights, "jointWeights", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(7 * 4, PEScalarType_Float, 4, PESemanticType_JointIndices, "jointIndices", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(11 * 4, PEScalarType_Float, 2, PESemanticType_TexCoord, "texcoord", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(13 * 4, PEScalarType_Float, 3, PESemanticType_Normal, "normal", 0);
		}
		else if (DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 8)
		{
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(3 * 4, PEScalarType_Float, 4, PESemanticType_JointWeights, "jointWeights", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(7 * 4, PEScalarType_Float, 4, PESemanticType_JointWeights, "jointWeights", 1);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(11 * 4, PEScalarType_Float, 4, PESemanticType_JointIndices, "jointIndices", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(15 * 4, PEScalarType_Float, 4, PESemanticType_JointIndices, "jointIndices", 1);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(19 * 4, PEScalarType_Float, 2, PESemanticType_TexCoord, "texcoord", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(21 * 4, PEScalarType_Float, 3, PESemanticType_Normal, "normal", 0);
		}

		info.m_bufferInfos.add(buf0);
		info.setAPIValues();

		m_vertexBufferInfos[info.m_vertexFormatLayout] = info;
		m_layoutToFormatMap[info.m_vertexFormatLayout] = PEVertexFormat_StdSkin;
	}
    
    // StdSkin (version 1) that has all vertex elements(attributes) in separate buffers
	{
		PEVertexBufferInfo info(*m_pContext, m_arena, PEVertexFormatLayout_StdSkin_B0__P0f3_B1__TC0f2_B2__N0f3_B3__BW0f4_B4__BW1f4_B5__BI0f4_B6__BI1f4);
		info.m_bufferInfos.reset(8);
		PEVertexAttributeBufferInfo buf0, buf1, buf2, buf3, buf4, buf5, buf6, buf7;
		// position
		
		if (DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 4)
		{
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(0, PEScalarType_Float, 3, PESemanticType_Position, "position", 0);
			buf1.m_attributeInfos[buf1.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 2, PESemanticType_TexCoord, "texcoord", 0);
			buf2.m_attributeInfos[buf2.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 3, PESemanticType_Normal, "normal", 0);
			buf3.m_attributeInfos[buf3.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 4, PESemanticType_JointWeights, "jointWeights", 0);
			buf4.m_attributeInfos[buf4.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 4, PESemanticType_JointIndices, "jointIndices", 0);
            
			info.m_bufferInfos.add(buf0, buf1, buf2, buf3);
			info.m_bufferInfos.add(buf4);
		}
		else if (DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 8)
		{
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(0, PEScalarType_Float, 3, PESemanticType_Position, "position", 0);
			buf1.m_attributeInfos[buf1.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 2, PESemanticType_TexCoord, "texcoord", 0);
			buf2.m_attributeInfos[buf2.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 3, PESemanticType_Normal, "normal", 0);
			buf3.m_attributeInfos[buf3.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 4, PESemanticType_JointWeights, "jointWeights", 0);
			buf4.m_attributeInfos[buf4.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 4, PESemanticType_JointWeights, "jointWeights", 1);
			buf5.m_attributeInfos[buf5.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 4, PESemanticType_JointIndices, "jointIndices", 0);
			buf6.m_attributeInfos[buf6.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 4, PESemanticType_JointIndices, "jointIndices", 1);
            
			info.m_bufferInfos.add(buf0, buf1, buf2, buf3);
			info.m_bufferInfos.add(buf4, buf5, buf6);
		}
        
		info.setAPIValues();
		m_vertexBufferInfos[info.m_vertexFormatLayout] = info;
		m_layoutToFormatMap[info.m_vertexFormatLayout] = PEVertexFormat_StdSkin;
	}

	// DetailedSkin has only one buffer with all elements within one stride
	{
		PEVertexBufferInfo info(*m_pContext, m_arena, PEVertexFormatLayout_DetailedSkin_B0__P0f3_BW0f4_BW1f4_BI0f4_BI1f4_TC0f2_N0f3_T0f3);
		info.m_bufferInfos.reset(1);
		PEVertexAttributeBufferInfo buf0;
		// position
		
		if (DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 4)
		{
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(0, PEScalarType_Float, 3, PESemanticType_Position, "position", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(3 * 4, PEScalarType_Float, 4, PESemanticType_JointWeights, "jointWeights", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(7 * 4, PEScalarType_Float, 4, PESemanticType_JointIndices, "jointIndices", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(11 * 4, PEScalarType_Float, 2, PESemanticType_TexCoord, "texcoord", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(13 * 4, PEScalarType_Float, 3, PESemanticType_Normal, "normal", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(16 * 4, PEScalarType_Float, 3, PESemanticType_Tangent, "tangent", 0);
		}
		else if (DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 8)
		{
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(0, PEScalarType_Float, 3, PESemanticType_Position, "position", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(3 * 4, PEScalarType_Float, 4, PESemanticType_JointWeights, "jointWeights", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(7 * 4, PEScalarType_Float, 4, PESemanticType_JointWeights, "jointWeights", 1);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(11 * 4, PEScalarType_Float, 4, PESemanticType_JointIndices, "jointIndices", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(15 * 4, PEScalarType_Float, 4, PESemanticType_JointIndices, "jointIndices", 1);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(19 * 4, PEScalarType_Float, 2, PESemanticType_TexCoord, "texcoord", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(21 * 4, PEScalarType_Float, 3, PESemanticType_Normal, "normal", 0);
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(24 * 4, PEScalarType_Float, 3, PESemanticType_Tangent, "tangent", 0);
		}

		info.m_bufferInfos.add(buf0);
		info.setAPIValues();
		m_vertexBufferInfos[info.m_vertexFormatLayout] = info;
		m_layoutToFormatMap[info.m_vertexFormatLayout] = PEVertexFormat_DetailedSkin;
	}

	// DetailedSkin that has all vertex elements(attributes) in separate buffers
	#if !PE_PLAT_IS_PSVITA // vita supports up to 4 vertex streams
	{
		PEVertexBufferInfo info(*m_pContext, m_arena, PEVertexFormatLayout_DetailedSkin_B0__P0f3_B1__TC0f2_B2__N0f3_B3__T0f3_B4__BW0f4_B5__BW1f4_B6__BI0f4_B7__BI1f4);
		info.m_bufferInfos.reset(8);
		PEVertexAttributeBufferInfo buf0, buf1, buf2, buf3, buf4, buf5, buf6, buf7;
		// position
		
		if (DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 4)
		{
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(0, PEScalarType_Float, 3, PESemanticType_Position, "position", 0);
			buf1.m_attributeInfos[buf1.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 2, PESemanticType_TexCoord, "texcoord", 0);
			buf2.m_attributeInfos[buf2.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 3, PESemanticType_Normal, "normal", 0);
			buf3.m_attributeInfos[buf3.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 3, PESemanticType_Tangent, "tangent", 0);
			buf4.m_attributeInfos[buf4.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 4, PESemanticType_JointWeights, "jointWeights", 0);
			buf5.m_attributeInfos[buf5.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 4, PESemanticType_JointIndices, "jointIndices", 0);
		
			info.m_bufferInfos.add(buf0, buf1, buf2, buf3);
			info.m_bufferInfos.add(buf4, buf5);
		}
		else if (DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 8)
		{
			buf0.m_attributeInfos[buf0.m_numAttributes++] = PEVertexAttributeInfo(0, PEScalarType_Float, 3, PESemanticType_Position, "position", 0);
			buf1.m_attributeInfos[buf1.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 2, PESemanticType_TexCoord, "texcoord", 0);
			buf2.m_attributeInfos[buf2.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 3, PESemanticType_Normal, "normal", 0);
			buf3.m_attributeInfos[buf3.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 3, PESemanticType_Tangent, "tangent", 0);
			buf4.m_attributeInfos[buf4.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 4, PESemanticType_JointWeights, "jointWeights", 0);
			buf5.m_attributeInfos[buf5.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 4, PESemanticType_JointWeights, "jointWeights", 1);
			buf6.m_attributeInfos[buf6.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 4, PESemanticType_JointIndices, "jointIndices", 0);
			buf7.m_attributeInfos[buf7.m_numAttributes++] = PEVertexAttributeInfo(0 * 4, PEScalarType_Float, 4, PESemanticType_JointIndices, "jointIndices", 1);
		
			info.m_bufferInfos.add(buf0, buf1, buf2, buf3);
			info.m_bufferInfos.add(buf4, buf5, buf6, buf7);
		}

		info.setAPIValues();
		m_vertexBufferInfos[info.m_vertexFormatLayout] = info;
		m_layoutToFormatMap[info.m_vertexFormatLayout] = PEVertexFormat_DetailedSkin;
	}
	#endif

}

}; // namespace PE

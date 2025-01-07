#include "PositionBufferCPUManager.h"
#include "../IndexBufferCPU/IndexBufferCPU.h"
#include "../TexCoordBufferCPU/TexCoordBufferCPU.h"
#include "../TangentBufferCPU/TangentBufferCPU.h"
#include "../SkeletonCPU/SkinWeightsCPU.h"
#include "../MaterialCPU/MaterialSetCPU.h"

namespace PE {
Handle PositionBufferCPUManager::s_myHandle;

PositionBufferCPUManager::PositionBufferCPUManager(PE::GameContext &context, PE::MemoryArena arena)
	: m_map(context, arena, 1024)
	, m_iBufferCPUMap(context, arena, 512)
	, m_tcBufferCPUMap(context, arena, 512)
	, m_nBufferCPUMap(context, arena, 512)
	, m_tBufferCPUMap(context, arena, 512)
	, m_SWCPUMap(context, arena, 256)
	, m_MatSetMap(context, arena, 512)
{
	m_arena = arena; m_pContext = &context;
}
// Reads the specified buffer from file
Handle PositionBufferCPUManager::ReadVertexBuffer(const char *filename, const char *package, const char *tag)
{
	char key[256];
	StringOps::concat(filename, tag, &key[0], 256);

	Handle res = m_map.findHandle(key);
	if (res.isValid())
	{
		// already have it
		return res;
	}

	res = Handle("VERTEX_BUFFER_CPU", sizeof(PositionBufferCPU));
	PositionBufferCPU *pvbcpu = new(res) PositionBufferCPU(*m_pContext, m_arena);

	pvbcpu->ReadPositionBuffer(filename, package);

	m_map.add(key, res);
	return res;
}

Handle PositionBufferCPUManager::ReadIndexBuffer(const char *filename, const char *package, const char *tag)
{
	char key[256];
	StringOps::concat(filename, tag, &key[0], 256);

	Handle res = m_iBufferCPUMap.findHandle(key);
	if (res.isValid())
	{
		// already have it
		return res;
	}

	res = Handle("INDEX_BUFFER_CPU", sizeof(IndexBufferCPU));
	IndexBufferCPU *pib = new(res) IndexBufferCPU(*m_pContext, m_arena);
	pib->ReadIndexBuffer(filename, package);
	
	m_iBufferCPUMap.add(key, res);
	return res;
}

Handle PositionBufferCPUManager::ReadTexCoordBuffer(const char *filename, const char *package, const char *tag)
{
	char key[256];
	StringOps::concat(filename, tag, &key[0], 256);

	Handle res = m_tcBufferCPUMap.findHandle(key);
	if (res.isValid())
	{
		// already have it
		return res;
	}

	res = Handle("TEXCOORD_BUFFER_CPU", sizeof(TexCoordBufferCPU));
	TexCoordBufferCPU *ptcbcpu = new(res) TexCoordBufferCPU(*m_pContext, m_arena);

	ptcbcpu->ReadTexCoordBuffer(filename, package);

	m_tcBufferCPUMap.add(key, res);
	return res;
}

Handle PositionBufferCPUManager::ReadTangentBuffer(const char *filename, const char *package, const char *tag)
{
	char key[256];
	StringOps::concat(filename, tag, &key[0], 256);

	Handle res = m_tBufferCPUMap.findHandle(key);
	if (res.isValid())
	{
		// already have it
		return res;
	}

	res = Handle("TANGENT_BUFFER_CPU", sizeof(TangentBufferCPU));
	TangentBufferCPU *ptbcpu = new(res) TangentBufferCPU(*m_pContext, m_arena);

	ptbcpu->ReadTangentBuffer(filename, package);

	m_tBufferCPUMap.add(key, res);
	return res;
}

Handle PositionBufferCPUManager::ReadSkinWeights(const char *filename, const char *package, const char *tag)
{
	char key[256];
	StringOps::concat(filename, tag, &key[0], 256);

	Handle res = m_SWCPUMap.findHandle(key);
	if (res.isValid())
	{
		// already have it
		return res;
	}

	res = Handle("SKIN_WEIGHTS_CPU", sizeof(SkinWeightsCPU));

	SkinWeightsCPU *psw = new(res) SkinWeightsCPU(*m_pContext, m_arena);
	psw->ReadSkinWeights(filename, package);

	m_SWCPUMap.add(key, res);
	return res;
}

Handle PositionBufferCPUManager::ReadMaterialSetCPU(const char *filename, const char *package)
{
	Handle res = m_MatSetMap.findHandle(filename);
	if (res.isValid())
	{
		// already have it
		return res;
	}

	res = Handle("MATERIAL_SET_CPU", sizeof(MaterialSetCPU));
	MaterialSetCPU *pmset = new(res) MaterialSetCPU(*m_pContext, m_arena);
	pmset->ReadMaterialSet(filename, package);

	m_MatSetMap.add(filename, res);
	return res;
}

}; // namespace PE

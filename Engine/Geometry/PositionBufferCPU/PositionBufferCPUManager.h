#ifndef __PYENGINE_2_0_VERTEX_BUFFER_CPU_MANAGER__
#define __PYENGINE_2_0_VERTEX_BUFFER_CPU_MANAGER__

#define NOMINMAX

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/Math/Matrix4x4.h"
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/Utils/StringOps.h"
#include "../../Utils/Array/Array.h"
#include "../../Utils/StrToHandleMap.h"
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"

// Sibling/Children includes
#include "PositionBufferCPU.h"
namespace PE {

struct PositionBufferCPUManager : PE::PEAllocatableAndDefragmentable
{
	PositionBufferCPUManager(PE::GameContext &context, PE::MemoryArena arena);

	static void Construct(PE::GameContext &context, PE::MemoryArena arena)
	{
		s_myHandle = Handle("VERTEX_BUFFER_CPU_MANAGER", sizeof(PositionBufferCPUManager));
		/* PositionBufferCPUManager *pMan = */ new(s_myHandle) PositionBufferCPUManager(context, arena);
	}

	static PositionBufferCPUManager* Instance()
	{
		return s_myHandle.getObject<PositionBufferCPUManager>();
	}

	// Reads the specified buffer from file
	Handle ReadIndexBuffer(const char *filename, const char *package, const char *tag);
	Handle ReadVertexBuffer(const char *filename, const char *package, const char *tag);
	Handle ReadTexCoordBuffer(const char *filename, const char *package, const char *tag);
	Handle ReadTangentBuffer(const char *filename, const char *package, const char *tag);

	Handle ReadMaterialSetCPU(const char *filename, const char *package = NULL);
	Handle ReadSkinWeights(const char *filename, const char *package, const char *tag);

	static Handle s_myHandle;

	StrToHandleMap m_map;

	StrToHandleMap m_iBufferCPUMap;
	StrToHandleMap m_tcBufferCPUMap;
	StrToHandleMap m_nBufferCPUMap;
	StrToHandleMap m_tBufferCPUMap;

	StrToHandleMap m_SWCPUMap;

	StrToHandleMap m_MatSetMap;

	PE::MemoryArena m_arena; PE::GameContext *m_pContext;
};
}; // namespace PE
#endif

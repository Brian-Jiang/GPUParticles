#ifndef __PYENGINE_2_0_VERTEX_BUFFER_GPU_MANAGER__
#define __PYENGINE_2_0_VERTEX_BUFFER_GPU_MANAGER__

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
#include "../../Utils/HandlesToHandleMap.h"
#include "../../Utils/PEMap.h"
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"

#include "../../Geometry/PositionBufferCPU/PositionBufferCPU.h"
#include "../../Geometry/TexCoordBufferCPU/TexCoordBufferCPU.h"
#include "../../Geometry/NormalBufferCPU/NormalBufferCPU.h"
#include "../../Geometry/SkeletonCPU/AnimationSetCPU.h"


#include "BufferInfo.h"
// Sibling/Children includes
#include "VertexBufferGPU.h"

namespace PE {

struct VertexBufferGPUManager : public PE::PEAllocatableAndDefragmentable
{
	VertexBufferGPUManager(PE::GameContext &context, PE::MemoryArena arena);

	static void Construct(PE::GameContext &context, PE::MemoryArena arena)
	{
		Handle h("VERTEX_BUFFER_GPU_MANAGER", sizeof(VertexBufferGPUManager));
		VertexBufferGPUManager *pMan = new(h) VertexBufferGPUManager(context, arena);
		s_myHandle = h;
		pMan->setupVertexBufferInfos();
	}

	static VertexBufferGPUManager* Instance()
	{
		return s_myHandle.getObject<VertexBufferGPUManager>();
	}

	// Reads the specified buffer from file
	Handle createGPUBuffer(Handle hvb, Handle htcb, Handle hnb, bool useBufferRegistry);

	Handle createGPUBufferFromVBufTCBufNBufTBuf(Handle hvb, Handle htcb, Handle hnb, Handle hTBuf, bool useBufferRegistry);
	Handle createGPUBufferFromVBufTCBufSWBufNBufTBuf(Handle hvb, Handle htcb, Handle hSWBuf, Handle hnb, Handle hTBuf, bool useBufferRegistry);
	Handle createGPUBufferFromVBufTCBufSWBufNBuf(Handle hvb, Handle htcb, Handle hSWBuf, Handle hnb, bool useBufferRegistry);

	Handle createFromSource_ColoredMinimalMesh(Handle hpb, Handle hcb, bool useBufferRegistry);

	Handle createMatSetGPUFromMatSetCPU(Handle hMatSetCPU);

	Handle createIndexGPUBuffer(Handle hib, bool useBufferRegistry);

	void setupVertexBufferInfos();

	static Handle s_myHandle;

	HandlesToHandleMap<5> m_map;
	HandlesToHandleMap<1> m_matSetGPUMap;

	HandlesToHandleMap<1> m_IndexGPUMap;

	EPEVertexFormat m_layoutToFormatMap[PEVertexFormatLayout_Count];
	PEVertexBufferInfo m_vertexBufferInfos[PEVertexFormatLayout_Count];

	PE::MemoryArena m_arena; PE::GameContext *m_pContext;
};
}; // namespace PE
#endif

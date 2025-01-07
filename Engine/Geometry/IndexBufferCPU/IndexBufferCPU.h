#ifndef __PYENGINE_2_0_INDEX_BUFFER_CPU__
#define __PYENGINE_2_0_INDEX_BUFFER_CPU__
// This class works with index buffers in system memory
// common usage: load from file and create a index buffer in GPU memory

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/Utils/Array/Array.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/BufferInfo.h"

// Sibling/Children includes

struct IndexRange
{
	IndexRange(PE::GameContext &context, PE::MemoryArena arena);
	PrimitiveTypes::UInt32 m_start;
	PrimitiveTypes::UInt32 m_end;

	PrimitiveTypes::UInt32 m_minVertIndex;
	PrimitiveTypes::UInt32 m_maxVertIndex;

	struct BoneSegment {
		BoneSegment(PE::GameContext &context, PE::MemoryArena arena):m_boneSegmentBones(context, arena)
		{}
		Array<PrimitiveTypes::UInt32 > m_boneSegmentBones;

		PrimitiveTypes::UInt32 m_start;
		PrimitiveTypes::UInt32 m_end;
	};
	
	Array<BoneSegment> m_boneSegments; // further separate the range into bone segments

};

struct IndexBufferCPU : PE::PEAllocatableAndDefragmentable
{
	IndexBufferCPU(PE::GameContext &context, PE::MemoryArena arena) : m_values(context, arena, 0), m_indexRanges(context, arena), m_vertsPerFacePerRange(context, arena)
	{
		m_arena = arena; m_pContext = &context;
        m_dbgName[0] = '\0';
	}

	// Reads the specified buffer from file
	void ReadIndexBuffer(const char *filename, const char *package = NULL);

	void createEmptyCPUBuffer();

	void createBillboardCPUBuffer();

	// start of the index data in memory
	void *getStartAddress(){return &m_values[0];}

	PrimitiveTypes::UInt32 getByteSize(){return m_values.m_size * sizeof(PrimitiveTypes::UInt16);}

	Array<PrimitiveTypes::UInt16> m_values;
	Array<IndexRange> m_indexRanges;

	Array<PrimitiveTypes::UInt32> m_vertsPerFacePerRange;
	int m_minVertexIndex;
	int m_maxVertexIndex;

	PEPrimitveTopology m_primitiveTopology;
	PrimitiveTypes::Int32 m_verticesPerPolygon;

    char m_dbgName[256];
	PE::MemoryArena m_arena; PE::GameContext *m_pContext;
};

#endif

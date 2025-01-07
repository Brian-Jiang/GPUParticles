#ifndef __PYENGINE_2_0_TEX_COORD_BUFFER_CPU__
#define __PYENGINE_2_0_TEX_COORD_BUFFER_CPU__
// This class works with tex coord buffers in system memory
// common usage: load from file and create a vertex buffer in GPU memory from combined PositionBufferCPU and TexCoordBufferCPU

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../../Utils/Array/Array.h"

// Sibling/Children includes

struct TexCoordBufferCPU : PE::PEAllocatableAndDefragmentable
{
	TexCoordBufferCPU(PE::GameContext &context, PE::MemoryArena arena) : m_values(context, arena, 0)
	{
		m_pContext = &context;
	}

	// Reads the specified buffer from file
	void ReadTexCoordBuffer(const char *filename, const char *package = NULL);

	void createBillboardCPUBuffer();
	void createMockCPUBuffer(PrimitiveTypes::UInt32 nVertices);

	// start of the vertex data in memory
	void *getStartAddress(){return &m_values[0];}

	PrimitiveTypes::UInt32 getByteSize(){return m_values.m_size * sizeof(PrimitiveTypes::Float32);}

	Array<PrimitiveTypes::Float32> m_values;

	PE::GameContext *m_pContext;
};

#endif

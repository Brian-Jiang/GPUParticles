#ifndef __PYENGINE_2_0_VERTEX_BUFFER_CPU__
#define __PYENGINE_2_0_VERTEX_BUFFER_CPU__
// This class works with vertex buffers in system memory
// common usage: load from file and create a vertex buffer in GPU memory

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../../Utils/Array/Array.h"

// Sibling/Children includes

struct PositionBufferCPU : PE::PEAllocatableAndDefragmentable
{
	PositionBufferCPU(PE::GameContext &context, PE::MemoryArena arena) : m_values(context, arena, 0)
	{
		m_pContext = &context;
	}

	// Reads the specified buffer from file
	void ReadPositionBuffer(const char *filename, const char *package = NULL);

	void createEmptyCPUBuffer();

	void createBillboardCPUBuffer(PrimitiveTypes::Float32 w, PrimitiveTypes::Float32 h);
	void createNormalizeBillboardCPUBufferXYWithPtOffsets(PrimitiveTypes::Float32 dx, PrimitiveTypes::Float32 dy);

	void add3Floats(PrimitiveTypes::Float32 f0, PrimitiveTypes::Float32 f1, PrimitiveTypes::Float32 f2)
	{
		m_values.add(f0); m_values.add(f1); m_values.add(f2);
	}

	// start of the vertex data in memory
	void *getStartAddress(){return &m_values[0];}

	PrimitiveTypes::UInt32 getByteSize(){return m_values.m_size * sizeof(PrimitiveTypes::Float32);}

	Array<PrimitiveTypes::Float32> m_values;

	PE::GameContext *m_pContext;
};

#endif

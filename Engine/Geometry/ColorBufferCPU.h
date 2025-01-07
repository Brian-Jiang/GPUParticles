#ifndef __PYENGINE_2_0_COLOR_BUFFER_CPU__
#define __PYENGINE_2_0_COLOR_BUFFER_CPU__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/Utils/StringOps.h"
#include "PrimeEngine/Utils/Array/Array.h"
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"
// Sibling/Children includes

struct ColorBufferCPU : PE::PEAllocatableAndDefragmentable
{
	ColorBufferCPU(PE::GameContext &context, PE::MemoryArena arena) : m_values(context, arena, 0)
	{
		m_pContext = &context;
	}

	// Reads the specified buffer from file
	void ReadColorBuffer(const char *filename, const char *package = NULL);

	void createEmptyCPUBuffer();

	void createBillboardCPUBuffer();

	// start of the normal data in memory
	void *getStartAddress(){return &m_values[0];}

	PrimitiveTypes::UInt32 getByteSize(){return m_values.m_size * sizeof(PrimitiveTypes::Float32);}

	Array<PrimitiveTypes::Float32> m_values;
	PE::GameContext *m_pContext;
};

#endif

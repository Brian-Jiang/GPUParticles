#ifndef __PYENGINE_2_0_TANGENT_BUFFER_CPU__
#define __PYENGINE_2_0_TANGENT_BUFFER_CPU__


// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/Utils/StringOps.h"
#include "../../Utils/Array/Array.h"
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"
// Sibling/Children includes

struct TangentBufferCPU : PE::PEAllocatableAndDefragmentable
{
	TangentBufferCPU(PE::GameContext &context, PE::MemoryArena arena);

	// Reads the specified buffer from file
	void ReadTangentBuffer(const char *filename, const char *package = NULL);

	void createMockCPUBuffer(PrimitiveTypes::UInt32 nVertices);
	// start of the vertex data in memory
	void *getStartAddress(){return &m_values[0];}

	PrimitiveTypes::UInt32 getByteSize(){return m_values.m_size * sizeof(PrimitiveTypes::Float32);}

	Array<PrimitiveTypes::Float32> m_values;

	PE::GameContext *m_pContext;
};

#endif

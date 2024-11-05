#ifndef __PYENGINE_2_0_SKIN_WEIGHTS_CPU__
#define __PYENGINE_2_0_SKIN_WEIGHTS_CPU__

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

struct WeightPair
{
	PrimitiveTypes::UInt32 m_jointIndex;
	PrimitiveTypes::Float32 m_weight;
	PrimitiveTypes::UInt32 m_localJointIndex; // index of the bone when parts of skin are rendered and max of 16 bones are given to gpu
};

struct SkinWeightsCPU : PE::PEAllocatableAndDefragmentable
{
	SkinWeightsCPU(PE::GameContext &context, PE::MemoryArena arena);

	// Reads the specified buffer from file
	void ReadSkinWeights(const char *filename, const char *package);

	void reduceWeights(int num);

	Array<Array<WeightPair> > m_weightsPerVertex;
	PE::MemoryArena m_arena; PE::GameContext *m_pContext;
};

#endif

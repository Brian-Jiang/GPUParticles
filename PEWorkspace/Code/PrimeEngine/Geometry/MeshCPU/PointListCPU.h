#ifndef __PYENGINE_2_0_POINT_LIST_CPU__
#define __PYENGINE_2_0_POINT_LIST_CPU__

#define NOMINMAX
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

#include "../PositionBufferCPU/PositionBufferCPU.h"
#include "../TexCoordBufferCPU/TexCoordBufferCPU.h"
#include "../IndexBufferCPU/IndexBufferCPU.h"
#include "../NormalBufferCPU/NormalBufferCPU.h"

#include "PrimeEngine/Math/Vector4.h"
// Sibling/Children includes

namespace PE {

// This class is a simple POD struct that holds all the CPU information about the mesh before it is given to DX to be put in GPU memory
struct PointListCPU : PE::PEAllocatableAndDefragmentable
{
	PointListCPU(PE::GameContext &context, PE::MemoryArena arena):m_colorArr(context, arena)
	{
		m_arena = arena; m_pContext = &context;
	}

	void create(PrimitiveTypes::Float32 x0, PrimitiveTypes::Float32 x1, PrimitiveTypes::Float32 y0, PrimitiveTypes::Float32 y1, PrimitiveTypes::UInt32 nsprites);
	

	void createPS3PS(PrimitiveTypes::Float32 x0, PrimitiveTypes::Float32 x1, PrimitiveTypes::Float32 y0, PrimitiveTypes::Float32 y1, PrimitiveTypes::UInt32 nsprites);
	

	void createVoxelSpace(PrimitiveTypes::Float32 x0, PrimitiveTypes::Float32 x1,
		PrimitiveTypes::Float32 y0, PrimitiveTypes::Float32 y1,
		PrimitiveTypes::Float32 z0, PrimitiveTypes::Float32 z1,
		PrimitiveTypes::UInt32 numVoxelsPerDim,
		char *pTechName);
	

	Handle m_hPositionBufferCPU;
	Handle m_hTexCoordBufferCPU;
	Handle m_hOrientationTexCoordBufferCPU;
	
	Array<Vector4> m_colorArr;
	Handle m_hMaterialSetCPU;

	PE::MemoryArena m_arena; PE::GameContext *m_pContext;

	char m_csTechName[64];
};
}; // namespace PE
#endif

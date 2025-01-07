#ifndef __PYENGINE_2_0_ANIMATION_SET_CPU_MANAGER__
#define __PYENGINE_2_0_ANIMATION_SET_CPU_MANAGER__

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
#include "AnimationSetCPU.h"
namespace PE {

struct AnimationSetGPUManager : PE::PEAllocatableAndDefragmentable
{
	AnimationSetGPUManager(PE::GameContext &context, PE::MemoryArena arena) : m_map(context, arena, 1024)
	{
		
	}

	static void Construct(PE::GameContext &context, PE::MemoryArena arena)
	{
		s_myHandle = Handle("ANIMATION_SET_CPU_MANAGER", sizeof(AnimationSetGPUManager));
		/*AnimationSetGPUManager *pMan = */ new(s_myHandle) AnimationSetGPUManager(context, arena);
	}

	static AnimationSetGPUManager* Instance()
	{
		return s_myHandle.getObject<AnimationSetGPUManager>();
	}

	// Reads the specified buffer from file
	Handle ReadAnimationSet(PE::GameContext &context, PE::MemoryArena arena, const char *filename, const char *package, SkeletonCPU &skel);

	static Handle s_myHandle;

	StrToHandleMap m_map;
};
}; // namespace PE
#endif

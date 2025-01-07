#ifndef __PYENGINE_2_0_ANIMATION_SET_CPU__
#define __PYENGINE_2_0_ANIMATION_SET_CPU__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/Math/Matrix4x4.h"
#include "../../Utils/Array/Array.h"
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"

// Sibling/Children includes
#include "AnimationCPU.h"
namespace PE {

struct AnimSpeedSet : PE::PEAllocatableAndDefragmentable
{
	AnimSpeedSet(PE::GameContext &context)
	{
		m_pContext = &context;
		m_size = 64;
	}

	void ReadAnimationSpeedSet(const char *filename, const char *package = NULL);

	void saveBackToFile();
	static Handle Construct(PE::GameContext &context, const char *filename, const char *package = NULL)
	{
		Handle h("T_ANIM_SPEED_SET", sizeof(AnimSpeedSet));
		AnimSpeedSet *p = new(h) AnimSpeedSet(context);
		p->ReadAnimationSpeedSet(filename, package);
		return h;
	}

	PrimitiveTypes::UInt32 m_size;
	PrimitiveTypes::Float32 m_v64[64];
	char m_filename[1024];

	PE::GameContext *m_pContext;

	
};

struct AnimNameSet : PE::PEAllocatableAndDefragmentable
{
	AnimNameSet(PE::GameContext &context, PE::MemoryArena arena):m_strings(context, arena, 64)
	{
		m_pContext = &context;
		for (unsigned int i = 0; i < m_strings.m_capacity; ++i)
			m_strings.add(PEString(context, arena));
	}

	void ReadAnimationNameSet(const char *filename, const char *package = NULL);

	static Handle Construct(PE::GameContext &context, PE::MemoryArena arena)
	{
		Handle h("T_ANIM_NAME_SET", sizeof(AnimNameSet));
		new(h) AnimNameSet(context, arena);
		return h;
	}

	Array<PEString> m_strings;
	PrimitiveTypes::UInt32 m_size;

	PE::GameContext *m_pContext;
};


struct AnimationSetCPU : PE::PEAllocatableAndDefragmentable
{
	AnimationSetCPU(PE::GameContext &context, PE::MemoryArena arena):m_animations(context, arena)
	{
		m_arena = arena; m_pContext = &context;
	}

	// Reads the specified buffer from file
	void ReadAnimationSet(const char *filename, const char *package, SkeletonCPU &skel);

	Array<AnimationCPU> m_animations;
	PE::MemoryArena m_arena;
	PE::GameContext *m_pContext;
	char m_name[256];
};

}; // namespace PE

#endif

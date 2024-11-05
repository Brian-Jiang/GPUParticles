#ifndef __PYENGINE_2_0_ANIMATION_CPU__
#define __PYENGINE_2_0_ANIMATION_CPU__

#define STORE_AS_TSQ

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/Math/Matrix4x4.h"
#include "PrimeEngine/Math/TSQ.h"
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/Utils/StringOps.h"
#include "../../Utils/Array/Array.h"
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"

namespace PE
{

// Sibling/Children includes
enum AnimSlotFlags
{
	PARTIAL_BODY_ANIMATION = 0x01,
	ACTIVE = 0x02,
	LOOPING = 0x04,
	FADING_AWAY = 0x08,
	NOTIFY_ON_ANIMATION_END = 0x010,
	STAY_ON_ANIMATION_END = 0x020,
};

struct AnimationSlot
{
	AnimationSlot(PrimitiveTypes::UInt32 animationSetIndex, PrimitiveTypes::UInt32 animationIndex, PrimitiveTypes::Float32 frameIndex,
		PrimitiveTypes::Float32 framesLeft,
		PrimitiveTypes::UInt32 startJoint, PrimitiveTypes::UInt32 endJoint,
		PrimitiveTypes::UInt32 flags,
		PrimitiveTypes::Float32 weight = 1.0f,
		PrimitiveTypes::Float32 numFramesToNotifyBeforeEnd = 0.0f)
	{
		m_animationSetIndex = animationSetIndex;
		m_animationIndex = animationIndex;
		m_frameIndex = frameIndex;
		m_startJoint = startJoint;
		m_endJoint = endJoint;
		m_flags = flags;
		m_iFrameIndex0 = 0;
		m_iFrameIndex1 = 0;
		m_blendFactor = 0;
		m_framesLeft = framesLeft;
		m_numFrames = framesLeft;
		m_weight = weight;
		m_numFramesToNotifyBeforeEnd = numFramesToNotifyBeforeEnd;
	}
	
	AnimationSlot()
	{
		m_flags = 0;
		m_iFrameIndex0 = 0;
		m_iFrameIndex1 = 0;
		m_blendFactor = 0.0f;
		m_framesLeft = -1;
		m_numFrames = m_framesLeft;
		m_weight = 1.0f;
		m_numFramesToNotifyBeforeEnd = 0.0f;
	}

	PrimitiveTypes::Float32 getProgressPercentage()
	{
		return m_frameIndex / m_numFrames;
	}

	PrimitiveTypes::Bool isLooping()
	{
		return (m_flags & LOOPING) > 0;
	}
	void setNotLooping()
	{
		m_flags = m_flags & (~LOOPING);
	}

	void setNotActive()
	{
		m_flags = m_flags & (~ACTIVE);
	}

	PrimitiveTypes::Bool isFadingAway()
	{
		return (m_flags & FADING_AWAY) > 0;
	}
	void setFadingAway()
	{
		m_flags = m_flags | FADING_AWAY;
	}

	PrimitiveTypes::Bool isActive()
	{
		return (m_flags & ACTIVE) > 0;
	}

	PrimitiveTypes::Bool needToNotifyOnAnimationEnd()
	{
		return (m_flags & NOTIFY_ON_ANIMATION_END) > 0;
	}

	void setNotToNotifyOnAnimationEnd()
	{
		m_flags = m_flags & (~NOTIFY_ON_ANIMATION_END);
	}
	
	PrimitiveTypes::Bool needToStayOnLastFrame()
	{
		return (m_flags & STAY_ON_ANIMATION_END) > 0;
	}

	void setToStayOnLastFrame()
	{
		m_flags = m_flags & (~STAY_ON_ANIMATION_END);
	}


	PrimitiveTypes::UInt32 m_animationSetIndex;
	PrimitiveTypes::UInt32 m_animationIndex;
	PrimitiveTypes::Float32 m_frameIndex;
	PrimitiveTypes::UInt32 m_iFrameIndex0;
	PrimitiveTypes::UInt32 m_iFrameIndex1;
	PrimitiveTypes::Float32 m_blendFactor;
	PrimitiveTypes::UInt32 m_startJoint;
	PrimitiveTypes::UInt32 m_endJoint;
	PrimitiveTypes::UInt32 m_flags;
	PrimitiveTypes::Float32 m_framesLeft;
	PrimitiveTypes::Float32 m_numFrames;

	PrimitiveTypes::Float32 m_weight;
	PrimitiveTypes::Float32 m_numFramesToNotifyBeforeEnd;

	
};

struct SkeletonCPU;

struct AnimationCPU : PE::PEAllocatableAndDefragmentable
{
	AnimationCPU(PE::GameContext &context, PE::MemoryArena arena):m_frames(context, arena)
	{
		m_arena = arena; m_pContext = &context;
		m_name[0] = '\0';
	}

	// Reads the animation from file
	void ReadAnimation(FileReader &f, SkeletonCPU &skel, float positionFactor, int version);

	// Member vars -------------------------------------------------------------
	PrimitiveTypes::UInt32 m_numJoints;
	PrimitiveTypes::UInt32 m_startJoint;
	PrimitiveTypes::UInt32 m_endJoint;
	PE::MemoryArena m_arena; PE::GameContext *m_pContext;

	Array<Array<TSQ> > m_frames; 

	char m_name[128];
};
};
#endif

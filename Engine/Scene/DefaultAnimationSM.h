#ifndef _PE_DEFAULT_ANIMATION_STATE_MACHINE_H_
#define _PE_DEFAULT_ANIMATION_STATE_MACHINE_H_

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "../Geometry/SkeletonCPU/AnimationCPU.h"
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"

// Sibling/Children includes
#include "Mesh.h"

namespace PE {
namespace Components {

// this class is in charge of advancing skin animations on UPDATE
// it is a simple controller that will loop through animations
struct DefaultAnimationSM : public Component
{
	PE_DECLARE_CLASS(DefaultAnimationSM);

	DefaultAnimationSM(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself);

	virtual ~DefaultAnimationSM(){}

	virtual void addDefaultComponents() ;

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_SCENE_GRAPH_UPDATE);
	virtual void do_SCENE_GRAPH_UPDATE(Events::Event *pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_CALCULATE_TRANSFORMATIONS);
	virtual void do_CALCULATE_TRANSFORMATIONS(Events::Event *pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_Event_PLAY_ANIMATION);
	virtual void do_Event_PLAY_ANIMATION(Events::Event *pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_PRE_RENDER_needsRC)
	void do_PRE_RENDER_needsRC(PE::Events::Event *pEvt);



	void setSlot(PrimitiveTypes::UInt32 slotId, AnimationSlot &slot)
	{
		m_animSlots[slotId] = slot;
	}

	PrimitiveTypes::Int32 setAnimationWeight(
		PrimitiveTypes::UInt32 animationSetIndex,
		PrimitiveTypes::UInt32 animationIndex,
		PrimitiveTypes::UInt32 firstActiveAnimSlotIndex,
		PrimitiveTypes::UInt32 lastActiveAnimSlotIndex,
		PrimitiveTypes::Float32 weight);

	PrimitiveTypes::Int32 setWeightsBetweenAnimations(
		PrimitiveTypes::UInt32 animationSetIndex0,
		PrimitiveTypes::UInt32 animationIndex0,
		PrimitiveTypes::UInt32 animationSetIndex1,
		PrimitiveTypes::UInt32 animationIndex1,
		PrimitiveTypes::UInt32 firstActiveAnimSlotIndex,
		PrimitiveTypes::UInt32 lastActiveAnimSlotIndex,
		PrimitiveTypes::Float32 alpha);

	AnimationSlot* setAnimation(
		PrimitiveTypes::UInt32 animationSetIndex,
		PrimitiveTypes::UInt32 animationIndex,
		PrimitiveTypes::UInt32 firstActiveAnimSlotIndex,
		PrimitiveTypes::UInt32 lastActiveAnimSlotIndex,
		PrimitiveTypes::UInt32 firstFadingAnimSlotIndex,
		PrimitiveTypes::UInt32 lastFadingAnimSlotIndex,
		PrimitiveTypes::UInt32 additionalFlags = 0,
		PrimitiveTypes::Float32 weight = 1.0f,
		Array<PrimitiveTypes::UInt32> *animationsNotToExclude = NULL,
		PrimitiveTypes::Bool useProgressPercentageOfReplacedAnimation = true,
		PrimitiveTypes::Float32 customPercentage = 0.0f);

	void setAnimations(
		PrimitiveTypes::UInt32 animationSetIndex0,
		PrimitiveTypes::UInt32 animationIndex0,
		PrimitiveTypes::UInt32 animationSetIndex1,
		PrimitiveTypes::UInt32 animationIndex1,
		PrimitiveTypes::Float32 alpha,
		PrimitiveTypes::UInt32 firstActiveAnimSlotIndex,
		PrimitiveTypes::UInt32 lastActiveAnimSlotIndex,
		PrimitiveTypes::UInt32 firstFadingAnimSlotIndex,
		PrimitiveTypes::UInt32 lastFadingAnimSlotIndex,
		PrimitiveTypes::UInt32 additionalFlags = 0);

	void disableAnimation(
		PrimitiveTypes::UInt32 animationSetIndex,
		PrimitiveTypes::UInt32 animationIndex,
		PrimitiveTypes::UInt32 firstActiveAnimSlotIndex,
		PrimitiveTypes::UInt32 lastActiveAnimSlotIndex);

	void disableAnimations(
		PrimitiveTypes::UInt32 firstActiveAnimSlotIndex,
		PrimitiveTypes::UInt32 lastActiveAnimSlotIndex);

	void fadeAnimation(
		PrimitiveTypes::UInt32 animationSetIndex,
		PrimitiveTypes::UInt32 animationIndex,
		PrimitiveTypes::UInt32 firstActiveAnimSlotIndex,
		PrimitiveTypes::UInt32 lastActiveAnimSlotIndex,
		PrimitiveTypes::UInt32 firstFadingAnimSlotIndex,
		PrimitiveTypes::UInt32 lastFadingAnimSlotIndex);


	AnimationSlot *getSlot(
		PrimitiveTypes::UInt32 animationSetIndex,
		PrimitiveTypes::UInt32 animationIndex,
		PrimitiveTypes::UInt32 firstActiveAnimSlotIndex,
		PrimitiveTypes::UInt32 lastActiveAnimSlotIndex);

	PrimitiveTypes::Float32 getProgressPercentage(
		PrimitiveTypes::UInt32 animationSetIndex,
		PrimitiveTypes::UInt32 animationIndex,
		PrimitiveTypes::UInt32 firstActiveAnimSlotIndex,
		PrimitiveTypes::UInt32 lastActiveAnimSlotIndex);

	PrimitiveTypes::Bool isRunningAnimation(
		PrimitiveTypes::UInt32 animationSetIndex,
		PrimitiveTypes::UInt32 animationIndex,
		PrimitiveTypes::UInt32 firstActiveAnimSlotIndex,
		PrimitiveTypes::UInt32 lastActiveAnimSlotIndex);

	void createAdditionalLocalTransforms();
	
	void setInstancedCSJobIndex(int index) {m_instanceCSJobIndex = index;}

	// each slot is an animation track
	// that is if multiple slots are active, multiple animations are blended together
	Array<AnimationSlot> m_animSlots;
	// additional transformations to apply to joins
	Array<Matrix4x4> m_additionalLocalTransforms;
	// control whether or not to apply additional transformations
	Array<PrimitiveTypes::Bool> m_additionalLocalTransformFlags;

	Array<Matrix4x4> m_curPalette; // transformation of each joint multiplied by inverse bind pose transform (to be passed to GPU)
	Array<Matrix4x4> m_modelSpacePalette;

	bool m_gpuAnimation;
	int m_debugAnimIdOffset;

	int m_instanceCSJobIndex;
};

}; // namespace Components
}; // namespace PE
#endif

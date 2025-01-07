#ifndef __pe_skeletoninstance_h__
#define __pe_skeletoninstance_h__

#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"

// Sibling/Children includes
#include "Mesh.h"

namespace PE {
namespace Components {


struct SkeletonInstance : public Component
{
	PE_DECLARE_CLASS(SkeletonInstance);

	SkeletonInstance(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself,
		PE::Handle hDefaultStateMachine);

	virtual ~SkeletonInstance(){}

	virtual void addDefaultComponents();

	void initFromFiles(const char *skeletonAssetName, const char *skeletonAssetPackage, int &threadOwnershipMask);
	void setAnimSet(const char *animsetAssetName, const char *animsetAssetPackage);
	Array<Handle> m_hAnimationSetGPUs;
	Handle m_hAnimationSM;
};

}; // namespace Components
}; // namespace PE
#endif

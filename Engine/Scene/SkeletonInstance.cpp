#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/APIAbstraction/GPUMaterial/GPUMaterialSet.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/APIAbstraction/Texture/Texture.h"
#include "PrimeEngine/APIAbstraction/Effect/EffectManager.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/VertexBufferGPUManager.h"
#include "PrimeEngine/Geometry/SkeletonCPU/AnimationSetGPUManager.h"
#include "PrimeEngine/Geometry/SkeletonCPU/SkeletonCPU.h"
#include "PrimeEngine/Lua/EventGlue/EventDataCreators.h"
#include "PrimeEngine/Scene/Skeleton.h"
#include "DefaultAnimationSM.h"
#include "Light.h"

#include "PrimeEngine/GameObjectModel/Camera.h"

// Sibling/Children includes
#include "SkeletonInstance.h"
#include "MeshManager.h"
#include "SceneNode.h"
#include "CameraManager.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"
namespace PE {
namespace Components{

PE_IMPLEMENT_CLASS1(SkeletonInstance, Component);

SkeletonInstance::SkeletonInstance(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself,
	Handle hDefaultStateMachine)
: Component(context, arena, hMyself)
, m_hAnimationSM(hDefaultStateMachine)
, m_hAnimationSetGPUs(context, arena, 8)
{
}


void SkeletonInstance::initFromFiles(const char *skeletonAssetName, const char *skeletonAssetPackage,
	int &threadOwnershipMask)
{
	Handle h = m_pContext->getMeshManager()->getAsset(skeletonAssetName, skeletonAssetPackage, threadOwnershipMask);

	static int allowedEvts[] = {0};
	// note we don't want to receive any events from this parent so we don't allow our event handlers to propagate to this parent
	// reason for this is that we will receive all events through the normal parent. this parent only needs this component for tracking purposes
	h.getObject<Component>()->addComponent(m_hMyself, &allowedEvts[0]);
}

void SkeletonInstance::setAnimSet(const char *animsetAssetName, const char *animsetAssetPackage)
{
	Skeleton *pSkel = getFirstParentByTypePtr<Skeleton>();
	Handle hAnimationSetGPU = AnimationSetGPUManager::Instance()->ReadAnimationSet(*m_pContext, m_arena, animsetAssetName, animsetAssetPackage, *pSkel->m_hSkeletonCPU.getObject<SkeletonCPU>());
	m_hAnimationSetGPUs.add(hAnimationSetGPU);
}

void SkeletonInstance::addDefaultComponents()
{
	if (m_hAnimationSM.isValid())
	{
		// animation state machine was provided
	}
	else
	{
		// use default state machine
		m_hAnimationSM = Handle("DefaultAnimationSM", sizeof(DefaultAnimationSM));
		DefaultAnimationSM *pASM = new(m_hAnimationSM) DefaultAnimationSM(*m_pContext, m_arena, m_hAnimationSM);
		pASM->addDefaultComponents();
	}
	addComponent(m_hAnimationSM);
	Component::addDefaultComponents();
}

}; // namespace Components
}; // namespace PE

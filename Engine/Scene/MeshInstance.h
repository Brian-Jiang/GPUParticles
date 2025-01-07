#ifndef __pe_meshinstance_h__
#define __pe_meshinstance_h__

#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"
#include "PrimeEngine/Math/Box.h"
#include "PrimeEngine/Math/Sphere.h"
#include "PrimeEngine/Scene/SceneNode.h"

// Sibling/Children includes
#include "Mesh.h"

namespace PE {
namespace Components {

struct MeshInstance : public Component
{
	PE_DECLARE_CLASS(MeshInstance);

	// Constructor -------------------------------------------------------------
	MeshInstance(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) ;

	void initFromFile(const char *assetName, const char *assetPackage,
		int &threadOwnershipMask);

	void initFromRegisteredAsset(const PE::Handle &h);

	virtual ~MeshInstance(){}

	virtual void addDefaultComponents();

	bool hasSkinWeights();

    SceneNode* getFirstParentSceneNode();

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_PRE_RENDER_needsRC)
	void do_PRE_RENDER_needsRC(PE::Events::Event* pEvt);

	void setDrawBoundingBox(const PE::Handle& sceneNodeHandle, const PE::Handle& mainSceneNodeHandle);
	void setUseSphereForCollision();
	Box getBoundingBox();
	void updateBoundingBox();
	Sphere getBoundingSphere();
    void MoveTo(Vector3 position);
	void FinalizePosition(Vector3 position);

	void AddParticleEmitter();

    bool m_culledOut;
	Handle m_hAsset;

	int m_skinDebugVertexId;

	bool m_drawBoundingBox;
	Handle m_sceneNode;
	Handle m_rootSceneNode;
	
	Box m_boundingBox;
	bool m_boundingBoxCreated;

	PE::Handle m_hPhysicsNode;
    Handle m_particleEmitter;
};

}; // namespace Components
}; // namespace PE
#endif

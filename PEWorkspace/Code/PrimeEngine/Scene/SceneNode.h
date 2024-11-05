#ifndef __PYENGINE_2_0_SCENENODE_H__
#define __PYENGINE_2_0_SCENENODE_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../Events/Component.h"
#include "../Utils/Array/Array.h"
#include "Light.h"

//#define USE_DRAW_COMPONENT

namespace PE {
namespace Components {
struct SceneNode : public Component
{
	PE_DECLARE_CLASS(SceneNode);

	// Constructor -------------------------------------------------------------
	SceneNode(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself);

	virtual ~SceneNode() {}

	// Component ------------------------------------------------------------

	virtual void addDefaultComponents();
	
	// Individual events -------------------------------------------------------

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_MOVE);
	virtual void do_MOVE(Events::Event *pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_CALCULATE_TRANSFORMATIONS);
	virtual void do_CALCULATE_TRANSFORMATIONS(Events::Event *pEvt);


	Handle m_hComponentParent;
	

	Matrix4x4 m_base; // local transform
	Matrix4x4 m_worldTransform; // is calculated bnefore every draw via Events::CALULCATE_TRANSFORMATIONS

	bool m_inheritPositionOnly;

	static SceneNode *s_pRootSceneNode;
	Array<Handle> m_lights;
}; // class SceneNode

struct JointSceneNode : public SceneNode
{
	PE_DECLARE_CLASS(JointSceneNode);

	JointSceneNode(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself, int myJoint) : SceneNode(context, arena, hMyself), m_myJoint(myJoint)
	{}

	virtual ~JointSceneNode() {}

	// looks up transformations form skin joint
	virtual void do_CALCULATE_TRANSFORMATIONS(Events::Event *pEvt) ;

	int m_myJoint;
};

}; // namespace Components
}; // namespace PE
#endif
